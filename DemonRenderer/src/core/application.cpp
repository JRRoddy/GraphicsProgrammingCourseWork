/** \file application.cpp */

#include "core/application.hpp"

Application::Application(const WindowProperties& winProps)
{
	m_window.open(winProps);

	m_window.handler.onWinClose = [this](WindowCloseEvent& e) {onClose(e);};
	m_window.handler.onKeyPress = [this](KeyPressedEvent& e) {onKeyPressed(e);};
	m_window.handler.onKeyRelease = [this](KeyReleasedEvent& e) {onKeyReleased(e);};

	auto setupTime = m_timer.reset();
}

void Application::run() 
{
    spdlog::debug("Application running");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (m_running) {		
		auto timestep = m_timer.reset();

		onUpdate(timestep);
		if(m_window.isHostingImGui()) onImGuiRender();
		onRender();

		m_window.onUpdate(timestep);
	}
}

void Application::onUpdate(float timestep)
{
	m_layerStack.for_each(
		[timestep](std::unique_ptr<Layer>& layerRef) {
			layerRef->onUpdate(timestep);
		}
	);
}

void Application::onRender() const
{
	m_layerStack.for_each(
		[](const std::unique_ptr<Layer>& layerRef) {
			layerRef->onRender();
		}
	);
}

void Application::onImGuiRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (m_ImGuiOpen)
	{
		ImGui::Begin("ImGUI Window", &m_ImGuiOpen, ImGuiWindowFlags_MenuBar);
		if(!m_ImGuiOpen) spdlog::info("ImGui window closed. Press I to reopen it.");
		
		m_layerStack.for_each(
			[](std::unique_ptr<Layer>& layerRef) {
				layerRef->onImGUIRender();
			}
		);

		ImGui::End();
	}

	ImGui::Render();
}

void Application::onClose(WindowCloseEvent& e)
{
	m_running = false;
	m_layerStack.for_each(
		[&e](std::unique_ptr<Layer>& layerRef) {
			layerRef->onClose(e);
		}
	);
	e.handle();
}

void Application::onResize(WindowResizeEvent& e)
{
	m_layerStack.for_each(
		[&e](std::unique_ptr<Layer>& layerRef) {
			layerRef->onResize(e);
		}
	);
}

void Application::onKeyPressed(KeyPressedEvent& e)
{
	if (e.getKeyCode() == GLFW_KEY_ESCAPE) {
		spdlog::info("Exiting");
		m_running = false;
		e.handle();
		return;
	}

	else{
		if (e.getKeyCode() == GLFW_KEY_I) m_ImGuiOpen = true;
		m_layerStack.for_each(
			[&e](std::unique_ptr<Layer>& layerRef) {
				layerRef->onKeyPressed(e);
			}
		);
		e.handle();
	}

}

void Application::onKeyReleased(KeyReleasedEvent& e)
{
	m_layerStack.for_each(
		[&e](std::unique_ptr<Layer>& layerRef) {
			layerRef->onKeyReleased(e);
		}
	);
}

void Application::onMousePress(MouseButtonPressedEvent& e)
{
	m_layerStack.for_each(
		[&e](std::unique_ptr<Layer>& layerRef) {
			layerRef->onMousePress(e);
		}
	);
}

void Application::onMouseReleased(MouseButtonReleasedEvent& e)
{
	m_layerStack.for_each(
		[&e](std::unique_ptr<Layer>& layerRef) {
			layerRef->onMouseReleased(e);
		}
	);
}

void Application::onMouseMoved(MouseMovedEvent& e)
{
	m_layerStack.for_each(
		[&e](std::unique_ptr<Layer>& layerRef) {
			layerRef->onMouseMoved(e);
		}
	);
}

void Application::onMouseScrolled(MouseScrolledEvent& e)
{
	m_layerStack.for_each(
		[&e](std::unique_ptr<Layer>& layerRef) {
			layerRef->onMouseScrolled(e);
		}
	);
}
