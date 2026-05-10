#version 300 es
precision mediump float;

uniform float u_Feather;
uniform float u_InnerRadius;
uniform float u_OuterRadius;
uniform vec3 u_RingColor;

in vec2 vP;
in vec2 vNode;
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

void main(void)
{
    float outerR = u_OuterRadius - u_Feather * 0.5;
    float innerR = u_InnerRadius - u_Feather * 0.5;

    vec2 offset = vP - vNode.xy;
    float d = length(offset);

    float alphaBg = 1.0 - ((d - outerR) / u_Feather);
    vec4 color = vec4(u_RingColor.rgb, alphaBg);

    float alphaFg = 1.0 - ((d - innerR) / u_Feather);
    vec3 fgColor = getWireColor(vWireValue);
    color = blend(color, fgColor, alphaFg);

    FragColor = color;
}
