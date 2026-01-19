/*
 * Copyright (c) 2025 Le Juez Victor
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that you
 *   wrote the original software. If you use this software in a product, an acknowledgment
 *   in the product documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *   as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 */

#ifndef R3D_OUTLINE_H
#define R3D_OUTLINE_H

#include "r3d.h"
#include <stdbool.h>

/* === Outline configuration structure === */

typedef struct R3D_OutlineConfig {
    bool enabled;       // Whether outline is enabled for this model
    float width;        // Outline width in world units
    Color color;        // Outline color
} R3D_OutlineConfig;

/* === Outline management functions === */

/**
 * @brief Initialize the outline system
 */
void r3d_outline_init(void);

/**
 * @brief Shutdown the outline system and free resources
 */
void r3d_outline_shutdown(void);

/**
 * @brief Set outline configuration for a model
 * @param model Pointer to the model
 * @param config Outline configuration
 */
void r3d_outline_set_config(const R3D_Model* model, R3D_OutlineConfig config);

/**
 * @brief Get outline configuration for a model
 * @param model Pointer to the model
 * @param outConfig Pointer to store the configuration
 * @return true if configuration exists, false otherwise
 */
bool r3d_outline_get_config(const R3D_Model* model, R3D_OutlineConfig* outConfig);

/**
 * @brief Remove outline configuration for a model
 * @param model Pointer to the model
 */
void r3d_outline_remove_config(const R3D_Model* model);

#endif // R3D_OUTLINE_H
