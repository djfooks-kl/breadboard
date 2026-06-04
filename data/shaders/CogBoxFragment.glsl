#version 300 es
precision mediump float;

in vec2 vP1;
in vec2 vP2;
in vec2 vWorldPos;

uniform float u_Size;
uniform float u_Expand;
uniform float u_Feather;
uniform vec3 u_Color;
uniform vec3 u_FillColor;

out vec4 FragColor;

#define SQRT2 1.4142135

void main()
{
    float width = 0.05;

    vec2 size = vP2 - vP1;
    vec2 center = (vP1 + vP2) * 0.5;

    float borderExtras = width * 0.5 + u_Feather;
    float clampedBorder = min(u_Size + borderExtras, 0.65) - borderExtras;

    vec2 a = abs(vWorldPos - center) - size * 0.5;
    float dist = max(a.x, a.y);
    float inner = clampedBorder - width * 0.5;
    float outer = clampedBorder + width * 0.5 + u_Feather * u_Expand * SQRT2;
    float scale =       smoothstep(inner, inner + u_Feather, dist);
    float alpha = 1.0 - smoothstep(outer, outer + u_Feather, dist);

    vec3 c = mix(u_FillColor, u_Color, scale);
    FragColor = vec4(c, alpha);
}
