#version 300 es
precision mediump float;

layout (location = 0) in vec2 position;
out vec2 worldPos;

uniform mat4 viewProjection;
uniform vec4 box;

void main()
{
    vec2 p = mix(box.xy, box.zw, position);
    worldPos = p;
    gl_Position = viewProjection * vec4(p, 0.0, 1.0);
}