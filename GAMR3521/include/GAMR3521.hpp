#pragma once
#include <DemonRenderer.hpp>
#include <grid.hpp> 
#include <numeric>

class MainLayer : public Layer
{
public:
	MainLayer(GLFWWindowImpl& win);
protected:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onImGUIRender() override;
	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat);
	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat, size_t & outId);

	void addPointLight(glm::vec3 colour, glm::vec3 position, glm::vec3 attenuation = { 1.f, 0.1f, 0.01f });

	void addPointLights(int PointLightNum);


	void addPointLightDataToPass(RenderPass& pass, int pointLightNum);

private:
	std::shared_ptr<Scene> m_scene; // Scene where actors reside
	std::shared_ptr<Scene> m_postProcessScene;
	Renderer m_renderer;			// Renderer to draw the scene
	size_t m_cameraIdx;				// Actor index of the camera, used to update scene
	size_t m_FloorIdx;              // Actor id to keep track of the floor within the actor buffer of the scene
	size_t m_skyBoxIdx;
	
	float m_screenWidth; 
	float m_screenHeight;
	std::vector<float> screenVertices;
	std::vector<uint32_t> screenIndices;

	//Gui
	bool m_wireFrame{ false }; // render in wireframe 
	glm::vec3 floorColour = { 0.35f,0.0f,0.0f };// floor colour manipulated by a colour wheel define using ImGui 
	int PointLightNum = 20;

	std::array<const char*, 6> cubeMapPaths = {
	"./assets/textures/oGLDevSkybox/sp3right.jpg",
	"./assets/textures/oGLDevSkybox/sp3left.jpg",
	"./assets/textures/oGLDevSkybox/sp3top.jpg",
	"./assets/textures/oGLDevSkybox/sp3bot.jpg",
	"./assets/textures/oGLDevSkybox/sp3front.jpg",
	"./assets/textures/oGLDevSkybox/sp3back.jpg",
	};
	std::vector<uint32_t> skyboxIndices;
	std::vector<float> skyboxVertices = {
		// positions          
		-100.f,  100.f, -100.f,
		-100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		 100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f, -100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f,  100.f,
		-100.f, -100.f,  100.f,

		 100.f, -100.f, -100.f,
		 100.f, -100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f, -100.f,
		 100.f, -100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f, -100.f,  100.f,
		-100.f, -100.f,  100.f,

		-100.f,  100.f, -100.f,
		 100.f,  100.f, -100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		-100.f,  100.f,  100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		 100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		 100.f, -100.f,  100.f
	};

};