/*
 * Outline Fragment Shader
 * 
 * This shader renders a solid color for the outline.
 */

#version 330 core

// Layout for Deferred rendering (matching R3D G-Buffer)
// Layout for Deferred rendering (matching R3D G-Buffer)
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
in vec2 vTexCoord;
in float vShouldOutline;

// Uniforms
uniform vec4 uOutlineColor;
uniform sampler2D uTexAlbedo;

vec3 srgb_to_linear(vec3 c) {
    return pow(c, vec3(2.2));
}

void main() {
    vec4 texColor = texture(uTexAlbedo, vTexCoord);
    
    if (vShouldOutline > 0.5) {
        // Outline color with texture alpha
        FragColor = vec4(srgb_to_linear(uOutlineColor.rgb), texColor.a * uOutlineColor.a);
    } else {
        // Normal texture color (for backfaces of excluded/transparent areas)
        FragColor = vec4(srgb_to_linear(texColor.rgb), texColor.a);
    }
    
    // Discard transparent fragments to avoid writing to depth/color buffers
    if (FragColor.a < 0.01) discard;
}

