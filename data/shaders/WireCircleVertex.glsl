#version 300 es
precision mediump float;

uniform float u_Feather;
uniform mat4 u_ViewProjection;
uniform float u_Size;
uniform float u_HasInfoTexture;
uniform vec2 u_WireTextureSize;

uniform sampler2D wireTexture;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 wireUV;

out vec2 vP;
out vec2 vCircleCenter;
out vec2 vUV;
out float vWireValue;

void main()
{
    float r = u_Size + u_Feather * 2.0;
    vec2 p1 = vec2(position.x - r, position.y - r);
    vec2 p2 = vec2(position.x + r, position.y + r);

    vec2 p = vec2(mix(p1.x, p2.x, uv.x), mix(p1.y, p2.y, uv.y));

    vP = p;
    vCircleCenter = position;
    vUV = uv;
    vWireValue = u_HasInfoTexture * texture(wireTexture, (wireUV + vec2(0.5, 0.5)) / u_WireTextureSize).x;

    vec4 mvPosition = vec4(p, 0.0, 1.0);
    gl_Position = u_ViewProjection * mvPosition;
}
