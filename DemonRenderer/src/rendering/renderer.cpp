#pragma once

#include "rendering/renderer.hpp"
#include <assert.h>

size_t Renderer::addRenderPass(const RenderPass& pass)
{
	size_t result = m_renderOrder.size();
	m_renderOrder.push_back(std::pair<PassType, size_t>(PassType::render, m_renderPasses.size()));
	m_renderPasses.push_back(pass);
	return result;
}

size_t  Renderer::addDepthPass(const DepthPass& depthPass)
{
	size_t result = m_renderOrder.size();
	m_renderOrder.push_back(std::pair<PassType, size_t>(PassType::depth, m_depthPasses.size()));
	m_depthPasses.push_back(depthPass);
	return result;
}

size_t  Renderer::addComputePass(const ComputePass& pass)
{
	size_t result = m_renderOrder.size();
	m_renderOrder.push_back(std::pair<PassType, size_t>(PassType::compute, m_computePasses.size()));
	m_computePasses.push_back(pass);
	return result;
}

RenderPass& Renderer::getRenderPass(size_t index)
{
	assert(isRenderPass(index));
	auto& [passType, passIdx] = m_renderOrder[index];
	return m_renderPasses[passIdx];
}

DepthPass& Renderer::getDepthPass(size_t index)
{
	assert(isDepthPass(index));
	auto& [passType, passIdx] = m_renderOrder[index];
	return m_depthPasses[passIdx];
}

ComputePass& Renderer::getComputePass(size_t index)
{
	assert(isComputePass(index));
	auto& [passType, passIdx] = m_renderOrder[index];
	return m_computePasses[passIdx];
}

bool Renderer::isRenderPass(size_t index)
{
	return m_renderOrder.at(index).first == PassType::render;
}

bool Renderer::isDepthPass(size_t index)
{
	return m_renderOrder.at(index).first == PassType::depth;
}

bool Renderer::isComputePass(size_t index)
{
	return m_renderOrder.at(index).first == PassType::compute;
}

void Renderer::render() const
{
	for (auto& [passType, idx] : m_renderOrder)
	{
		if (passType == PassType::render)
		{
			auto& renderPass = m_renderPasses[idx];

			for (auto& func : renderPass.prePassActions) func();

			renderPass.target->use();
			glViewport(renderPass.viewPort.x, renderPass.viewPort.y, renderPass.viewPort.width, renderPass.viewPort.height);

			if (renderPass.clearDepth && renderPass.clearColour) {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			else if (renderPass.clearDepth) {
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			else if (renderPass.clearColour) {
				glClear(GL_COLOR_BUFFER_BIT);
			}

			renderPass.uploadCachedValues();

			// Iterate over all actors in the scene
			for (auto it = renderPass.scene->m_actors.begin(); it != renderPass.scene->m_actors.end(); ++it)
			{
				if (it->material) {
					it->material->apply();
					if (it->material->getTransformUniformName().length() > 0) it->material->m_shader->uploadUniform(it->material->getTransformUniformName(), it->transform);

					if (it->geometry) { // All geometry is indexed i.e. it uses IBOs
						glBindVertexArray(it->geometry->getID());
						glDrawElements(it->material->getPrimitive(), it->geometry->getDrawCount(), GL_UNSIGNED_INT, NULL);
					}
					else if (it->SSBOgeometry) {
						glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, it->SSBOgeometry->getID());  // binding point from material
						glDrawArrays(it->material->getPrimitive(), 0, it->SSBOgeometry->getElementCount());
					}
				}
			}

			for (auto& func : renderPass.postPassActions) func();
		}
		else if (passType == PassType::depth)
		{
			glCullFace(GL_FRONT);
			auto& depthPass = m_depthPasses[idx];

			for (auto& func : depthPass.prePassActions) func();

			depthPass.target->use();
			glViewport(depthPass.viewPort.x, depthPass.viewPort.y, depthPass.viewPort.width, depthPass.viewPort.height);

			if (depthPass.clearDepth) {
				glClear(GL_DEPTH_BUFFER_BIT);
			}

			depthPass.uploadCachedValues();

			// Iterate over all actors in the scene
			for (auto it = depthPass.scene->m_actors.begin(); it != depthPass.scene->m_actors.end(); ++it)
			{
				if (it->depthMaterial) {
					it->depthMaterial->apply();
					if (it->depthMaterial->getTransformUniformName().length() > 0) it->depthMaterial->m_shader->uploadUniform(it->depthMaterial->getTransformUniformName(), it->transform);

					if (it->depthGeometry) { // All geometry is indexed i.e. it uses IBOs
						glBindVertexArray(it->depthGeometry->getID());
						glDrawElements(it->depthMaterial->getPrimitive(), it->depthGeometry->getDrawCount(), GL_UNSIGNED_INT, NULL);
					}
					else if (it->SSBOgeometry) {
						glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, it->SSBOgeometry->getID());  // binding point from material
						glDrawArrays(it->depthMaterial->getPrimitive(), 0, it->SSBOgeometry->getElementCount());
					}
				}
			}

			for (auto& func : depthPass.postPassActions) func();

			glCullFace(GL_BACK);
		}
		else if (passType == PassType::compute)
		{
			auto& computePass = m_computePasses[idx];

			// Bind images -- Note: Could consider an image unit manager if this becomes a hot path
			for (auto& img : computePass.images) {
				GLboolean layered = img.getLayered() ? GL_TRUE : GL_FALSE;
				
				GLenum access = 0;
				switch (img.getAccess())
				{
				case TextureAccess::ReadOnly:
					access = GL_READ_ONLY; 
					break;
				case TextureAccess::WriteOnly:
					access = GL_WRITE_ONLY;
					break;
				case TextureAccess::ReadWrite:
					access = GL_READ_WRITE;
					break;
				}

				GLenum fmt = 0; // Ignoring depth for now
				if (img.isHDR()) {
					if (img.getChannels() == 3) fmt = GL_RGB16F;
					else fmt = GL_RGBA16F;
				}
				else {
					if (img.getChannels() == 3) fmt = GL_RGB8;
					else fmt = GL_RGBA8;
				}
				glBindImageTexture(img.getImageUnit(), img.getID(), img.getMipLevel(), layered, 0, access, fmt);
			}

			computePass.material->apply();
			auto& wg = computePass.workgroups;
			glDispatchCompute(wg.x, wg.y, wg.z);
			if(computePass.barrier != MemoryBarrier::None) glMemoryBarrier(static_cast<GLbitfield>(computePass.barrier));
		}
	}
}

