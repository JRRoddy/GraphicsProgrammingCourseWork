/** \file image.hpp */
#pragma once

#include <memory>
#include <variant>
#include "assets/texture.hpp"
#include "assets/cubeMap.hpp"


/** \enum TextureAccess
*   Ways texture memory may be accessed when bound to an image
*/
enum class TextureAccess {
	WriteOnly = GL_WRITE_ONLY,
	ReadOnly = GL_READ_ONLY,
	ReadWrite = GL_READ_WRITE
};

/**	\struct ImageDescWithCubeMap
*	\brief Details needed to create an image with a cubemap
*/
struct ImageDescWithCubeMap
{
	std::shared_ptr<CubeMap> cubeMap{ nullptr };
	uint32_t imageUnit{ 0 };
	uint32_t mipLevel{ 0 };
	int32_t layer{ 0 };
	TextureAccess access{ TextureAccess::ReadWrite };
};

/**	\struct ImageDescWithTexture
*	\brief Details needed to create an image with a texture
*/
struct ImageDescWithTexture
{
	std::shared_ptr<Texture> texture{ nullptr };
	uint32_t imageUnit{ 0 };
	uint32_t mipLevel{ 0 };
	bool layered{ false };
	int32_t layer{ 0 };
	TextureAccess access{ TextureAccess::ReadWrite };
};

/**	\struct Image
*	\brief A texture or cubemap which is bound to an image for compute shader access
*/
class Image
{
public:
	Image(ImageDescWithCubeMap desc);
	Image(ImageDescWithTexture desc);
	std::shared_ptr<CubeMap> getCubeMap() const { return std::get<std::shared_ptr<CubeMap>>(m_texOrCubemap); }
	std::shared_ptr<Texture> getTexture() const { return std::get<std::shared_ptr<Texture>>(m_texOrCubemap); }
	inline uint32_t [[nodiscard]] getImageUnit() const { return m_imageUnit; }
	inline uint32_t [[nodiscard]] getMipLevel() const { return m_mipLevel; }
	inline bool [[nodiscard]] getLayered() const { return m_layered; }
	inline TextureAccess [[nodiscard]] getAccess() const { return m_access; }
	uint32_t [[nodiscard]] getChannels() const noexcept { return m_channels; }
	bool [[nodiscard]] isHDR() const noexcept {return m_ID;}
	inline uint32_t [[nodiscard]] getID() const noexcept { return m_ID; } //!< Get the ID
private:
	std::variant<std::shared_ptr<Texture>, std::shared_ptr<CubeMap>> m_texOrCubemap;
	uint32_t m_imageUnit{ 0 };
	uint32_t m_mipLevel{ 0 };
	bool m_layered{ false };
	int32_t m_layer{ 0 };
	TextureAccess m_access{ TextureAccess::ReadWrite };
	bool m_HDR{ false }; //!< IS underlying texture or cubemap HDR?
	uint32_t m_channels{ 0 }; //!< Number of channels of underlying texture or cubemap
	uint32_t m_ID{ 0 }; //!< OpenGL ID of underlying texture or cubemap
};
