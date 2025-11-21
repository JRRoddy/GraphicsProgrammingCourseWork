/** \file FBOlayout.hpp*/
#pragma once

#include <vector>
#include <memory>
#include "assets/texture.hpp"

/** \enum AttachmentType
*	Possible attachments for a frame buffer */
enum class AttachmentType { Colour, ColourHDR, Depth, Stencil, DepthAndStencil };

/** \struct FBOAttachment
*	\brief Settings for each texture attached to a framebuffer */
struct Attachment {
	AttachmentType type{ AttachmentType::Colour }; //!< type of attachment
	bool isSampled{ false }; //!< can the attachement be sampled
	bool isMipMapped{ false }; //!< should mipmaps be generated for the texture. For existing texture these need to match.
	uint8_t level{ 0 }; //!< Mip level for this attachment. If no equal to zero isMipMapped must be true
	std::shared_ptr<Texture> preExisingTexture; //!< Shared pointer to pre-existing texture
};

/** \class FBOLayout
*	\brief Provides the structure which describes an FBO */

class FBOLayout
{
public:
	FBOLayout() {}; //!< Default constructor
	/** Initializer list constructor i.e.
	*	FBOLayout layout = {
	*		{{AttachmentType::ColourHDR, true},
	*		{ AttachmentType::Depth, false }
	*	}};
	*/
	FBOLayout(const std::initializer_list<Attachment>& attachements, uint32_t stride = 0) : m_attachments(attachements) {}
	std::vector<Attachment>::iterator begin() { return m_attachments.begin(); } //!< Begin iterator
	std::vector<Attachment>::iterator end() { return m_attachments.end(); } //!< End iterator
	std::vector<Attachment>::const_iterator begin() const { return m_attachments.begin(); }//!< Begin const iterator
	std::vector<Attachment>::const_iterator end() const { return m_attachments.end(); } //!< End const iterator
private:
	std::vector<Attachment> m_attachments; //!< Container for attachments
};