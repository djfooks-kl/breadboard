#version 300 es
precision mediump float;

uniform float u_Feather;
uniform vec2 u_WireTextureSize;
uniform mat4 u_ViewProjection;
uniform float u_OuterRadius;

uniform sampler2D wireTexture;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 wireUV;

out vec2 vP;
out vec2 vNode;
out float vWireValue;

void main()
{
    float r = u_OuterRadius + u_Feather;
    vec2 p1 = vec2(position.x - r, position.y - r);
    vec2 p2 = vec2(position.x + r, position.y + r);

    vec2 p = vec2(mix(p1.x, p2.x, uv.x), mix(p1.y, p2.y, uv.y));

    vWireValue = texture(wireTexture, (wireUV + vec2(0.5, 0.5)) / u_WireTextureSize).x;
    vP = p;
    vNode = position;
    vec4 mvPosition = vec4(p, 0.0, 1.0);
    gl_Position = u_ViewProjection * mvPosition;
}
