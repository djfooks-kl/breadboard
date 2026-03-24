#version 300 es
precision mediump float;

in vec2 vWorldPos;
in vec3 vBase;
in vec3 vP1;
in vec3 vP2;
in float vConnected;

out vec4 FragColor;

uniform float feather;
uniform float innerRadius;
uniform float outerRadius;
uniform float innerContactWidth;
uniform float outerContactWidth;
uniform vec3 outlineColor;

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

vec4 wire(vec3 base, vec3 pConnected)
{
    vec2 n = base.xy - pConnected.xy;
    float l = length(n);
    if (l == 0.0)
    {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }
    n = n / l;
    n = vec2(n.y, -n.x);
    vec2 wireOffset = vWorldPos - base.xy;
    float d = abs(dot(wireOffset, n));

    float minX = min(base.x, pConnected.x);
    float maxX = max(base.x, pConnected.x);
    float minY = min(base.y, pConnected.y);
    float maxY = max(base.y, pConnected.y);

    float outerBounds = outerContactWidth + feather;
    bool inouterContactWidth = vWorldPos.x >= minX - (outerBounds * step(1.0, abs(n.x))) &&
                       vWorldPos.x <= maxX + (outerBounds * step(1.0, abs(n.x))) &&
                       vWorldPos.y >= minY - (outerBounds * step(1.0, abs(n.y))) &&
                       vWorldPos.y <= maxY + (outerBounds * step(1.0, abs(n.y)));
    if (!inouterContactWidth)
    {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }

    float v = (d - outerContactWidth) / feather;
    float alpha = 1.0 - v;

    v = (d - innerContactWidth) / feather;

    float wireValue = sign(base.z + pConnected.z);

    vec3 wireColor = getWireColor(wireValue);
    wireColor = mix(wireColor, outlineColor, max(min(v, 1.0), 0.0));

    return vec4(wireColor.rgb, alpha);
}

void main(void)
{
    float outerR = outerRadius - feather * 0.5;
    float innerR = innerRadius - feather * 0.5;

    vec2 offsetBase = vWorldPos - vBase.xy;
    float dBase = length(offsetBase);
    vec2 offset1 = vWorldPos - vP1.xy;
    float d1 = length(offset1);
    vec2 offset2 = vWorldPos - vP2.xy;
    float d2 = length(offset2);

    float is3Pin = length(vBase - vP1);

    // bg
    float alphaBg = (1.0 - ((dBase - outerR) / feather)) * is3Pin;
    alphaBg = max(alphaBg, 1.0 - ((d1 - outerR) / feather));
    alphaBg = max(alphaBg, 1.0 - ((d2 - outerR) / feather));
    vec4 color = vec4(outlineColor, alphaBg);

    // wire
    vec4 wireColor = wire(vBase, vConnected == 1.0 ? vP2 : vP1);
    color = blend(color, wireColor.rgb, wireColor.a);

    // fg
    float alphaFgBase = (1.0 - ((dBase - innerR) / feather)) * is3Pin;
    vec3 fgColorBase = getWireColor(vBase.z);
    color = blend(color, fgColorBase, alphaFgBase);

    float alphaFg1 = 1.0 - ((d1 - innerR) / feather);
    vec3 fgColor1 = getWireColor(vP1.z);
    color = blend(color, fgColor1, alphaFg1);

    float alphaFg2 = 1.0 - ((d2 - innerR) / feather);
    vec3 fgColor2 = getWireColor(vP2.z);
    color = blend(color, fgColor2, alphaFg2);

    FragColor = color;
}
