/*
 * Copyright (C) 2019-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/direct_submission/direct_submission_controller.h"

#include "shared/source/command_stream/command_stream_receiver.h"

#include <chrono>

namespace NEO {

DirectSubmissionController::DirectSubmissionController() {
    timeout = 5;

    if (DebugManager.flags.DirectSubmissionControllerTimeout.get() != -1) {
        timeout = DebugManager.flags.DirectSubmissionControllerTimeout.get();
    }

    directSubmissionControllingThread = std::thread(&DirectSubmissionController::controlDirectSubmissionsState, this);
};

DirectSubmissionController::~DirectSubmissionController() {
    keepControlling.store(false);
    if (directSubmissionControllingThread.joinable()) {
        directSubmissionControllingThread.join();
    }
}

void DirectSubmissionController::registerDirectSubmission(CommandStreamReceiver *csr) {
    std::lock_guard<std::mutex> lock(directSubmissionsMutex);
    directSubmissions.insert(std::make_pair(csr, DirectSubmissionState{}));
}

void DirectSubmissionController::unregisterDirectSubmission(CommandStreamReceiver *csr) {
    std::lock_guard<std::mutex> lock(directSubmissionsMutex);
    directSubmissions.erase(csr);
}

void DirectSubmissionController::controlDirectSubmissionsState() {
    while (true) {

        auto start = std::chrono::steady_clock::now();
        int diff = 0u;
        do {
            if (!keepControlling.load()) {
                return;
            }

            auto timestamp = std::chrono::steady_clock::now();
            diff = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - start).count());
        } while (diff <= timeout);

        this->checkNewSubmissions();
    }
}

void DirectSubmissionController::checkNewSubmissions() {
    std::lock_guard<std::mutex> lock(this->directSubmissionsMutex);

    for (auto &directSubmission : this->directSubmissions) {
        auto csr = directSubmission.first;
        auto &state = directSubmission.second;

        auto taskCount = csr->peekTaskCount();
        if (taskCount <= *csr->getTagAddress()) {
            if (taskCount == state.taskCount) {
                if (state.isStopped) {
                    continue;
                } else {
                    auto lock = csr->obtainUniqueOwnership();
                    csr->stopDirectSubmission();
                    state.isStopped = true;
                }
            } else {
                state.isStopped = false;
                state.taskCount = taskCount;
            }
        } else {
            state.isStopped = false;
        }
    }
}

} // namespace NEO