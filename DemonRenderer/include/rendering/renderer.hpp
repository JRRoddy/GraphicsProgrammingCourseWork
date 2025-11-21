/** \file renderer.hpp */
#pragma once

#include "rendering/renderPass.hpp"
#include "rendering/depthOnlyPass.hpp"
#include "rendering/computePass.hpp"

/**	\class Renderer
*	\brief Holds and executes a series of render passes
*/

class Renderer
{
public:
	Renderer() = default; //!< Default constructor
	size_t addRenderPass(const RenderPass& renderPass); //!< Add a render pass, returns index
	size_t addDepthPass(const DepthPass& renderPass); //!< Add a depth only pass, returns index
	size_t addComputePass(const ComputePass& renderPass);  //!< Add a compute pass, returns index
	RenderPass& getRenderPass(size_t index); //!< Get a render pass
	DepthPass& getDepthPass(size_t index);  //!< Get a depth only pass
	ComputePass& getComputePass(size_t index);  //!< Get a compute pass
	[[nodiscard]] size_t getPassCount() { return m_renderOrder.size(); } //!< Number of passes in the renderer
	bool isRenderPass(size_t index) ; //!< Is the pass at index a render pass?
	bool isDepthPass(size_t index); //!< Is the pass at index a depth only pass?
	bool isComputePass(size_t index); //!< Is the pass at index a compute pass?
	void render() const; //!< Execute all render passes
private:
	/** \enum PassType 
	*	Type of render pass
	*/
	enum class PassType {compute, depth, render}; 
	std::vector<RenderPass> m_renderPasses; //!< Internal storage for render passes
	std::vector<DepthPass> m_depthPasses; //!< Internal storage for depth only passes
	std::vector<ComputePass> m_computePasses; //!< Internal storage for compute passes
	std::vector<std::pair<PassType, size_t>> m_renderOrder; //!< Internal storage or order of passes, similar to a sparse set
};