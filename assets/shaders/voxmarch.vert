#version 310 es

layout (location = 0) in vec3 pos;

uniform vec3 camera_pos;
uniform mat4 inverse_model;

/* Model, View, & Projection matrix combined */
uniform mat4 pvm;

out vec3 frag_origin;
out vec3 frag_dir;

void main()
{
    gl_Position = pvm * vec4(pos, 1.0);

    /* Convert camera position from world-space to local-space of the model */
	vec3 camera_local = (inverse_model * vec4(camera_pos, 1.0)).xyz;

    /* Compute ray origin and direction */
	frag_origin = camera_local;
	frag_dir = (pos - camera_local);
}
