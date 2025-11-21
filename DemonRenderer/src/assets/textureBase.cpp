#include <glad/glad.h>
#include "assets/texture.hpp"
#include "stbImage/stb_image.h"
#include "core/log.hpp"


TextureBase::TextureBase(const char* filepath)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);

	m_textureParams.width = static_cast<uint32_t>(width);
	m_textureParams.height = static_cast<uint32_t>(height);
	m_textureParams.channels = static_cast<uint32_t>(channels);
	m_textureParams.isMipMapped = true;

	if (data) init(data);

	else spdlog::error("Failed to load texture from filepath: {}", filepath);
	stbi_image_free(data);
}

TextureBase::TextureBase(const TextureDescription& desc, unsigned char* data) : m_textureParams(desc)
{
	init(data);
}

TextureBase::~TextureBase()
{
	glDeleteTextures(1, &m_ID);
}

void TextureBase::edit(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, unsigned char* data)
{
	if (data) {
		if (m_textureParams.channels == 3) glTextureSubImage2D(m_ID, 0, xOffset, yOffset, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (m_textureParams.channels == 4) glTextureSubImage2D(m_ID, 0, xOffset, yOffset, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
}



void TextureBase::init(unsigned char* data)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, m_textureParams.wrapMethod);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, m_textureParams.wrapMethod);

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, m_textureParams.minFilterMethod);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, m_textureParams.magFilterMethod);

	int32_t mipCount = 1;
	if(m_textureParams.isMipMapped) mipCount = 1 + floor(log2(std::max(m_textureParams.width, m_textureParams.height)));

	GLenum internalFormat = GL_FALSE;
	GLenum format = GL_FALSE;
	GLenum dataType = GL_FALSE;

	if (m_textureParams.type == TextureDataType::depth) {
		glTextureParameteri(m_ID, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		internalFormat = GL_DEPTH_COMPONENT32F;
		format = GL_DEPTH_COMPONENT;
		dataType = GL_FLOAT;
	}
	else if (m_textureParams.type == TextureDataType::SDR) {
		switch(m_textureParams.channels) {
		case 1 : 
			internalFormat = GL_R8;
			format = GL_RED;
			break;
		case 2:
			internalFormat = GL_RG8;
			format = GL_RG;
			break;
		case 3:
			internalFormat = GL_RGB8;
			format = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			break;
		default :
			spdlog::error("unhandled texture parameters: type {} channels {}", (int)m_textureParams.type, m_textureParams.channels);
			return;
		}
		dataType = GL_UNSIGNED_BYTE;
	}
	else if (m_textureParams.type == TextureDataType::HDR) {
		switch (m_textureParams.channels) {
		case 1:
			internalFormat = GL_R16F;
			format = GL_RED;
			break;
		case 2:
			internalFormat = GL_RG16F;
			format = GL_RG;
			break;
		case 3:
			internalFormat = GL_RGB16F;
			format = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			break;
		default:
			spdlog::error("unhandled texture parameters: type {0:x} channels {1}", (int)m_textureParams.type, m_textureParams.channels);
			return;
		}
		dataType = GL_FLOAT;
	}
	else {
		spdlog::error("unhandled texture parameters: type {0:x} channels {1}", (int)m_textureParams.type, m_textureParams.channels);
		return;
	}

	glTextureStorage2D(m_ID, mipCount, internalFormat, m_textureParams.width, m_textureParams.height);
	if(data) glTextureSubImage2D(m_ID, 0, 0, 0, m_textureParams.width, m_textureParams.height, format, dataType, data);
	
	if(m_textureParams.isMipMapped) glGenerateTextureMipmap(m_ID);
}

