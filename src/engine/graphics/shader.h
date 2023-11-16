#pragma once

/**
 * @brief Shader interface.
 */
struct Shader {
    /* Shader object handle */
    GLuint handle = 0;

    Shader() = default;
    /**
     * @param vert_src Vertex shader source code.
     * @param frag_src Fragment shader source code.
     */
    Shader(const char* vert_src, const char* frag_src);
    ~Shader();

    /* Cannot be copied (because there's no reference counting for the shader
     * object) */
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /* Can be moved */
    Shader(Shader&&) = default;
    Shader& operator=(Shader&&) = default;

    /**
     * @brief Load a vertex and fragment shader from filesystem.
     *
     * @param vert_src_path Path to the vertex shader source file.
     * @param frag_src_path Path to the fragment shader source file.
     * @return Zero if loading failed, otherwise it's the shader program handle.
     */
    GLuint load(const char* vert_src_path, const char* frag_src_path);

    /**
     * @brief Start using this shader program.
     */
    void use() const;

    void set_float(const char* name, float value) const;
    void set_int(const char* name, int value) const;
    void set_vec2f(const char* name, const glm::vec2& vec) const;
    void set_vec3f(const char* name, const glm::vec3& vec) const;
    void set_mat4(const char* name, const glm::mat4& matrix) const;
};
