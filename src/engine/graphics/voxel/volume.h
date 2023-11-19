#pragma once

/**
 * @brief Data representing a voxel bounding volume.
 */
struct VoxelVolume {
    /* World transform */
    glm::vec3 pos = {};
    glm::ivec3 size = {};
    glm::quat rot = {};

    /* Voxel data */
    struct Data {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        int width = 0, height = 0, depth = 0;
        unsigned char* voxels = nullptr;
        GLuint handle = 0;
    } data;

    VoxelVolume() = default;
    // TODO: Cleanup resources in destructor!!!
    VoxelVolume(glm::vec3 pos, glm::ivec3 size, glm::quat rot)
        : pos(pos), size(size), rot(rot){};
    VoxelVolume(glm::vec3 pos, glm::ivec3 size, glm::vec3 rot)
        : pos(pos), size(size), rot(rot){};

    void gen_data(int w, int h, int d);
    void gen_mipmap();
};
