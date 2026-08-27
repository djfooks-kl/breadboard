#version 300 es
precision mediump float;

layout (location = 0) in vec2 textureUV;
out vec2 vWorldPos;

uniform vec2 u_Min;
uniform vec2 u_Max;
uniform mat4 u_ViewProjection;

void main()
{
    vec2 p = mix(u_Min, u_Max, textureUV);
    vWorldPos = p;
    gl_Position = u_ViewProjection * vec4(p, 0.0, 1.0);
}