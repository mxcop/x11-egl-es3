#version 310 es
layout (location = 0) in vec3 pos;

/* Model, View, & Projection matrix combined */
uniform mat4 model;
uniform mat4 pvm;
uniform vec3 camera_forward;

out float light;

void main()
{
    gl_Position = pvm * vec4(pos, 1.0);
    light = dot(camera_forward, normalize(vec3(model * vec4(pos, 1.0))));
}
