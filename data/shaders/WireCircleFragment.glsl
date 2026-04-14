#version 300 es
precision mediump float;

uniform float feather;
uniform float size;
uniform vec3 colorEmpty;
uniform vec3 colorFull;
uniform float expand;

in vec2 vP;
in vec2 vCircleCenter;
in float vWireValue;

out vec4 FragColor;

#define SQRT2 1.4142135

void main(void) {
    //FragColor = vec4(0.9, 0.9, 0.9, 1.0);

    vec2 offset1 = vP - vCircleCenter;
    float d = length(offset1);

    float r = size + feather * SQRT2 * expand;

    float alpha = 1.0 - ((d - r) / feather);

    vec3 color = mix(colorEmpty, colorFull, vWireValue);

    FragColor = vec4(color.rgb, alpha);
}
