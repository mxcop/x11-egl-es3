#include "game.h"

#include <algorithm>

#include "graphics/voxel/volume.h"
#include "graphics/voxel/renderer.h"

// TEMP: testing
std::vector<VoxelVolume> volumes;
VoxelRenderer renderer;

/* Projection matrix */
const glm::mat4 proj =
    glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

glm::vec3 camera_pos = glm::vec3(10.0f, 10.0f, 10.0f);

int init() {
    /* Create voxel volumes */
    volumes.reserve(4 * 4 * 4);
    for (int z = 0; z < 4; z++)
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++) {
                auto& vv = volumes.emplace_back(VoxelVolume(
                    glm::vec3(x * 1.5f, y * 1.5f, z * 1.5f),
                    glm::ivec3(1), glm::vec3(1.0f)));
                vv.gen_data(8, 8, 8);
                vv.gen_mipmap();
            }

    /* Initialize the voxel volume renderer */
    renderer.init();
    if (!renderer.load("assets/shaders/voxmarch.vert",
                       "assets/shaders/voxmarch.frag")) {
        return 0;
    }
    return 1;
}

void tick(double dt) {
    static double time = 0.0;
    static double sort_timer = 0.0;
    time += dt;
    sort_timer += dt;

    camera_pos.x = sin(time) * 10.0f + 5.0f;
    camera_pos.z = cos(time) * 10.0f + 5.0f;
    
    /* Sort the voxel volumes front to back (for avoiding overdraw) */
    if (sort_timer > 1.0 / 20.0) {
        std::sort(volumes.begin(), volumes.end(), [](VoxelVolume& a, const VoxelVolume& b){
            float da = (camera_pos.x - a.pos.x) * (camera_pos.x - a.pos.x) + (camera_pos.y - a.pos.y) * (camera_pos.y - a.pos.y) + (camera_pos.z - a.pos.z) * (camera_pos.z - a.pos.z);
            float db = (camera_pos.x - b.pos.x) * (camera_pos.x - b.pos.x) + (camera_pos.y - b.pos.y) * (camera_pos.y - b.pos.y) + (camera_pos.z - b.pos.z) * (camera_pos.z - b.pos.z);
            return (da < db);
        });
        sort_timer = 0.0;
    }

    /* Create the camera view matrix */
    glm::mat4 view =
        glm::lookAt(camera_pos, glm::vec3(2.5f, 2.5f, 2.5f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pv = proj * view;

    /* Update the volumes */
    int i = 0;
    for (VoxelVolume& cv : volumes) {
        cv.rot = glm::vec3(time);
        // cv.rot = glm::vec3(0.0f);
        cv.data.color = glm::vec3(1.0f - static_cast<float>(i) / volumes.size());
        i++;
    }

    renderer.draw_all(camera_pos, volumes, pv);
}
