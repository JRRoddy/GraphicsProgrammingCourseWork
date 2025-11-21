#pragma once
#include "rendering/depthOnlyPass.hpp"

/**	\struct RenderPass
*	\brief A render pass which only performs rasterisation
*/
class RenderPass : public UBOCacheBasePass
{
public:
	Camera camera; //!< Camera for the pass
	std::shared_ptr<FBO> target; //!< Render target
	std::shared_ptr<Scene> scene; //!< Scene being rendered
	std::vector<std::function<void()>> prePassActions; //!< Functions called before pass happens
	std::vector<std::function<void()>> postPassActions; //!< Functions called after pass happens
	ViewPort viewPort; //!< Portion of the render target being rendered too
	bool clearColour{ true };//!< Should the colour buffer be cleared by this parse?
	bool clearDepth{ true }; //!< Should the depth buffer be cleared by this parse?

	void parseScene(); //!< Populate variables based on the scene
};

