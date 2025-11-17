#version 300 es
precision mediump float;

layout (location = 0) in vec2 textureUV;
layout (location = 1) in vec2 p1;
layout (location = 2) in vec2 p2;
out vec2 vP1;
out vec2 vP2;
out vec2 vWorldPos;

uniform mat4 viewProjection;

void main()
{
    vP1 = p1;
    vP2 = p2;

    vec2 p = mix(p1 - vec2(1.5, 1.5), p2 + vec2(1.5, 1.5), textureUV);
    vWorldPos = p;
    gl_Position = viewProjection * vec4(p, 0.0, 1.0);
}