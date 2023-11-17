#include "volume.h"

#include <libs.h>

const float cv_vertices[] = {
    -0.5f, -0.5f, -0.5f,  // A 0
    0.5f,  -0.5f, -0.5f,  // B 1
    0.5f,  0.5f,  -0.5f,  // C 2
    -0.5f, 0.5f,  -0.5f,  // D 3
    -0.5f, -0.5f, 0.5f,   // E 4
    0.5f,  -0.5f, 0.5f,   // F 5
    0.5f,  0.5f,  0.5f,   // G 6
    -0.5f, 0.5f,  0.5f,   // H 7
};

const unsigned char cv_indices[] = {
    0, 3, 2,  //
    2, 1, 0,  //

    4, 5, 6,  //
    6, 7, 4,  //

    7, 3, 0,  //
    0, 4, 7,  //

    1, 2, 6,  //
    6, 5, 1,  //

    0, 1, 5,  //
    5, 4, 0,  //

    2, 3, 7,  //
    7, 6, 2,  //
};

constexpr float g_vertex_buffer_data[] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // triangle 1.
    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // triangle 2.
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // triangle 3.
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // triangle 4.
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // triangle 5.
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // triangle 6.
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // triangle 7.
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // triangle 8.
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // triangle 9.
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // triangle 10.
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // triangle 11.
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f   // triangle 12.
};

void CuboidVolumeRenderer::init() {
    /* Generate buffers */
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    /* Init vertex buffer */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cv_vertices), cv_vertices,
                 GL_STATIC_DRAW);

    /* Init element buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cv_indices), cv_indices,
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

int CuboidVolumeRenderer::load(const char* vert_src_path,
                               const char* frag_src_path) {
    /* Load shader */
    return static_cast<int>(shader.load(vert_src_path, frag_src_path));
}

void CuboidVolumeRenderer::draw(const CuboidVolume& cv,
                                const glm::mat4& pv) const {
    /* Create the model matrix */
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::scale(model, static_cast<glm::vec3>(cv.size));
    // glm::mat4 rotation = glm::mat4_cast(cv.rot);
    // model = glm::translate(model, cv.pos);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cv.pos);
    model = model * glm::mat4_cast(cv.rot);
    model = glm::scale(model, static_cast<glm::vec3>(cv.size));

    /* Combine matrices */
    glm::mat4 pvm = pv * model;

    /* Upload the matrix */
    shader.use();
    shader.set_mat4("pvm", pvm);

    /* Draw the volume */
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

CuboidVolumeRenderer::~CuboidVolumeRenderer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}
