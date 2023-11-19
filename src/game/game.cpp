#include "game.h"

#include "graphics/volume.h"

// TEMP: testing
std::vector<VoxelVolume> volumes;
VoxelVolumeRenderer renderer;

/* Projection matrix */
const glm::mat4 proj =
    glm::perspective(glm::radians(45.0f), 1.5f, 0.1f, 100.0f);

int init() {
    volumes.reserve(4 * 4 * 4);
    for (int z = 0; z < 4; z++)
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++) {
                volumes.emplace_back(VoxelVolume(
                    glm::vec3(x * 1.5f, y * 1.5f, z * 1.5f),
                    glm::ivec3(1), glm::vec3(1.0f)));
            }

    renderer.init();
    if (!renderer.load("assets/shaders/simple.vert",
                       "assets/shaders/simple.frag")) {
        return 0;
    }
    return 1;
}

void tick(double dt) {
    static double time = 0.0;
    time += dt;

    // volume.set_rotation(glm::vec3(time));

    /* Create the camera view matrix */
    glm::mat4 view =
        glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(3.0f, 3.0f, 3.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pv = proj * view;

    for (VoxelVolume& cv : volumes) {
        cv.set_rotation(glm::vec3(time));
    }

    renderer.draw_all(volumes, pv);
}
