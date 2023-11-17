#version 310 es
precision mediump float;

out vec4 frag_color;
in float light;

void main()
{
    frag_color = vec4(vec3(0.5, 0.0, 0.5) * (0.5 - light), 1.0);
}
