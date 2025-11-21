
#include "controllerWASD.hpp"
#include "core/log.hpp"
#include <iostream>



void ControllerWASDScript::onUpdate(float timestep) {

    float deceleration = 2.0f;
    // forward and right vectors
    glm::vec3 forward = glm::normalize(glm::vec3(-m_actor.transform[2][0], -m_actor.transform[2][1], -m_actor.transform[2][2])); // -Z axis
    glm::vec3 right = glm::normalize(glm::vec3(m_actor.transform[0][0], m_actor.transform[0][1], m_actor.transform[0][2]));     // +X axis

    // Move forward continuously
    m_actor.translation += forward * m_movementSpeed.z * timestep;

    // rotation
    if (m_winRef.isKeyPressed(GLFW_KEY_D)) {
        float theta = m_movementSpeed.y * -timestep; // Rotate right
        glm::quat delta(glm::vec3(0.f, theta, 0.f));
        m_actor.rotation *= delta;
        m_actor.translation += right * m_movementSpeed.y * timestep;
    }
    if (m_winRef.isKeyPressed(GLFW_KEY_A)) {
        float theta = m_movementSpeed.y * timestep; // Rotate left
        glm::quat delta(glm::vec3(0.f, theta, 0.f));
        m_actor.rotation *= delta;
        m_actor.translation += right * m_movementSpeed.y * timestep;
    }

    //move forward
    if (m_winRef.isKeyPressed(GLFW_KEY_W)) {
        m_movementSpeed.z = std::clamp(m_movementSpeed.z - timestep, m_maxSpeed, m_minSpeed);
        forward = glm::normalize(glm::vec3(-m_actor.transform[2][0], -m_actor.transform[2][1], -m_actor.transform[2][2]));
        m_actor.translation += forward * m_movementSpeed.z * timestep;
    }
    else if (m_winRef.isKeyPressed(GLFW_KEY_S)) {
        m_movementSpeed.z = std::clamp(m_movementSpeed.z + timestep, m_maxSpeed, m_minSpeed);
        forward = glm::normalize(glm::vec3(-m_actor.transform[2][0], -m_actor.transform[2][1], -m_actor.transform[2][2]));
        m_actor.translation -= forward * m_movementSpeed.z * timestep;
    }
    else {
        // Decelerate 
        if (m_movementSpeed.z > 0.0f) {
            m_movementSpeed.z = std::max(m_movementSpeed.z - deceleration * timestep, 0.0f);
        }
        else if (m_movementSpeed.z < 0.0f) {
            m_movementSpeed.z = std::min(m_movementSpeed.z + deceleration * timestep, 0.0f);
        }

    }
    m_actor.recalc();

    
    *m_worldSpacePosition = glm::vec3(m_actor.transform * glm::vec4(m_rearPositionLocal, 1.0f));
    //*m_speedOut = m_movementSpeed.z;
    *modelVelocity = m_movementSpeed.z;
    //std::cout << *modelVelocity << " " << m_movementSpeed.z << std::endl;
}



