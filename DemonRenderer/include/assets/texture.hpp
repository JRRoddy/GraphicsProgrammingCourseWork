/** \file Texture.hpp */
#pragma once

/** \class Texture
*   \brief A texture whose device binding point is managed by the texture unit manager.
*   Only 32 Textures can be bound at any one time.
*	Textures can be edited after creation.
*/

#include "assets/textureBase.hpp"
#include "assets/textureUnitManager.hpp"
#include "assets/managedTexture.hpp"

class Texture : public TextureBase, public ManagedTexture
{
public:
	explicit Texture(const char* filepath) : TextureBase(filepath) {}; //!< Constructor which takes a path to an image file to be loaded
	Texture(const TextureDescription& desc, unsigned char* data = nullptr) : TextureBase(desc, data) {}; //!< Constructor which takes a TextureDescription
private:
};