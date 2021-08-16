/*
 * Copyright (C) 2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/helpers/engine_node_helper.h"

#include "opencl/test/unit_test/fixtures/cl_device_fixture.h"
#include "test.h"

using namespace NEO;

using EngineNodeHelperTestsXeHPPlus = ::Test<ClDeviceFixture>;

HWCMDTEST_F(IGFX_XE_HP_CORE, EngineNodeHelperTestsXeHPPlus, WhenGetBcsEngineTypeIsCalledThenBcsEngineIsReturned) {
    const auto hwInfo = pDevice->getHardwareInfo();
    auto &selectorCopyEngine = pDevice->getDeviceById(0)->getSelectorCopyEngine();
    EXPECT_EQ(aub_stream::EngineType::ENGINE_BCS, EngineHelpers::getBcsEngineType(hwInfo, selectorCopyEngine, false));
}