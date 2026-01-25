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

#include "r3d_outline.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

/* === Internal data structures === */

#define R3D_OUTLINE_INITIAL_CAPACITY 16

typedef struct R3D_OutlineEntry {
    const R3D_Model* model;
    R3D_OutlineConfig config;
    unsigned char* ignoredMeshes;
} R3D_OutlineEntry;

static struct {
    R3D_OutlineEntry* entries;
    int count;
    int capacity;
    bool initialized;
} g_outlineData = { 0 };

/* === Internal helper functions === */

static int r3d_outline_find_index(const R3D_Model* model) {
    for (int i = 0; i < g_outlineData.count; i++) {
        if (g_outlineData.entries[i].model == model) {
            return i;
        }
    }
    return -1;
}

static bool r3d_outline_ensure_capacity(void) {
    if (g_outlineData.count >= g_outlineData.capacity) {
        int newCapacity = g_outlineData.capacity == 0 ? R3D_OUTLINE_INITIAL_CAPACITY : g_outlineData.capacity * 2;
        R3D_OutlineEntry* newEntries = (R3D_OutlineEntry*)RL_REALLOC(
            g_outlineData.entries,
            newCapacity * sizeof(R3D_OutlineEntry)
        );
        
        if (!newEntries) {
            TraceLog(LOG_ERROR, "R3D_OUTLINE: Failed to allocate memory for outline entries");
            return false;
        }
        
        g_outlineData.entries = newEntries;
        g_outlineData.capacity = newCapacity;
    }
    return true;
}

/* === Public functions === */

void r3d_outline_init(void) {
    if (g_outlineData.initialized) {
        return;
    }
    
    g_outlineData.entries = NULL;
    g_outlineData.count = 0;
    g_outlineData.capacity = 0;
    g_outlineData.initialized = true;
    
    TraceLog(LOG_INFO, "R3D_OUTLINE: Outline system initialized");
}

void r3d_outline_shutdown(void) {
    if (!g_outlineData.initialized) {
        return;
    }
    
    if (g_outlineData.entries) {
        for (int i = 0; i < g_outlineData.count; i++) {
            if (g_outlineData.entries[i].ignoredMeshes) {
                RL_FREE(g_outlineData.entries[i].ignoredMeshes);
            }
        }
        RL_FREE(g_outlineData.entries);
        g_outlineData.entries = NULL;
    }
    
    g_outlineData.count = 0;
    g_outlineData.capacity = 0;
    g_outlineData.initialized = false;
    
    TraceLog(LOG_INFO, "R3D_OUTLINE: Outline system shutdown");
}

void r3d_outline_set_config(const R3D_Model* model, R3D_OutlineConfig config) {
    if (!g_outlineData.initialized) {
        r3d_outline_init();
    }
    
    if (!model) {
        TraceLog(LOG_WARNING, "R3D_OUTLINE: Cannot set config for NULL model");
        return;
    }
    
    int index = r3d_outline_find_index(model);
    
    if (index >= 0) {
        // Update existing entry
        g_outlineData.entries[index].config = config;
    } else {
        // Add new entry
        if (!r3d_outline_ensure_capacity()) {
            return;
        }
        
        g_outlineData.entries[g_outlineData.count].model = model;
        g_outlineData.entries[g_outlineData.count].config = config;
        g_outlineData.entries[g_outlineData.count].ignoredMeshes = NULL;
        g_outlineData.count++;
    }
}

bool r3d_outline_get_config(const R3D_Model* model, R3D_OutlineConfig* outConfig, const unsigned char** outIgnoredMeshes) {
    if (!g_outlineData.initialized || !model || !outConfig) {
        return false;
    }
    
    int index = r3d_outline_find_index(model);
    
    if (index >= 0) {
        *outConfig = g_outlineData.entries[index].config;
        if (outIgnoredMeshes) {
            *outIgnoredMeshes = g_outlineData.entries[index].ignoredMeshes;
        }
        return true;
    }
    
    return false;
}

void r3d_outline_add_ignore_index(const R3D_Model* model, int meshIndex) {
    if (!g_outlineData.initialized) {
        r3d_outline_init();
    }

    if (!model || meshIndex < 0 || meshIndex >= model->meshCount) {
        TraceLog(LOG_WARNING, "R3D_OUTLINE: Ignore index %d out of bounds (meshCount: %d)", meshIndex, model ? model->meshCount : 0);
        return;
    }

    int index = r3d_outline_find_index(model);

    if (index < 0) {
        // Create entry with default config if it doesn't exist
        R3D_OutlineConfig defaultConfig = { false, 0.05f, BLACK };
        r3d_outline_set_config(model, defaultConfig);
        index = r3d_outline_find_index(model);
    }

    if (index >= 0) {
        if (!g_outlineData.entries[index].ignoredMeshes) {
            g_outlineData.entries[index].ignoredMeshes = (unsigned char*)RL_CALLOC(model->meshCount, sizeof(unsigned char));
        }
        if (g_outlineData.entries[index].ignoredMeshes) {
            g_outlineData.entries[index].ignoredMeshes[meshIndex] = 1;
        }
    }
}

void r3d_outline_remove_config(const R3D_Model* model) {
    if (!g_outlineData.initialized || !model) {
        return;
    }
    
    int index = r3d_outline_find_index(model);
    
    if (index >= 0) {
        if (g_outlineData.entries[index].ignoredMeshes) {
            RL_FREE(g_outlineData.entries[index].ignoredMeshes);
        }
        // Remove entry by shifting remaining entries
        for (int i = index; i < g_outlineData.count - 1; i++) {
            g_outlineData.entries[i] = g_outlineData.entries[i + 1];
        }
        g_outlineData.count--;
    }
}
