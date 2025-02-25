#pragma once

#include <kinc/global.h>

#include <kinc/image.h>

#include "rendertarget.h"
#include "shader.h"
#include "texture.h"
#include "textureunit.h"
#include "vertexstructure.h"

#include <kinc/backend/graphics5/graphics.h>

#include <kinc/math/matrix.h>
#include <kinc/math/vector.h>

/*! \file graphics.h
    \brief Contains the base G5-functionality.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kinc_g5_texture_addressing {
	KINC_G5_TEXTURE_ADDRESSING_REPEAT,
	KINC_G5_TEXTURE_ADDRESSING_MIRROR,
	KINC_G5_TEXTURE_ADDRESSING_CLAMP,
	KINC_G5_TEXTURE_ADDRESSING_BORDER
} kinc_g5_texture_addressing_t;

typedef enum kinc_g5_texture_filter {
	KINC_G5_TEXTURE_FILTER_POINT,
	KINC_G5_TEXTURE_FILTER_LINEAR,
	KINC_G5_TEXTURE_FILTER_ANISOTROPIC
} kinc_g5_texture_filter_t;

typedef enum kinc_g5_mipmap_filter {
	KINC_G5_MIPMAP_FILTER_NONE,
	KINC_G5_MIPMAP_FILTER_POINT,
	KINC_G5_MIPMAP_FILTER_LINEAR // linear texture filter + linear mip filter -> trilinear filter
} kinc_g5_mipmap_filter_t;

typedef enum kinc_g5_compare_mode {
	KINC_G5_COMPARE_MODE_ALWAYS,
	KINC_G5_COMPARE_MODE_NEVER,
	KINC_G5_COMPARE_MODE_EQUAL,
	KINC_G5_COMPARE_MODE_NOT_EQUAL,
	KINC_G5_COMPARE_MODE_LESS,
	KINC_G5_COMPARE_MODE_LESS_EQUAL,
	KINC_G5_COMPARE_MODE_GREATER,
	KINC_G5_COMPARE_MODE_GREATER_EQUAL
} kinc_g5_compare_mode_t;

typedef enum kinc_g5_cull_mode { KINC_G5_CULL_MODE_CLOCKWISE, KINC_G5_CULL_MODE_COUNTERCLOCKWISE, KINC_G5_CULL_MODE_NEVER } kinc_g5_cull_mode_t;

typedef enum kinc_g5_texture_direction { KINC_G5_TEXTURE_DIRECTION_U, KINC_G5_TEXTURE_DIRECTION_V, KINC_G5_TEXTURE_DIRECTION_W } kinc_g5_texture_direction_t;

/*typedef enum kinc_g5_render_target_format {
    KINC_G5_RENDER_TARGET_FORMAT_32BIT,
    KINC_G5_RENDER_TARGET_FORMAT_64BIT_FLOAT,
    KINC_G5_RENDER_TARGET_FORMAT_32BIT_RED_FLOAT,
    KINC_G5_RENDER_TARGET_FORMAT_128BIT_FLOAT,
    KINC_G5_RENDER_TARGET_FORMAT_16BIT_DEPTH,
    KINC_G5_RENDER_TARGET_FORMAT_8BIT_RED
} kinc_g5_render_target_format_t;*/
// typedef kinc_g4_render_target_format_t kinc_g5_render_target_format_t;

typedef enum kinc_g5_stencil_action {
	KINC_G5_STENCIL_ACTION_KEEP,
	KINC_G5_STENCIL_ACTION_ZERO,
	KINC_G5_STENCIL_ACTION_REPLACE,
	KINC_G5_STENCIL_ACTION_INCREMENT,
	KINC_G5_STENCIL_ACTION_INCREMENT_WRAP,
	KINC_G5_STENCIL_ACTION_DECREMENT,
	KINC_G5_STENCIL_ACTION_DECREMENT_WRAP,
	KINC_G5_STENCIL_ACTION_INVERT
} kinc_g5_stencil_action_t;

typedef enum kinc_g5_texture_operation {
	KINC_G5_TEXTURE_OPERATION_MODULATE,
	KINC_G5_TEXTURE_OPERATION_SELECT_FIRST,
	KINC_G5_TEXTURE_OPERATION_SELECT_SECOND
} kinc_g5_texture_operation_t;

typedef enum kinc_g5_texture_argument { KINC_G5_TEXTURE_ARGUMENT_CURRENT_COLOR, KINC_G5_TEXTURE_ARGUMENT_TEXTURE_COLOR } kinc_g5_texture_argument_t;

#define KINC_G5_CLEAR_COLOR 1
#define KINC_G5_CLEAR_DEPTH 2
#define KINC_G5_CLEAR_STENCIL 4

KINC_FUNC extern bool kinc_g5_fullscreen;

/// <summary>
/// I think this does nothing.
/// </summary>
KINC_FUNC void kinc_g5_flush(void);

/// <summary>
/// Assigns a texture to a texture-unit for sampled access.
/// </summary>
/// <param name="unit">The unit to assign this texture to</param>
/// <param name="texture">The texture to assign to the unit</param>
KINC_FUNC void kinc_g5_set_texture(kinc_g5_texture_unit_t unit, kinc_g5_texture_t *texture);

/// <summary>
/// Assigns a texture to a texture-unit for direct access.
/// </summary>
/// <param name="unit">The unit to assign this texture to</param>
/// <param name="texture">The texture to assign to the unit</param>
KINC_FUNC void kinc_g5_set_image_texture(kinc_g5_texture_unit_t unit, kinc_g5_texture_t *texture);

/// <summary>
/// Returns the currently used number of samples for hardware-antialiasing.
/// </summary>
/// <returns>The number of samples</returns>
KINC_FUNC int kinc_g5_antialiasing_samples(void);

/// <summary>
/// Sets the number of samples used for hardware-antialiasing. This typically uses multisampling and typically only works with a few specific numbers of
/// sample-counts - 2 and 4 are pretty save bets. It also might do nothing at all.
/// </summary>
/// <param name="samples">The number of samples</param>
KINC_FUNC void kinc_g5_set_antialiasing_samples(int samples);

/// <summary>
/// Returns whether textures are supported which have widths/heights which are not powers of two - which is always the case.
/// </summary>
/// <returns>True, always</returns>
KINC_FUNC bool kinc_g5_non_pow2_textures_qupported(void);

KINC_FUNC bool kinc_g5_render_targets_inverted_y(void);

KINC_FUNC void kinc_g5_set_render_target_face(kinc_g5_render_target_t *texture, int face);

/// <summary>
/// Needs to be called before rendering to a window. Typically called at the start of each frame.
/// </summary>
/// <param name="window">The window to render to</param>
KINC_FUNC void kinc_g5_begin(kinc_g5_render_target_t *renderTarget, int window);

/// <summary>
/// Needs to be called after rendering to a window. Typically called at the end of each frame.
/// </summary>
/// <param name="window">The window to render to</param>
/// <returns></returns>
KINC_FUNC void kinc_g5_end(int window);

/// <summary>
/// Needs to be called to make the rendered frame visible. Typically called at the very end of each frame.
/// </summary>
KINC_FUNC bool kinc_g5_swap_buffers(void);

KINC_FUNC void kinc_g5_set_texture_addressing(kinc_g5_texture_unit_t unit, kinc_g5_texture_direction_t dir, kinc_g5_texture_addressing_t addressing);

/// <summary>
/// Set the texture-sampling-mode for upscaled textures.
/// </summary>
/// <param name="unit">The texture-unit to set the texture-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KINC_FUNC void kinc_g5_set_texture_magnification_filter(kinc_g5_texture_unit_t texunit, kinc_g5_texture_filter_t filter);

/// <summary>
/// Set the texture-sampling-mode for downscaled textures.
/// </summary>
/// <param name="unit">The texture-unit to set the texture-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KINC_FUNC void kinc_g5_set_texture_minification_filter(kinc_g5_texture_unit_t texunit, kinc_g5_texture_filter_t filter);

/// <summary>
/// Sets the mipmap-sampling-mode which defines whether mipmaps are used at all and if so whether the two neighbouring mipmaps are linearly interoplated.
/// </summary>
/// <param name="unit">The texture-unit to set the mipmap-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KINC_FUNC void kinc_g5_set_texture_mipmap_filter(kinc_g5_texture_unit_t texunit, kinc_g5_mipmap_filter_t filter);

KINC_FUNC void kinc_g5_set_texture_operation(kinc_g5_texture_operation_t operation, kinc_g5_texture_argument_t arg1, kinc_g5_texture_argument_t arg2);

/// <summary>
/// Returns how many textures can be used at the same time in a fragment-shader.
/// </summary>
/// <returns>The number of textures</returns>
KINC_FUNC int kinc_g5_max_bound_textures(void);

// Occlusion Query
KINC_FUNC bool kinc_g5_init_occlusion_query(unsigned *occlusionQuery);
KINC_FUNC void kinc_g5_delete_occlusion_query(unsigned occlusionQuery);
KINC_FUNC void kinc_g5_render_occlusion_query(unsigned occlusionQuery, int triangles);
KINC_FUNC bool kinc_g5_are_query_results_available(unsigned occlusionQuery);
KINC_FUNC void kinc_g5_get_query_result(unsigned occlusionQuery, unsigned *pixelCount);

#ifndef KINC_DOCS
void kinc_g5_internal_init(void);
void kinc_g5_internal_init_window(int window, int depth_buffer_bits, int stencil_buffer_bits, bool vsync);
void kinc_g5_internal_destroy_window(int window);
void kinc_g5_internal_destroy(void);
#endif

#ifdef __cplusplus
}
#endif
