#include <glad/glad.h>
#include "buffers/FBO.hpp"
#include "core/log.hpp"

FBO::FBO(glm::ivec2 size, FBOLayout layout) : 
	m_layout(layout),
	m_size(size)
{
	glCreateFramebuffers(1, &m_ID);

	uint32_t colourAttachementCount = 0;
	uint32_t thisAttachment{ 0 };

	for (auto& elem : m_layout)
	{
		if (elem.isSampled)
		{
			if (elem.level > 0) assert(elem.isMipMapped);			// If level is not zero texture must be mipped
			if (elem.isMipMapped && elem.preExisingTexture) 
				assert(elem.preExisingTexture->isMipped());			// If elem is mipped any preexitsing texture must also be mipped
			if (!elem.isSampled) assert(!elem.preExisingTexture);	// If elem is not sampled there can be no pre-exisitng texture

			TextureDescription td;
			td.width = m_size.x;
			td.height = m_size.y;
			td.isMipMapped = elem.isMipMapped;

			switch (elem.type)
			{
			case AttachmentType::Colour:
				td.channels = 4;
				td.type = TextureDataType::SDR;
				if (!elem.preExisingTexture) m_sampledTargets.push_back(std::make_shared<Texture>(td, nullptr));
				else m_sampledTargets.push_back(elem.preExisingTexture);
				thisAttachment = GL_COLOR_ATTACHMENT0 + colourAttachementCount;
				glNamedFramebufferTexture(m_ID, thisAttachment, m_sampledTargets.back()->getID(), elem.level);
				m_colAttachments.push_back(thisAttachment);
				colourAttachementCount++;
				break;
			case AttachmentType::ColourHDR:
				td.channels = 4;
				td.type = TextureDataType::HDR;
				if (!elem.preExisingTexture) m_sampledTargets.push_back(std::make_shared<Texture>(td, nullptr));
				else m_sampledTargets.push_back(elem.preExisingTexture);
				thisAttachment = GL_COLOR_ATTACHMENT0 + colourAttachementCount;
				glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + colourAttachementCount, m_sampledTargets.back()->getID(), elem.level);
				m_colAttachments.push_back(thisAttachment);
				colourAttachementCount++;
				break;
			case AttachmentType::Depth:
				td.channels = 1;
				td.type = TextureDataType::depth;
				td.wrapMethod = GL_CLAMP_TO_EDGE;
				td.minFilterMethod = GL_NEAREST;
				td.minFilterMethod = GL_NEAREST;
				//glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				//glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				//glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				//glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				if (!elem.preExisingTexture) m_sampledTargets.push_back(std::make_shared<Texture>(td, nullptr));
				else m_sampledTargets.push_back(elem.preExisingTexture);
				glNamedFramebufferTexture(m_ID, GL_DEPTH_ATTACHMENT, m_sampledTargets.back()->getID(), elem.level);
				break;
			default:
				spdlog::error("Unsupported FBO sampled attachment type: {}", static_cast<int>(elem.type));
				break;
			}
		}
		else
		{
			switch (elem.type)
			{
			case AttachmentType::Colour:
				m_nonSampledTargets.push_back(std::make_shared<RBO>(elem.type, size));
				glNamedFramebufferRenderbuffer(m_ID, GL_COLOR_ATTACHMENT0 + colourAttachementCount, GL_RENDERBUFFER, m_nonSampledTargets.back()->getID());
				colourAttachementCount++;
				break;
			case AttachmentType::Depth:
				m_nonSampledTargets.push_back(std::make_shared<RBO>(elem.type, size));
				glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nonSampledTargets.back()->getID());
				break;
			case AttachmentType::DepthAndStencil:
				m_nonSampledTargets.push_back(std::make_shared<RBO>(elem.type, size));
				glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_nonSampledTargets.back()->getID());
				break;
			default:
				spdlog::error("Unsupported FBO non-sampled attachment type: {}", static_cast<int>(elem.type));
				break;
			}
		}
		GLenum status = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
	}

	if (colourAttachementCount == 0)
	{
		glNamedFramebufferDrawBuffer(m_ID,GL_NONE);
		glNamedFramebufferReadBuffer(m_ID,GL_NONE);
	}

	if (colourAttachementCount > 1) {   //MTR
		glNamedFramebufferDrawBuffers(m_ID, colourAttachementCount, m_colAttachments.data());
	}

	GLenum status = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::error("Framebuffer is not complete! Reason:");

		switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED: spdlog::error("GL_FRAMEBUFFER_UNDEFINED"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: spdlog::error("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: spdlog::error("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: spdlog::error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: spdlog::error("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
		case GL_FRAMEBUFFER_UNSUPPORTED: spdlog::error("GL_FRAMEBUFFER_UNSUPPORTED"); break;
		default: spdlog::error("Unknown error"); break;
		}
	}
}

FBO::~FBO()
{
	m_sampledTargets.clear();
	m_nonSampledTargets.clear();
	if(m_ID > 0) glDeleteFramebuffers(1, &m_ID);
}

void FBO::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FBO::onResize(WindowResizeEvent& e)
{
	// TO DO
	// Destroy and remake framebuffer and targets, sampled and non-sampled
}

std::shared_ptr<Texture> FBO::getTarget(uint32_t index)
{
	std::shared_ptr<Texture> result = nullptr;

	if (index < m_sampledTargets.size()) result = m_sampledTargets[index];

	return result;
}


