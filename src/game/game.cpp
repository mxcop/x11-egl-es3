#include "game.h"

#include "graphics/volume.h"

// TEMP: testing
CuboidVolume volume;
CuboidVolumeRenderer renderer;

/* Projection matrix */
const glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.5f, 0.1f, 100.0f);

int init() {
    volume = CuboidVolume(glm::vec3(0.0f), glm::ivec3(1), glm::vec3(1.0f));
    renderer.init();
    if (!renderer.load("assets/shaders/simple.vert", "assets/shaders/simple.frag")) {
        return 0;
    }
    return 1;
}

void tick(double dt) {
    static double time = 0.0;
    time += dt;

    volume.set_rotation(glm::vec3(time));

    /* Create the camera view matrix */
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pv = proj * view;

    renderer.draw(volume, pv);
}
