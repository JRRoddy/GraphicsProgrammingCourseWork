#pragma once

#include <glm/glm.hpp>

#include "gameObjects/actor.hpp"
#include "windows/GLFWWindowImpl.hpp"
#include "core/log.hpp"
#include <cmath>

class ControllerWASDScript : public Script
{
public:
	ControllerWASDScript(Actor& actor, GLFWWindowImpl& win, Actor& followCamera, const glm::vec3& movementSpeed, std::shared_ptr<float> velocity , std::shared_ptr<glm::vec3> posWorldSpace) :
		Script(actor),
		m_winRef(win),
		m_followCamera(followCamera),
		m_movementSpeed(movementSpeed),
		modelVelocity(velocity)
	{
		//*m_speedOut = 0.0;
		m_worldSpacePosition = posWorldSpace;
	}
	void onUpdate(float timestep) override;
	void onKeyPress(KeyPressedEvent& e) override {};
	void onKeyRelease(KeyReleasedEvent& e) override {};
	void onImGuiRender() override {};
private:
	//float easeOutSine(float x);

	GLFWWindowImpl& m_winRef;
	Actor& m_followCamera;
	glm::vec3 m_movementSpeed{ 0.f, 0.f, 0.f };
	const float m_minSpeed{ -0.0f };
	const float m_maxSpeed{ -10.5f };
	//float* m_speedOut{ nullptr };
	float m_velocity{0.0f};
	std::shared_ptr<glm::vec3> m_worldSpacePosition;
	std::shared_ptr<float> modelVelocity;

	const glm::vec3 m_rearPositionLocal{ glm::vec3(0.0f, 0.0f, -3.1f) };  // -4z rocket, -1z spaceship, current has 'incorrect' axis alighined on X
};
