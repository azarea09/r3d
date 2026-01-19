/*
 * Outline Vertex Shader
 * 
 * This shader implements vertex extrusion for outline rendering.
 * The outline width remains constant regardless of camera distance.
 */

#version 330 core

// Vertex attributes
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aColor;
layout (location = 4) in vec4 aTangent;
layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aBoneWeights;


// Uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uOutlineWidth;

// Bone matrices for skinning (max 128 bones)
uniform mat4 uBoneMatrices[128];
uniform int uBoneCount;

void main() {
    vec4 worldPos;
    vec3 worldNormal;
    
    // Apply skinning if bones are present
    if (uBoneCount > 0) {
        mat4 boneTransform = mat4(0.0);
        
        for (int i = 0; i < 4; i++) {
            int boneId = int(aBoneIds[i]);
            float weight = aBoneWeights[i];
            
            if (boneId >= 0 && boneId < uBoneCount && weight > 0.0) {
                boneTransform += uBoneMatrices[boneId] * weight;
            }
        }
        
        // Apply bone transformation
        worldPos = uModel * boneTransform * vec4(aPosition, 1.0);
        worldNormal = normalize(mat3(uModel * boneTransform) * aNormal);
    } else {
        // No skinning
        worldPos = uModel * vec4(aPosition, 1.0);
        worldNormal = normalize(mat3(uModel) * aNormal);
    }
    
    // Calculate view-space position and normal
    vec4 viewPos = uView * worldPos;
    vec3 viewNormal = normalize(mat3(uView) * worldNormal);
    
    // Extrude vertex along view-space normal
    // This ensures constant outline width regardless of distance
    viewPos.xyz += viewNormal * uOutlineWidth;
    
    // Project to clip space
    gl_Position = uProjection * viewPos;
}
