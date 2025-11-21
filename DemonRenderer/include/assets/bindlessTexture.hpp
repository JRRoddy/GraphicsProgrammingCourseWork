/** \file bindlessTexture.hpp */
#pragma once

/** \class Texture
*   \brief A texture which is resident on the GPU.
*   Bindless texture do not need binding but cannot be edited after creation.
*	Using bindless texture will mean RenderDoc will not be able to debug your program.
*/

#include "assets/textureBase.hpp"


class BindlessTexture : public TextureBase
{
public:
	explicit BindlessTexture(const char* filepath); //!< Constructor which takes a path to an image file to be loaded
	BindlessTexture(const TextureDescription& desc, unsigned char* data = nullptr); //!< Constructor which takes a TextureDescription
	~BindlessTexture();  //!< Destructor which removes the texture from the GPU
	inline uint64_t [[nodiscard]] getHandle() noexcept { return m_handle; } //!< Get the ID
private:
	uint64_t m_handle{ 0 }; //!< Holds the GPU handle for the cubemap
};