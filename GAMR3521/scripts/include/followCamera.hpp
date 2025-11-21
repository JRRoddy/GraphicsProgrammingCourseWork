#pragma once

#include "DemonRenderer.hpp"

class FollowCameraScript : public Script
{
public:
	FollowCameraScript(Actor& actor, GLFWWindowImpl& win, Actor& target, const glm::vec3& offset) :
		Script(actor),
		m_winRef(win),
		m_target(target),
		m_offset(offset)
	{}
	virtual void onUpdate(float timestep) override;
	virtual void onKeyPress(KeyPressedEvent& e) override {};
	virtual void onKeyRelease(KeyReleasedEvent& e) override {};
	void setOffsetZ(float z) { m_offset.z = z; }
	void setOffsetY(float y) { m_offset.z = y; }
private:
	GLFWWindowImpl& m_winRef;
	Actor& m_target;
	glm::vec3 m_offset;
	//  world space 
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 worldForward = glm::vec3(0.0f, 0.0f, -1.0f);
};
