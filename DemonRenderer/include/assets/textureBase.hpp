/** \file texture.hpp */
#pragma once
#include <glad/glad.h>
#include <inttypes.h>

enum class TextureDataType {SDR, HDR, depth};

/** \struct TextureDescription
*   \brief Description of a text, it's size, number of channels, whether of not it is HDR and pixel data.
*/
struct TextureDescription
{
	uint32_t width{ 0 }; //!< Width in pixels
	uint32_t height{ 0 }; //!< Height in pixels
	uint32_t channels{ 0 }; //!< Number of channels
	TextureDataType type{ TextureDataType::SDR };  //!< Is the texture high dynamic range
	uint32_t wrapMethod{ GL_REPEAT }; //!< How to handle UV coordinsates outside [0,1] range
	uint32_t minFilterMethod{ GL_LINEAR }; //!< Filter to use when minification happens
	uint32_t magFilterMethod{ GL_LINEAR }; //!< Filter to use when magnification happens
	bool isMipMapped{ false }; //!< Does the texture have mip levels
};

/** \class TextureBase
*   \brief Base class texture held on the GPU.
*	This class needs to be extended for specific use cases, providinmg common functionality to bindable and bindless textures.
*	It is not a standalone class, hence the protected constructor.
*/
class TextureBase
{
public:

	TextureBase() = delete; //!< Deleted default constructor
	TextureBase(TextureBase& other) = delete; //!< Deleted copy constructor
	TextureBase(TextureBase&& other) = delete; //!< Deleted move constructor
	TextureBase& operator=(TextureBase& other) = delete; //!< Deleted copy assignment operator
	TextureBase& operator=(TextureBase&& other) = delete; //!< Delete move assignment operator
	~TextureBase();  //!< Destructor which removes the texture from the GPU

	void edit(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, unsigned char* data); //!< Edit the textures data on the GPU
	inline uint32_t [[nodiscard]] getWidth() const noexcept { return m_textureParams.width; } //!< Get the width in pixels
	inline uint32_t [[nodiscard]] getHeight() const noexcept { return m_textureParams.height; } //!< Get the height in pixels
	inline uint32_t [[nodiscard]] getWidthf() const noexcept { return static_cast<float>(m_textureParams.width); } //!< Get the width in pixels as a floating point number
	inline uint32_t [[nodiscard]] getHeightf() const noexcept { return static_cast<float>(m_textureParams.height); } //!< Get the height in pixels as a floating point number
	inline uint32_t [[nodiscard]] getChannels() const noexcept { return m_textureParams.channels; } //!< Get the number of channels in the texture
	inline uint32_t [[nodiscard]] getID() const noexcept { return m_ID; } //!< Get the ID
	inline bool [[nodiscard]] isHDR() const noexcept { return m_textureParams.type == TextureDataType::HDR; } //!< Get whether or not the texture is high dynamic range
	inline bool [[nodiscard]] isDepth() const noexcept { return m_textureParams.type == TextureDataType::depth; } //!< Get whether or not the texture is a depth image
	inline bool [[nodiscard]] isMipped() const noexcept { return m_textureParams.isMipMapped; } //!< Get whether or not the texture has mip maps generated
protected:
	uint32_t m_ID{ 0 }; //!< Hold the GPU ID for the texture
protected:
	explicit TextureBase(const char* filepath); //!< Constructor which takes a path to an image file to be loaded
	TextureBase(const TextureDescription& desc, unsigned char* data); //!< Constructor which takes a TextureDescription
private:
	TextureDescription m_textureParams; //!< Parameter of this texture in a struct
private:
	void init(unsigned char* data); //!< Initialise the texture
};
