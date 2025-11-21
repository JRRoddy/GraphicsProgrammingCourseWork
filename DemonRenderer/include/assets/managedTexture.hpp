/** \file managedTexture.hpp */
#pragma once

/** \class ManagedTexture
*   \brief A managed texture is one whose device binding point is managed by the texture unit manager.
*   You should inherit this class into any texture you wish who binding point you want managed.
*	It is not a standalone class, hence the protected constructor.
*/

#include "assets/textureUnitManager.hpp"

class ManagedTexture
{
public:
	uint32_t [[nodiscard]] getUnit(uint32_t m_ID); //!< Get the texture unit, don't worry about flushing etc.
	inline uint32_t [[nodiscard]] inspectSlot(uint32_t slot) {return ManagedTexture::m_textureUnitManager[slot]; }; //!<< Get the ID od the texture bound to slot
	[[nodiscard]] static bool isTUMFull() { return m_textureUnitManager.full(); } //!< Is the Texture Unit Manager full?
	static void clearTUM() { m_textureUnitManager.clear(); }; //!< CLear Texture Unit Manager, resets all slots
protected:
	ManagedTexture() = default; //!< Default constructor, required by subclasses
private:
	/**
	* Manages which slots (32 in total) textures are bound to.
	* Static as we only want one record of this for all managed textures.
	*/
	inline static TextureUnitManager m_textureUnitManager = TextureUnitManager(32);
};