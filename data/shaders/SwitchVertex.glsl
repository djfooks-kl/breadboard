#version 300 es
precision mediump float;

layout (location = 0) in vec2 baseP;
layout (location = 1) in vec2 p1;
layout (location = 2) in vec2 p2;
layout (location = 3) in vec2 uv;
layout (location = 4) in vec2 wireUV;

out vec3 vBase;
out vec3 vP1;
out vec3 vP2;
out vec2 vWorldPos;
out float vConnected;

uniform sampler2D infoTexture;

uniform float u_Feather;
uniform float u_OuterRadius;
uniform vec2 u_WireTextureSize;
uniform mat4 u_ViewProjection;
uniform float u_HasInfoTexture;

void main()
{
    float r = u_OuterRadius + u_Feather * 2.0;
    vec2 pBaseMin = vec2(baseP.x - r, baseP.y - r);
    vec2 pBaseMax = vec2(baseP.x + r, baseP.y + r);
    vec2 p1Min = vec2(p1.x - r, p1.y - r);
    vec2 p1Max = vec2(p1.x + r, p1.y + r);
    vec2 p2Min = vec2(p2.x - r, p2.y - r);
    vec2 p2Max = vec2(p2.x + r, p2.y + r);

    vec2 min = min(min(pBaseMin, p1Min), p2Min);
    vec2 max = max(max(pBaseMax, p1Max), p2Max);

    vec2 p = vec2(mix(min.x, max.x, uv.x), mix(min.y, max.y, uv.y));

    float connected;
    float valueBase;
    float valueP1;
    float valueP2;

    if (u_WireTextureSize.x == 0)
    {
        connected = 0.f;
        valueBase = 0.f;
        valueP1   = 0.f;
        valueP2   = 0.f;
    }
    else
    {
        connected = u_HasInfoTexture * texture(infoTexture, (wireUV + vec2(0.5, 0.5)) / u_WireTextureSize).x;
        valueBase = u_HasInfoTexture * texture(infoTexture, (wireUV + vec2(1.5, 0.5)) / u_WireTextureSize).x;
        valueP1   = u_HasInfoTexture * texture(infoTexture, (wireUV + vec2(2.5, 0.5)) / u_WireTextureSize).x;
        valueP2   = u_HasInfoTexture * texture(infoTexture, (wireUV + vec2(3.5, 0.5)) / u_WireTextureSize).x;
    }

    vWorldPos = p;
    vBase.xy = baseP.xy;
    vP1.xy = p1.xy;
    vP2.xy = p2.xy;
    vConnected = connected;

    float is3Pin = length(baseP - p1);
    float isSimpleSwitch = 1.0 - is3Pin;

    vBase.z = sign(valueBase + valueP2 * connected + valueP1 * (1.0 - connected));
    vP1.z = sign(valueP1 + (isSimpleSwitch * connected * valueP2) + (valueBase * (is3Pin * (1.0 - connected))));
    vP2.z = sign(valueP2 + (isSimpleSwitch * connected * valueP1) + (valueBase * (is3Pin * connected)));

    gl_Position = u_ViewProjection * vec4(p, 0.0, 1.0);
}