/*
 * Copyright (C) 2021-2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/debugger/debugger_l0.inl"
#include "shared/source/debugger/debugger_l0_tgllp_and_later.inl"
namespace NEO {

using Family = NEO::XE_HPG_COREFamily;

template class DebuggerL0Hw<Family>;
static DebuggerL0PopulateFactory<IGFX_XE_HPG_CORE, Family> debuggerXeHpgCore;
} // namespace NEO