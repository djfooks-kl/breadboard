#version 300 es
precision mediump float;

in vec2 worldPos;
out vec4 FragColor;

uniform float feather;
uniform vec2 size;

const float gridLineWidth = 0.01;

vec2 distanceToInt(vec2 v)
{
    vec2 frac = fract(v);
    vec2 s = step(vec2(0.5, 0.5), frac);
    return abs(s - frac);
}

float grid(vec2 s)
{
    float d = min(s.x, s.y);
    float v = (d - gridLineWidth) / feather;
    return clamp(v, 0.0, 1.0) + 0.4;
}

float lerpWeight(float s)
{
    float w = s * feather;
    return clamp(w, 0.0, 1.0);
}

void main()
{
    const float every = 10.0;
    const float everySq = every * every;
    vec2 q = distanceToInt(worldPos);
    vec2 r = distanceToInt(worldPos / every) * every;
    vec2 s = distanceToInt(worldPos / (everySq)) * (everySq);

    float v = mix(grid(q), grid(r), lerpWeight(10.0 / every));
    v = mix(v, grid(s), lerpWeight(20.0 / (everySq)));
    //float v = grid(s);

    FragColor = vec4(v, v, v, 1.0);

    vec2 corner = size - worldPos;
    float edge = min(min(worldPos.x, worldPos.y), min(corner.x, corner.y));
    if (edge < -1.5 - feather * 2.0)
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else if (edge < -0.5)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
