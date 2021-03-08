#include "texture.h"

namespace urus
{
    Texture::Texture()
    : mWidth(0)
    , mHeight(0)
    , mChannels(0)
    {
        glGenTextures(1, &mHandle);
    }

    Texture::Texture(const char* path)
    {
        glGenTextures(1, &mHandle);
        load(path);
    }

    Texture::Texture(const std::string& path)
    : Texture(path.c_str())
    {

    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &mHandle);
    }

    void Texture::load(const char* path)
    {
        glBindTexture(GL_TEXTURE_2D, mHandle);
        // FIXME: correct the image loading part
        int width, height, channels;
        // FIXME: idem
		// unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
        unsigned char* data = nullptr;
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// FIXME: free the image data
		// stbi_image_free(data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		mWidth = width;
		mHeight = height;
		mChannels = channels;
    }

    void Texture::load(const std::string& path)
    {
        load(path.c_str());
    }

    void Texture::set(GLint uniformIndex, GLint textureIndex)
    {
        GLenum lol;
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(GL_TEXTURE_2D, mHandle);
        glUniform1i(uniformIndex, textureIndex);
    }

    void Texture::unset(GLint textureIndex)
    {
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
    }
} // namespace urus
