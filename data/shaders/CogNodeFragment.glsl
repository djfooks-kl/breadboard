#version 300 es
precision mediump float;

uniform float feather;
uniform float radius;
uniform vec3 ringColor;

in vec2 vP;
in vec2 vCircle;
in float vWireValue;
out vec4 FragColor;

vec4 blend(vec4 color, vec3 inColor, float alpha)
{
    alpha = clamp(alpha, 0.0, 1.0);
    float outAlpha = color.a + alpha * (1.0 - color.a);
    if (outAlpha == 0.0)
    {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }
    vec3 v0 = inColor * alpha;
    vec3 v1 = color.rgb * color.a * (1.0 - alpha);
    vec3 v2 = (v0 + v1) / outAlpha;
    return vec4(v2, outAlpha);
}

vec3 getWireColor(float wireValue)
{
    return mix(vec3(1.0, 1.0, 1.0), vec3(1.0, 0.53, 0.53), wireValue);
}

void main(void) {
    const float innerRadius = 0.26;
    const float outerRadius = 0.31;
    float outerR = outerRadius - feather * 0.5;
    float innerR = innerRadius - feather * 0.5;

    vec2 offset = vP - vCircle.xy;
    float d = length(offset);

    float alphaBg = 1.0 - ((d - outerR) / feather);
    vec4 color = vec4(ringColor.rgb, alphaBg);

    float alphaFg = 1.0 - ((d - innerR) / feather);
    vec3 fgColor = getWireColor(vWireValue);
    color = blend(color, fgColor, alphaFg);

    FragColor = color;
}
