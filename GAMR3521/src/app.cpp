#include "app.hpp"
#include "GAMR3521.hpp"

App::App(const WindowProperties& winProps) : Application(winProps)
{
	m_layerStack.push(std::make_unique<MainLayer>(MainLayer(m_window)));
}

Application* startApplication()
{
	WindowProperties props("DemonRenderer Demo", 800, 600);
	props.isHostingImGui = true;
	props.isResizable = false;
	return new App(props);
}