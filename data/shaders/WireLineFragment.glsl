#version 300 es
precision mediump float;

uniform float feather;
uniform vec2 wireTextureSize;
uniform vec3 colorEmpty;
uniform vec3 colorFull;
uniform vec3 colorEdge;
uniform float expand;
uniform float innerWidth;
uniform float outerWidth;

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
    float outerWire = outerWidth + feather * SQRT2 * expand;

    float v = (d - outerWire) / feather;
    float alpha = 1.0 - v;

    v = (d - innerWidth) / feather;

    float wireValue = texture(wireTexture, vWireUV / wireTextureSize).x;

    vec3 wireColor = mix(colorEmpty, colorFull, wireValue);

    wireColor = mix(wireColor, colorEdge, max(min(v, 1.0), 0.0));

    FragColor = vec4(wireColor.rgb, alpha);
}
