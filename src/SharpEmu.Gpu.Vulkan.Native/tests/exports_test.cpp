/* Copyright (C) 2026 SharpEmu Emulator Project
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "sharpemu_gpu_vulkan.h"

int main() {
    if (se_gpu_abi_version() != SE_GPU_ABI_VERSION) return 1;

    uint32_t output_kind{};
    if (se_gpu_render_target_output_kind(10, 4, &output_kind) != SE_GPU_OK || output_kind != 1) return 2;
    if (se_gpu_render_target_output_kind(10, 5, &output_kind) != SE_GPU_OK || output_kind != 2) return 3;
    if (se_gpu_render_target_output_kind(0xffffffffu, 0, &output_kind) != SE_GPU_NOT_FOUND) return 4;

    return 0;
}
