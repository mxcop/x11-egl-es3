#pragma once

#include <vector>

#include "shader.h"

/**
 * @brief Data representing a voxel bounding volume.
 */
struct VoxelVolume {
    glm::vec3 pos;
    glm::ivec3 size;
    glm::quat rot;

    VoxelVolume() = default;
    VoxelVolume(glm::vec3 pos, glm::ivec3 size, glm::quat rot)
        : pos(pos), size(size), rot(rot){};
    VoxelVolume(glm::vec3 pos, glm::ivec3 size, glm::vec3 rot)
        : pos(pos), size(size), rot(rot){};

    void set_rotation(glm::vec3 rot);
};

/**
 * @brief Used for rendering a large number of voxel volumes.
 */
class VoxelVolumeRenderer {
    /* Buffer handles */
    GLuint vbo = 0, vao = 0, ebo = 0;
    Shader shader;

public:
    VoxelVolumeRenderer() = default;
    ~VoxelVolumeRenderer();

    void init();

    /**
     * @brief Load a vertex and fragment shader from filesystem.
     *
     * @param vert_src_path Path to the vertex shader source file.
     * @param frag_src_path Path to the fragment shader source file.
     * @return Zero if loading failed, otherwise it's the shader program handle.
     */
    int load(const char *vert_src_path, const char *frag_src_path);

    /**
     * @brief Draw a voxel volume without binding and unbinding resources.
     * 
     * @param vv The voxel volume data.
     * @param pv The view and projection matrices.
     */
    void draw_nobind(const VoxelVolume& vv, const glm::mat4& pv) const;

    /**
     * @brief Draw a voxel volume.
     * 
     * @param vv The voxel volume data.
     * @param pv The view and projection matrices.
     */
    void draw(const VoxelVolume& vv, const glm::mat4& pv) const;

    /**
     * @brief Draw all voxel volumes in a vector.
     * 
     * @param vvv The voxel volume data vector.
     * @param pv The view and projection matrices.
     */
    void draw_all(const std::vector<VoxelVolume>& vvv, const glm::mat4& pv) const;
};
