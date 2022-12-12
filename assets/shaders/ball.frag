#version 330 core

in vec4 v_colour;

out vec4 f_colour;

uniform float aspect;
uniform vec2 u_resolution;
uniform vec2 u_center;
uniform float u_radius;

void main()
{
    vec2 st = gl_FragCoord.xy / u_resolution.xy;
    st.x *= u_resolution.x / u_resolution.y;
    st = st * 2. - vec2(aspect, 1.);
    float dist = length(st - u_center);
    // f_colour = vec4(v_colour.rgb, smoothstep(.0, 1., float(dist < .05) - dist));
    f_colour = vec4(v_colour.rgb, float(dist < u_radius));
}
