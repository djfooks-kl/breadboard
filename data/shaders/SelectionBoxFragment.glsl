#version 300 es
precision mediump float;

in vec2 vWorldPos;

uniform vec2 u_Min;
uniform vec2 u_Max;
uniform vec3 u_EdgeColor;
uniform vec3 u_FillColor;
uniform float u_FillAlpha;
uniform float u_EdgeWidth;
uniform float u_Feather;

out vec4 FragColor;

#define SQRT2 1.4142135

void main()
{
    vec2 edgeDistances = min(abs(u_Min - vWorldPos), abs(u_Max - vWorldPos));
    float edgeDistance = min(edgeDistances.x, edgeDistances.y);
    if (edgeDistance < u_Feather * u_EdgeWidth)
    {
        FragColor = vec4(u_EdgeColor, 1.0);
    }
    else
    {
        FragColor = vec4(u_FillColor, u_FillAlpha);
    }
}
