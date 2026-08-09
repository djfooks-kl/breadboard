#version 300 es
precision mediump float;

layout (location = 0) in vec2 textureUV;
layout (location = 1) in vec2 p1;
layout (location = 2) in vec2 p2;
layout (location = 3) in vec2 vfxP1;
layout (location = 4) in vec2 vfxP2;
layout (location = 5) in float vfxF1;
layout (location = 6) in float vfxF2;
out vec2 vP1;
out vec2 vP2;
out vec2 vWorldPos;
out vec2 vVFXP1;
out vec2 vVFXP2;
out float vVFXF1;
out float vVFXF2;

uniform mat4 u_ViewProjection;

void main()
{
    vP1 = p1;
    vP2 = p2;

    vec2 p = mix(p1 - vec2(1.0, 1.0), p2 + vec2(1.0, 1.0), textureUV);
    vWorldPos = p;
    gl_Position = u_ViewProjection * vec4(p, 0.0, 1.0);
    vVFXP1 = vfxP1;
    vVFXP2 = vfxP2;
    vVFXF1 = vfxF1;
    vVFXF2 = vfxF2;
}