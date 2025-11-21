/** \file image.cpp */
#include "assets/image.hpp"

Image::Image(ImageDescWithCubeMap desc) :
	m_access(desc.access),
	m_imageUnit(desc.imageUnit),
	m_layer(desc.layer),
	m_layered(true),
	m_mipLevel(desc.mipLevel),
	m_texOrCubemap(desc.cubeMap),
	m_channels(desc.cubeMap->getChannels()),
	m_ID(desc.cubeMap->getID()),
	m_HDR(desc.cubeMap->isHDR())
{
}

Image::Image(ImageDescWithTexture desc) :
	m_access(desc.access),
	m_imageUnit(desc.imageUnit),
	m_layer(desc.layer),
	m_layered(desc.layered),
	m_mipLevel(desc.mipLevel),
	m_texOrCubemap(desc.texture),
	m_channels(desc.texture->getChannels()),
	m_ID(desc.texture->getID()),
	m_HDR(desc.texture->isHDR())
{
}
