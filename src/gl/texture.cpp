#include "gl/texture.h"
#include "disable_all_warnings.h"
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
#include <stb_image.h>
DISABLE_WARNINGS_POP()

Texture::Texture(std::filesystem::path filePath)
{
    if (!std::filesystem::exists(filePath))
        throw ImageLoadingException(fmt::format("File {} does not exist", filePath.string().c_str()));

    // Load image from disk to CPU memory.
    int width, height, channels;
    stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &channels, 3);
    if (!pixels)
        throw ImageLoadingException(fmt::format("Failed to load image file {}", filePath.string().c_str()));

    assert(width > 0 && height > 0);

    // Create a texture on the GPU with 3 channels with 8 bits each.
    m_texture = std::shared_ptr<GLuint>(new GLuint(),[](GLuint *p){
        glDeleteTextures(1, p);
        delete p;
    });
    glCreateTextures(GL_TEXTURE_2D, 1, m_texture.get());
    glTextureStorage2D(*m_texture, 1, GL_RGB8, width, height);

    // Upload pixels into the GPU texture.
    glTextureSubImage2D(*m_texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(*m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(*m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(*m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(*m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(GLsizei width, GLsizei height, GLenum internalFormat)
{
    // Create a texture on the GPU with 3 channels with 8 bits each.
    m_texture = std::shared_ptr<GLuint>(new GLuint(),[](GLuint *p){
        glDeleteTextures(1, p);
        delete p;
    });
    glCreateTextures(GL_TEXTURE_2D, 1, m_texture.get());
    glTextureStorage2D(*m_texture, 1, internalFormat, width, height);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(*m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(*m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(*m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(*m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::set(GLenum parameter, GLint setting)
{
    glTextureParameteri(*m_texture, parameter, setting);
}

void Texture::bind(GLint textureSlot) const
{
    glActiveTexture(GL_TEXTURE0 + GLenum(textureSlot));
    glBindTexture(GL_TEXTURE_2D, *m_texture);
}
