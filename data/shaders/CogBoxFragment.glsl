#version 300 es
precision mediump float;

in vec2 vP1;
in vec2 vP2;
in vec2 vWorldPos;

uniform float border;
uniform float expand;
uniform float feather;
uniform vec3 color;
uniform vec3 fillColor;

out vec4 FragColor;

#define SQRT2 1.4142135

void main()
{
    float width = 0.05;

    vec2 size = vP2 - vP1;
    vec2 center = (vP1 + vP2) * 0.5;

    float borderExtras = width * 0.5 + feather;
    float clampedBorder = min(border + borderExtras, 0.65) - borderExtras;

    vec2 a = abs(vWorldPos - center) - size * 0.5;
    float dist = max(a.x, a.y);
    float inner = clampedBorder - width * 0.5;
    float outer = clampedBorder + width * 0.5 + feather * expand * SQRT2;
    float scale =       smoothstep(inner, inner + feather, dist);
    float alpha = 1.0 - smoothstep(outer, outer + feather, dist);

    vec3 c = mix(fillColor, color, scale);
    FragColor = vec4(c, alpha);
}
