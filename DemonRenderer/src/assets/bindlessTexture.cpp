/** \file bindlessTexture.cpp */
#include "assets/bindlessTexture.hpp"
#include "core/log.hpp"

BindlessTexture::BindlessTexture(const char* filepath) : TextureBase(filepath)
{
    m_handle = glGetTextureHandleARB(m_ID);
    if (m_handle == 0) spdlog::error("Could not generate GPU resident handle for texture: {}", filepath);
    glMakeTextureHandleResidentARB(m_handle);
}

BindlessTexture::BindlessTexture(const TextureDescription& desc, unsigned char* data) : TextureBase(desc, data)
{
    m_handle = glGetTextureHandleARB(m_ID);
    if (m_handle == 0) spdlog::error("Could not generate GPU resident handle for texture");
    glMakeTextureHandleResidentARB(m_handle);
}

BindlessTexture::~BindlessTexture()
{
    glMakeTextureHandleNonResidentARB(m_handle);
}
