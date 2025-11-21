/** \file computePass.hpp*/
#pragma once

#include <memory>
#include "rendering/material.hpp"
#include "assets/image.hpp"

/** \enum MemoryBarrier
*	Barriers which cause compute shaders to wait, essentially a synchonisation mechanism
*/
enum class MemoryBarrier : GLbitfield {
	None = 0,
	ShaderImageAccess = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
	ShaderStorageAccess = GL_SHADER_STORAGE_BARRIER_BIT,
	VertexAttribArray = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
	ElementArray = GL_ELEMENT_ARRAY_BARRIER_BIT,
	Uniform = GL_UNIFORM_BARRIER_BIT,
	FrameBuffer = GL_FRAMEBUFFER_BARRIER_BIT,
	All = GL_ALL_BARRIER_BITS
};


/**	\struct ComputePass
*	\brief A render pass which executes a compute shader
*/
struct ComputePass
{
	std::shared_ptr<SSBO> ssbo;
	std::shared_ptr<Material> material;
	glm::ivec3 workgroups{ glm::ivec3{0,0,0} };
	MemoryBarrier barrier{ MemoryBarrier::None };
	std::vector<Image> images;
};