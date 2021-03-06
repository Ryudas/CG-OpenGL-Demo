#pragma once
#include "disable_all_warnings.h"
DISABLE_WARNINGS_PUSH()
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <memory>
#include <exception>
#include <filesystem>
#include <vector>

#include "gl/shader_stage.h"
#include "util3D/geometry.h"

class BasicGeometry :public Geometry{
private:
    GLsizei m_numIndices { 0 };
    bool m_hasTextureCoords { false };
    std::shared_ptr<GLuint> m_ibo = nullptr;
    std::shared_ptr<GLuint> m_vbo = nullptr;
    std::shared_ptr<GLuint> m_vao = nullptr;
    VertexShader vertex_shader, xray_vertex_shader;
public:
    BasicGeometry() = default;
    BasicGeometry(const BasicGeometry&) = default;
    BasicGeometry(std::filesystem::path filePath);
    bool hasTextureCoords() const;
    const VertexShader& getVertexShader() const override;
    void setVertexShader(std::filesystem::path filePath);
    // Bind VAO and call glDrawElements.
    void draw() const;
    virtual const void* getUniformData() const;
    virtual GLsizeiptr getUniformDataSize() const;
    virtual ~BasicGeometry() = default;
};
