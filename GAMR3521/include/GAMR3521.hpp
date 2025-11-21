#pragma once
#include <DemonRenderer.hpp>
#include <grid.hpp>

class MainLayer : public Layer
{
public:
	MainLayer(GLFWWindowImpl& win);
protected:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onImGUIRender() override;
private:
	std::shared_ptr<Scene> m_scene; // Scene where actors reside
	Renderer m_renderer;			// Renderer to draw the scene
	size_t m_cameraIdx;				// Actor index of the camera, used to update scene
	//Gui
	bool m_wireFrame{ false }; // render in wireframe
};