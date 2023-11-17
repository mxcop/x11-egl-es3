#version 310 es
layout (location = 0) in vec3 pos;

/* Model, View, & Projection matrix combined */
uniform mat4 pvm;

void main()
{
    gl_Position = pvm * vec4(pos, 1.0);
}
