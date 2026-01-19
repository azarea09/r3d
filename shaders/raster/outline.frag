/*
 * Outline Fragment Shader
 * 
 * This shader renders a solid color for the outline.
 */

#version 330 core

// Layout for Deferred rendering (matching R3D G-Buffer)
layout(location = 0) out vec4 FragColor;

// Uniforms
uniform vec4 uOutlineColor;

void main() {
    // We output the outline color directly to the current target (usually ping-pong)
    FragColor = uOutlineColor;
}

