
/** \file bindlessCubeMap.hpp */
#pragma once

#include <array>

/**
* \class BindlessCubeMap
* \brief Creates a cubemap from texture files.
* Bindless cube maps use bindless textures meaning bindless cube maps cannot be edited after creation.
* However, bindless cubemaps do not take up a texture unit.
* RenderDoc cannot read bindless cubemaps and will crash.
*/

class BindlessCubeMap
{
public:
	BindlessCubeMap() = delete; //!< Deleted default constructor
	BindlessCubeMap(const std::array<const char*, 6>& filepaths, bool isHDR = false, bool flipUvs = true); //!< Cubemap built from 6 seperate files, is it HR and should UVs be flipped
	BindlessCubeMap(BindlessCubeMap& other) = delete; //!< Deleted copy constructor
	BindlessCubeMap(BindlessCubeMap&& other) = delete; //!< Deleted move constructor
	BindlessCubeMap& operator=(BindlessCubeMap& other) = delete; //!< Deleted copy assignment operator
	BindlessCubeMap& operator=(BindlessCubeMap&& other) = delete; //!< Delete move assignment operator
	~BindlessCubeMap(); //!< Destructor which removes the cubemap from the GPU
	inline uint64_t [[nodiscard]] getHandle() noexcept { return m_handle; } //!< Get the ID
	inline bool [[nodiscard]] isHDR() const noexcept { return m_isHDR; } //!< Is this cubemap HDR
private:
	uint64_t m_handle{ 0 }; //!< Holds the GPU handle for the cubemap
	uint32_t m_ID{ 0 }; //!< Holds the GPU ID for the texture
	bool m_isHDR{ false }; //!< Holds whether or not this cubemap high dynamic range
};
