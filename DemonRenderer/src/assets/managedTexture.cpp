/** \file managedTexture.cpp */
#include <glad/glad.h>
#include "assets/managedTexture.hpp"

uint32_t ManagedTexture::getUnit(uint32_t m_ID)
{
	uint32_t unit = -1;
	bool needsBinding = ManagedTexture::m_textureUnitManager.getUnit(m_ID, unit);
	if (needsBinding) {
		glBindTextureUnit(unit, m_ID);
	}
	return unit;
}

