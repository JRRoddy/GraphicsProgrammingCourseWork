#include "followCamera.hpp"

//void FollowCameraScript::onUpdate(float timestep)
//{
//	glm::vec3 targetRight = { m_actor.transform[0][0], m_actor.transform[0][1], m_actor.transform[0][2] };
//	glm::vec3 targetUp = { m_actor.transform[1][0], m_actor.transform[1][1], m_actor.transform[1][2] };
//	glm::vec3 targetForward = { -m_target.transform[2][0], -m_target.transform[2][1], -m_target.transform[2][2] };
//
//	m_actor.translation = m_target.translation;
//	m_actor.translation += targetRight * m_offset.x;
//	m_actor.translation += targetUp * m_offset.y;
//	m_actor.translation += targetForward * m_offset.z;
//
//	m_actor.rotation = m_target.rotation * glm::quat(glm::vec3(0.f, glm::pi<float>(), 0.f));
//	
//	m_actor.recalc();
//}


void FollowCameraScript::onUpdate(float timestep)
{


    //  camera above and behind actor
    m_actor.translation = m_target.translation;
    m_actor.translation += worldUp * m_offset.y;
    m_actor.translation += worldForward * m_offset.z;

    //look-at rotation
    glm::mat4 lookAtMatrix = glm::lookAt(
        m_actor.translation,   // Camera 
        m_target.translation,  // Target 
        worldUp
    );

    //  lookAt matrix to a quaternion 
    m_actor.rotation = glm::quat_cast(lookAtMatrix);
    m_actor.recalc();



}
