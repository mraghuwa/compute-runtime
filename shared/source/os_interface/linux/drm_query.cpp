/*
 * Copyright (C) 2020-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/helpers/hw_helper.h"
#include "shared/source/helpers/string.h"
#include "shared/source/os_interface/linux/cache_info_impl.h"
#include "shared/source/os_interface/linux/drm_engine_mapper.h"
#include "shared/source/os_interface/linux/engine_info_impl.h"
#include "shared/source/os_interface/linux/local_memory_helper.h"
#include "shared/source/os_interface/linux/memory_info_impl.h"
#include "shared/source/os_interface/linux/sys_calls.h"

#include "drm_neo.h"
#include "drm_query_flags.h"

#include <fstream>

namespace NEO {

namespace IoctlHelper {
std::string getIoctlStringRemaining(unsigned long request) {
    return std::to_string(request);
}

std::string getIoctlParamStringRemaining(int param) {
    return std::to_string(param);
}
} // namespace IoctlHelper

bool Drm::queryEngineInfo(bool isSysmanEnabled) {
    auto length = 0;
    auto dataQuery = this->query(DRM_I915_QUERY_ENGINE_INFO, DrmQueryItemFlags::empty, length);
    auto data = reinterpret_cast<drm_i915_query_engine_info *>(dataQuery.get());
    if (data) {
        this->engineInfo.reset(new EngineInfoImpl(data->engines, data->num_engines));
        return true;
    }
    return false;
}

bool Drm::queryMemoryInfo() {
    auto pHwInfo = getRootDeviceEnvironment().getHardwareInfo();
    auto isLocalMemSupported = HwHelper::get(pHwInfo->platform.eRenderCoreFamily).getEnableLocalMemory(*pHwInfo);
    if (!isLocalMemSupported) {
        return true;
    }

    auto length = 0;
    auto dataQuery = this->query(DRM_I915_QUERY_MEMORY_REGIONS, DrmQueryItemFlags::empty, length);
    if (dataQuery) {
        auto localMemHelper = LocalMemoryHelper::get(pHwInfo->platform.eProductFamily);
        auto data = localMemHelper->translateIfRequired(dataQuery.release(), length);
        auto memRegions = reinterpret_cast<drm_i915_query_memory_regions *>(data.get());
        this->memoryInfo.reset(new MemoryInfoImpl(memRegions->regions, memRegions->num_regions));
        return true;
    }
    return false;
}

unsigned int Drm::bindDrmContext(uint32_t drmContextId, uint32_t deviceIndex, aub_stream::EngineType engineType, bool engineInstancedDevice) {
    return DrmEngineMapper::engineNodeMap(engineType);
}

int Drm::createDrmVirtualMemory(uint32_t &drmVmId) {
    drm_i915_gem_vm_control ctl = {};
    auto ret = SysCalls::ioctl(getFileDescriptor(), DRM_IOCTL_I915_GEM_VM_CREATE, &ctl);
    if (ret == 0) {
        if (ctl.vm_id == 0) {
            // 0 is reserved for invalid/unassigned ppgtt
            return -1;
        }
        drmVmId = ctl.vm_id;
    }
    return ret;
}

bool Drm::queryTopology(const HardwareInfo &hwInfo, QueryTopologyData &topologyData) {
    int32_t length;
    auto dataQuery = this->query(DRM_I915_QUERY_TOPOLOGY_INFO, DrmQueryItemFlags::topology, length);
    auto data = reinterpret_cast<drm_i915_query_topology_info *>(dataQuery.get());

    if (!data) {
        return false;
    }

    topologyData.maxSliceCount = data->max_slices;
    topologyData.maxSubSliceCount = data->max_subslices;
    topologyData.maxEuCount = data->max_eus_per_subslice;

    TopologyMapping mapping;
    auto result = translateTopologyInfo(data, topologyData, mapping);
    this->topologyMap[0] = mapping;
    return result;
}

bool Drm::isDebugAttachAvailable() {
    return false;
}

bool Drm::querySystemInfo() {
    return false;
}

void Drm::setupSystemInfo(HardwareInfo *hwInfo, SystemInfo *sysInfo) {}

void Drm::setupCacheInfo(const HardwareInfo &hwInfo) {
    this->cacheInfo.reset(new CacheInfoImpl());
}

int Drm::bindBufferObject(OsContext *osContext, uint32_t vmHandleId, BufferObject *bo) {
    return 0;
}

int Drm::unbindBufferObject(OsContext *osContext, uint32_t vmHandleId, BufferObject *bo) {
    return 0;
}

void Drm::waitForBind(uint32_t vmHandleId) {
}

int Drm::waitUserFence(uint32_t ctx, uint64_t address, uint64_t value, ValueWidth dataWidth, int64_t timeout, uint16_t flags) {
    return 0;
}

bool Drm::isVmBindAvailable() {
    return this->bindAvailable;
}

uint32_t Drm::createDrmContextExt(drm_i915_gem_context_create_ext &gcc, uint32_t drmVmId, bool isSpecialContextRequested) {
    return ioctl(DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT, &gcc);
}

void Drm::appendDrmContextFlags(drm_i915_gem_context_create_ext &gcc, bool isSpecialContextRequested) {
}

void Drm::queryPageFaultSupport() {
}

bool Drm::hasPageFaultSupport() const {
    return false;
}

} // namespace NEO
