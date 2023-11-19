#include "volume.h"

#include <libs.h>

const float vv_vertices[] = {
    -0.5, -0.5, 0.5,  /* 0 */
    0.5,  -0.5, 0.5,  /* 1 */
    -0.5, 0.5,  0.5,  /* 2 */
    0.5,  0.5,  0.5,  /* 3 */
    -0.5, -0.5, -0.5, /* 4 */
    0.5,  -0.5, -0.5, /* 5 */
    -0.5, 0.5,  -0.5, /* 6 */
    0.5,  0.5,  -0.5  /* 7 */
};

const unsigned char vv_indices[] = {
    2, 6, 7, 2, 3, 7, /* top */
    0, 4, 5, 0, 1, 5, /* bottom */
    0, 2, 6, 0, 4, 6, /* left */
    1, 3, 7, 1, 5, 7, /* right */
    0, 2, 3, 0, 1, 3, /* front */
    4, 6, 7, 4, 5, 7, /* tear */
};

void VoxelVolumeRenderer::init() {
    /* Generate buffers */
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    /* Init vertex buffer */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vv_vertices), vv_vertices,
                 GL_STATIC_DRAW);

    /* Init element buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vv_indices), vv_indices,
                 GL_STATIC_DRAW);

    /* Init vertex array */
    glBindVertexArray(vao);

    /* Position */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int VoxelVolumeRenderer::load(const char* vert_src_path,
                              const char* frag_src_path) {
    /* Load shader */
    return static_cast<int>(shader.load(vert_src_path, frag_src_path));
}

void VoxelVolumeRenderer::draw_nobind(const VoxelVolume& vv,
                                      const glm::mat4& pv) const {
    /* Create the model matrix */
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, vv.pos);
    model = model * glm::mat4_cast(vv.rot);
    model = glm::scale(model, static_cast<glm::vec3>(vv.size));

    /* Combine matrices */
    glm::mat4 pvm = pv * model;

    /* Upload the uniforms */
    shader.set_mat4("pvm", pvm);

    /* Draw the volume */
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
}

void VoxelVolumeRenderer::draw(const VoxelVolume& vv,
                               const glm::mat4& pv) const {
    /* Starting using the shader */
    shader.use();

    /* Bind buffers */
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    draw_nobind(vv, pv);

    /* Unbind buffers */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VoxelVolumeRenderer::draw_all(const std::vector<VoxelVolume>& vvv,
                                   const glm::mat4& pv) const {
    /* Starting using the shader */
    shader.use();

    /* Bind buffers */
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    for (const VoxelVolume& vv : vvv) {
        draw_nobind(vv, pv);
    }

    /* Unbind buffers */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

VoxelVolumeRenderer::~VoxelVolumeRenderer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void VoxelVolume::set_rotation(glm::vec3 rot) { this->rot = rot; }
