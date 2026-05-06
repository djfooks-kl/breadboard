#version 300 es
precision mediump float;

uniform float u_Feather;
uniform float u_HasInfoTexture;
uniform vec2 u_WireTextureSize;
uniform vec3 u_ColorEmpty;
uniform vec3 u_ColorFull;
uniform vec3 u_ColorEdge;
uniform float u_Expand;
uniform float u_InnerWidth;
uniform float u_OuterWidth;

in vec2 vP;
in vec2 vP1;
in vec2 vP2;
in vec2 vWireUV;

out vec4 FragColor;

uniform sampler2D wireTexture;

#define SQRT2 1.4142135

void main(void) {
    vec2 n = vP1 - vP2;
    n = normalize(n);
    n = vec2(n.y, -n.x);
    vec2 wireOffset = vP - vP1;
    float d = abs(dot(wireOffset, n));

    float minX = min(vP1.x, vP2.x);
    float maxX = max(vP1.x, vP2.x);
    float minY = min(vP1.y, vP2.y);
    float maxY = max(vP1.y, vP2.y);
    float outerWire = u_OuterWidth + u_Feather * SQRT2 * u_Expand;

    float v = (d - outerWire) / u_Feather;
    float alpha = 1.0 - v;

    v = (d - u_InnerWidth) / u_Feather;

    float wireValue = u_HasInfoTexture * texture(wireTexture, vWireUV / u_WireTextureSize).x;

    vec3 wireColor = mix(u_ColorEmpty, u_ColorFull, wireValue);

    wireColor = mix(wireColor, u_ColorEdge, max(min(v, 1.0), 0.0));

    FragColor = vec4(wireColor.rgb, alpha);
}
