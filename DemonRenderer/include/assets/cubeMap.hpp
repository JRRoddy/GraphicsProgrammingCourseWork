/** \file cubeMap.hpp */
#pragma once

#include <array>
#include "assets/managedTexture.hpp"
#include "assets/TextureBase.hpp"

/**
* \class CubeMap
* \brief Creates a cubemap from texture files.
* Cube maps use standard textures meaning cube maps can be edited after creation
* To make dynamic cubemaps use two cubemaps and interpolate.
*/
class CubeMap : public ManagedTexture
{
public:
	CubeMap() = delete; //!< Deleted default constructor
	CubeMap(const std::array<const char*,6>& filepaths, bool isHDR = false, bool flipUvs = true); //!< Cubemap built from 6 seperate files, is it HDR and should UVs be flipped
	CubeMap(const TextureDescription& textureDesc, uint32_t mips); //!< Cubemap built from texture description and number of mipLevels
	CubeMap(CubeMap& other) = delete; //!< Deleted copy constructor
	CubeMap(CubeMap&& other) = delete; //!< Deleted move constructor
	CubeMap& operator=(CubeMap& other) = delete; //!< Deleted copy assignment operator
	CubeMap& operator=(CubeMap&& other) = delete; //!< Delete move assignment operator
	~CubeMap(); //!< Destructor which removes the cubemap from the GPU
	inline bool [[nodiscard]] isHDR() const noexcept{ return m_isHDR; } //!< Is this cubemap HDR
	inline uint32_t [[nodiscard]] getID() const noexcept { return m_ID; } //!< Get the ID
	inline uint32_t [[nodiscard]] getChannels() const noexcept { return m_channels; }
	void generateMipmaps(); //!< generate mipmaps for cubemap. Useful if cubemap is initially empty and subsequently written to
private:
	uint32_t m_ID{ 0 }; //!< Hold the GPU ID for the texture
	bool m_isHDR{ false }; //!< Holds whether or not this cubemap high dynamic range
	uint32_t m_channels{ 0 }; //!< Holds number of channels
};
