#version 300 es
precision mediump float;

uniform float u_Feather;
uniform float u_Size;
uniform vec3 u_ColorEmpty;
uniform vec3 u_ColorFull;
uniform float u_Expand;

in vec2 vP;
in vec2 vCircleCenter;
in float vWireValue;

out vec4 FragColor;

#define SQRT2 1.4142135

void main(void) {
    vec2 offset1 = vP - vCircleCenter;
    float d = length(offset1);

    float r = u_Size + u_Feather * SQRT2 * u_Expand;

    float alpha = 1.0 - ((d - r) / u_Feather);

    vec3 color = mix(u_ColorEmpty, u_ColorFull, vWireValue);

    FragColor = vec4(color.rgb, alpha);
}
