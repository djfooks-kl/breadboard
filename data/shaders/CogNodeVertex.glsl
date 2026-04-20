#version 300 es
precision mediump float;

uniform float feather;
uniform vec2 wireTextureSize;
uniform mat4 viewProjection;
uniform float radius;

uniform sampler2D wireTexture;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 wireUV;

out vec2 vP;
out vec2 vNode;
out float vWireValue;

void main()
{
    float r = radius + feather;
    vec2 p1 = vec2(position.x - r, position.y - r);
    vec2 p2 = vec2(position.x + r, position.y + r);

    vec2 p = vec2(mix(p1.x, p2.x, uv.x), mix(p1.y, p2.y, uv.y));

    vWireValue = texture(wireTexture, (wireUV + vec2(0.5, 0.5)) / wireTextureSize).x;
    vP = p;
    vNode = position;
    vec4 mvPosition = vec4(p, 0.0, 1.0);
    gl_Position = viewProjection * mvPosition;
}
