#pragma once

#include "buffers/FBO.hpp"
#include "rendering/scene.hpp"
#include "rendering/camera.hpp"
#include "rendering/UBObasePass.hpp"

/** \struct ViewPort
*	\brief Portion of the target taken up by a render pass*/
struct ViewPort {
	int32_t x{ 0 };
	int32_t y{ 0 };
	uint32_t width{ 0 };
	uint32_t height{ 0 };
};

/**	\class DepthPass
*	\brief A render pass which only performs depth calculations
*/
class DepthPass : public UBOCacheBasePass
{
public:
	Camera camera; //!< Camera for the pass
	std::shared_ptr<FBO> target; //!< Render target
	std::shared_ptr<Scene> scene; //!< Scene being rendered
	std::vector<std::function<void()>> prePassActions; //!< Functions called before pass happens
	std::vector<std::function<void()>> postPassActions; //!< Functions called after pass happens
	ViewPort viewPort; //!< Portion of the render target being rendered too
	bool clearDepth{ true }; //!< Should the depth buffer be cleared by this parse?

	void parseScene() override; //!< Populate variables based on the scene
};
