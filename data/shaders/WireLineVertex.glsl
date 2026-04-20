#version 300 es
precision mediump float;

uniform mat4 viewProjection;

layout (location = 0) in vec2 p1;
layout (location = 1) in vec2 p2;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec2 wireUV1;
layout (location = 4) in vec2 wireUV2;

out vec2 vP;
out vec2 vP1;
out vec2 vP2;
out vec2 vWireUV;

void main()
{
    vP1 = p1;
    vP2 = p2;

    vec2 o = p1 - p2;
    float d = length(o);
    vec2 n = o / d;
    vec2 m = vec2(n.y, -n.x);

    vec2 p = mix(vP1, vP2, uv.x);
    p += mix(-m, m, uv.y);

    float uv = (dot(vP1 - p, n) + 0.5) / d;
    vWireUV = mix(wireUV1, wireUV2, uv);

    vP = p;
    vec4 mvPosition = vec4(p, 0.0, 1.0);
    gl_Position = viewProjection * mvPosition;
}
