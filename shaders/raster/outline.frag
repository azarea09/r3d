/*
 * Outline Fragment Shader
 * 
 * This shader renders a solid color for the outline.
 */

#version 330 core

// Layout for Deferred rendering (matching R3D G-Buffer)
layout(location = 0) out vec3 FragAlbedo;
layout(location = 1) out vec3 FragEmission;
layout(location = 2) out vec2 FragNormal;
layout(location = 3) out vec3 FragORM;

// Uniforms
uniform vec4 uOutlineColor;

void main() {
    // Forward rendering (location 0)
    // Deferred rendering:
    // We want the outline to be unlit, so we put the color in Emission and 0 in Albedo
    
    FragAlbedo = vec3(0.0);
    FragEmission = uOutlineColor.rgb;
    FragNormal = vec2(0.5, 0.5); // Default octahedral normal
    FragORM = vec3(1.0, 1.0, 0.0).rgb; // Occlusion 1, Roughness 1, Metalness 0
    
    // For single target rendering (Forward), location 0 is used.
    // In R3D's forward pass, it uses location 0.
    // BUT if we output to 0 as vec3(0.0), it will be black!
    // We need to know if we are in forward or deferred.
}

