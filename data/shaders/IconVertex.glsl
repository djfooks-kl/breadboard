#version 300 es
precision mediump float;

uniform float feather;
uniform mat4 viewProjection;
uniform float iconSize;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 offset;
layout (location = 2) in vec2 textureUV;
layout (location = 3) in vec3 color;

out vec2 vUV;
out vec3 vColor;

void main()
{
    float r = (iconSize * 0.5) + feather;
    vec2 p1 = vec2(position.x - r, position.y - r);
    vec2 p2 = vec2(position.x + r, position.y + r);

    vec2 vertexP = vec2(mix(p1.x, p2.x, offset.x), mix(p1.y, p2.y, offset.y));

    vUV = textureUV - vec2(0.5, 0.5);
    vColor = color;
    vec4 mvPosition = vec4(vertexP, 0.0, 1.0);
    gl_Position = viewProjection * mvPosition;
}
