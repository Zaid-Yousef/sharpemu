/* Copyright (C) 2026 SharpEmu Emulator Project
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
# define SE_GPU_CALL __cdecl
# if defined(SE_GPU_BUILD)
#  define SE_GPU_API __declspec(dllexport)
# else
#  define SE_GPU_API __declspec(dllimport)
# endif
#else
# define SE_GPU_CALL
# define SE_GPU_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SE_GPU_ABI_VERSION 1u
typedef struct se_gpu_backend se_gpu_backend;
typedef enum se_gpu_result {
    SE_GPU_OK = 0, SE_GPU_NOT_FOUND = 1, SE_GPU_NOT_READY = 2,
    SE_GPU_INVALID_ARGUMENT = -1, SE_GPU_INCOMPATIBLE_ABI = -2,
    SE_GPU_PLATFORM_ERROR = -3, SE_GPU_VULKAN_ERROR = -4,
    SE_GPU_OUT_OF_MEMORY = -5, SE_GPU_INTERNAL_ERROR = -6
} se_gpu_result;
typedef struct se_gpu_bytes { const void* data; size_t size; } se_gpu_bytes;
typedef void (SE_GPU_CALL *se_gpu_log_fn)(int32_t level, const char* message, void* user);
typedef struct se_gpu_create_info {
    uint32_t struct_size, abi_version, width, height, enable_validation;
    const char* title_utf8;
    se_gpu_log_fn log; void* log_user;
} se_gpu_create_info;
typedef struct se_gpu_sampler { uint32_t words[4]; } se_gpu_sampler;
typedef struct se_gpu_texture {
    uint32_t struct_size; uint64_t address; uint32_t width, height, format, number_type;
    se_gpu_bytes rgba_pixels; uint32_t is_fallback, is_storage, mip_levels, mip_level;
    uint32_t pitch, tile_mode, dst_select; se_gpu_sampler sampler;
} se_gpu_texture;
typedef struct se_gpu_memory_buffer { uint64_t address; se_gpu_bytes data; } se_gpu_memory_buffer;
typedef struct se_gpu_vertex_buffer {
    uint32_t struct_size, location, component_count, data_format, number_format;
    uint64_t address; uint32_t stride, offset_bytes; se_gpu_bytes data;
} se_gpu_vertex_buffer;
typedef struct se_gpu_index_buffer { se_gpu_bytes data; uint32_t is_32_bit; } se_gpu_index_buffer;
typedef struct se_gpu_rect { int32_t x, y; uint32_t width, height; } se_gpu_rect;
typedef struct se_gpu_viewport { float x, y, width, height, min_depth, max_depth; } se_gpu_viewport;
typedef struct se_gpu_blend {
    uint32_t enable, color_src, color_dst, color_func, alpha_src, alpha_dst, alpha_func;
    uint32_t separate_alpha, write_mask;
} se_gpu_blend;
typedef struct se_gpu_render_target {
    uint32_t struct_size; uint64_t address; uint32_t width, height, format, number_type, mip_levels;
} se_gpu_render_target;
typedef struct se_gpu_draw {
    uint32_t struct_size, width, height; se_gpu_bytes vertex_spirv, pixel_spirv;
    const se_gpu_texture* textures; uint32_t texture_count;
    const se_gpu_memory_buffer* memory_buffers; uint32_t memory_buffer_count;
    const se_gpu_vertex_buffer* vertex_buffers; uint32_t vertex_buffer_count;
    const se_gpu_render_target* targets; uint32_t target_count;
    const se_gpu_blend* blends; uint32_t blend_count;
    const se_gpu_index_buffer* index_buffer; const se_gpu_rect* scissor;
    const se_gpu_viewport* viewport; uint32_t attribute_count, vertex_count, instance_count;
    uint32_t primitive_type, publish_targets;
} se_gpu_draw;
typedef struct se_gpu_compute {
    uint32_t struct_size; uint64_t shader_address; se_gpu_bytes spirv;
    const se_gpu_texture* textures; uint32_t texture_count;
    const se_gpu_memory_buffer* memory_buffers; uint32_t memory_buffer_count;
    uint32_t groups_x, groups_y, groups_z;
} se_gpu_compute;
typedef struct se_gpu_input {
    uint32_t struct_size, keyboard_focused, virtual_keys[8], gamepad_connected, gamepad_buttons;
    uint8_t left_x, left_y, right_x, right_y, left_trigger, right_trigger, reserved[2];
    char gamepad_name_utf8[128];
} se_gpu_input;

SE_GPU_API uint32_t SE_GPU_CALL se_gpu_abi_version(void);
SE_GPU_API const char* SE_GPU_CALL se_gpu_last_error(const se_gpu_backend* backend);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_create(const se_gpu_create_info*, se_gpu_backend**);
SE_GPU_API void SE_GPU_CALL se_gpu_destroy(se_gpu_backend*);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_poll(se_gpu_backend*, uint32_t* should_close);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_input_snapshot(se_gpu_backend*, se_gpu_input*);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_present_bgra(
    se_gpu_backend*, const void* pixels, size_t size, uint32_t width, uint32_t height, uint32_t pitch);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_submit_draw(se_gpu_backend*, const se_gpu_draw*);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_submit_compute(se_gpu_backend*, const se_gpu_compute*);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_register_display_buffer(se_gpu_backend*, uint64_t, uint32_t);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_present_guest_image(
    se_gpu_backend*, uint64_t, uint32_t, uint32_t, uint32_t);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_has_guest_image(
    se_gpu_backend*, uint64_t, uint32_t, uint32_t);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_blit_guest_image(
    se_gpu_backend*, uint64_t, uint32_t, uint32_t, uint32_t,
    uint64_t, uint32_t, uint32_t, uint32_t);
SE_GPU_API se_gpu_result SE_GPU_CALL se_gpu_render_target_output_kind(
    uint32_t format, uint32_t number_type, uint32_t* output_kind);

#ifdef __cplusplus
}
#endif
