#include "shader.h"

#include <fstream>
#include <string>

#include <libs.h>
#include <glm/gtc/type_ptr.hpp> /* "glm::value_ptr" */

/* DEBUG safety check */
#if DEBUG
/* Returns if shader is uninitialized. */
#define INIT_GUARD                                                            \
    if (handle == 0) {                                                        \
        printf("error: attempt to call function on uninitialized shader.\n"); \
        return;                                                               \
    }
#else
#define INIT_GUARD
#endif

/**
 * @brief Print any shader compilation errors to the console.
 */
bool log_err(unsigned int shader_id, std::string type) {
    int success;
    char info_log[1024];
    if (type != "shader program") {
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader_id, 1024, NULL, info_log);
            printf("compile-time error: in %s\n%s", type.c_str(), info_log);
        }
    } else {
        glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_id, 1024, NULL, info_log);
            printf("link-time error: in %s\n%s", type.c_str(), info_log);
        }
    }
    return success;
}

/**
 * @brief Compile shaders, combine them into a program, and store it on the
 * GPU.
 *
 * @param vert_src Vertex shader source code.
 * @param frag_src Fragment shader source code.
 * @return The handle of the shader program, if an error occured the handle will
 * be zero.
 */
GLuint compile(const char* vert_src, const char* frag_src) {
    /* Don't compile if either the vertex or fragment shader is null */
    if (vert_src == nullptr || frag_src == nullptr) return 0;

    GLuint vert_shader, frag_shader;

    /* Compile vertex shader */
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_src, NULL);
    glCompileShader(vert_shader);
    if (!log_err(vert_shader, "vertex shader")) {
        /* Cleanup if compiling failed */
        glDeleteShader(vert_shader);
        return 0;
    }

    /* Compile fragment shader */
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_src, NULL);
    glCompileShader(frag_shader);
    if (!log_err(frag_shader, "fragment shader")) {
        /* Cleanup if compiling failed */
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        return 0;
    }

    /* Create the shader program */
    GLuint handle = glCreateProgram();

    /* Attach the compiled shaders */
    glAttachShader(handle, vert_shader);
    glAttachShader(handle, frag_shader);

    /* Link the shader program */
    glLinkProgram(handle);
    if (!log_err(handle, "shader program")) {
        /* Cleanup if linking failed */
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        glDeleteProgram(handle);
        return 0;
    }

    /*
        Finally, delete the shaders once linked.
        Now they're stored on the GPU.
    */
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return handle;
}

Shader::Shader(const char* vert_src, const char* frag_src)
    : handle(compile(vert_src, frag_src)) {}

GLuint Shader::load(const char* vert_src_path, const char* frag_src_path) {
    std::ifstream vert_file, frag_file;
    vert_file.open(vert_src_path);
    frag_file.open(frag_src_path);

    /* Safety checks to see if files opened */
    if (!vert_file.is_open()) {
        printf("error: couldn't open vertex shader file: %s\n", vert_src_path);
        return 0;
    }
    if (!frag_file.is_open()) {
        printf("error: couldn't open fragment shader file: %s\n",
               frag_src_path);
        return 0;
    }

    /* Dump file contents into strings */
    std::string vert_src, frag_src;
    vert_file.seekg(0, std::ios::end);
    vert_src.reserve(vert_file.tellg());
    vert_file.seekg(0, std::ios::beg);
    vert_src.assign((std::istreambuf_iterator<char>(vert_file)),
            std::istreambuf_iterator<char>());
    frag_file.seekg(0, std::ios::end);
    frag_src.reserve(frag_file.tellg());
    frag_file.seekg(0, std::ios::beg);
    frag_src.assign((std::istreambuf_iterator<char>(frag_file)),
            std::istreambuf_iterator<char>());

    handle = compile(vert_src.c_str(), frag_src.c_str());
    return handle;
}

Shader::~Shader() {
    INIT_GUARD
    glDeleteProgram(handle);
}

void Shader::use() const {
    INIT_GUARD
    glUseProgram(handle);
}

void Shader::set_float(const char* name, float value) const {
    INIT_GUARD
    glUniform1f(glGetUniformLocation(handle, name), value);
}

void Shader::set_int(const char* name, int value) const {
    INIT_GUARD
    glUniform1i(glGetUniformLocation(handle, name), value);
}

void Shader::set_vec2f(const char* name, const glm::vec2& vec) const {
    INIT_GUARD
    glUniform2f(glGetUniformLocation(handle, name), vec.x, vec.y);
}

void Shader::set_vec3f(const char* name, const glm::vec3& vec) const {
    INIT_GUARD
    glUniform3f(glGetUniformLocation(handle, name), vec.x, vec.y, vec.z);
}

void Shader::set_mat4(const char* name, const glm::mat4& matrix) const {
    INIT_GUARD
    glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, false,
                       glm::value_ptr(matrix));
}
