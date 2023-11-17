#pragma once

#include <vector>

#include "shader.h"

/**
 * @brief Data representing a cuboid bounding volume.
 */
struct CuboidVolume {
    glm::vec3 pos;
    glm::ivec3 size;
    glm::quat rot;

    CuboidVolume() = default;
    CuboidVolume(glm::vec3 pos, glm::ivec3 size, glm::quat rot)
        : pos(pos), size(size), rot(rot){};
    CuboidVolume(glm::vec3 pos, glm::ivec3 size, glm::vec3 rot)
        : pos(pos), size(size), rot(rot){};
};

/**
 * @brief Used for rendering a large number of cuboid volumes.
 */
class CuboidVolumeRenderer {
    /* Buffer handles */
    GLuint vbo = 0, vao = 0, ebo = 0;
    Shader shader;

public:
    CuboidVolumeRenderer() = default;
    ~CuboidVolumeRenderer();

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
     * @brief Draw a cuboid volume.
     * 
     * @param cv The volume data.
     * @param pv The view and projection matrices.
     */
    void draw(const CuboidVolume& cv, const glm::mat4& pv) const;
};
