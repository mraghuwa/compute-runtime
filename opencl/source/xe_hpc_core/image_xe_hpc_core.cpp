/*
 * Copyright (C) 2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/xe_hpc_core/hw_cmds_base.h"

#include "opencl/source/mem_obj/image.inl"

namespace NEO {

using Family = XE_HPC_COREFamily;
static auto gfxCore = IGFX_XE_HPC_CORE;
} // namespace NEO
#include "opencl/source/mem_obj/image_tgllp_and_later.inl"
#include "opencl/source/mem_obj/image_xehp_and_later.inl"

// factory initializer
#include "opencl/source/mem_obj/image_factory_init.inl"