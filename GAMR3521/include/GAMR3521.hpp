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
	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO>  Vao, std::shared_ptr<Material> mat);

	void addPointLight(glm::vec3 colour, glm::vec3 position, glm::vec3 attenuation = { 1.f, 0.1f, 0.01f });

	void addPointLights(int PointLightNum);


	void addPointLightDataToPass(RenderPass& pass, int pointLightNum);

private:
	std::shared_ptr<Scene> m_scene; // Scene where actors reside
	Renderer m_renderer;			// Renderer to draw the scene
	size_t m_cameraIdx;				// Actor index of the camera, used to update scene
	size_t m_FloorIdx;              // Actor id to keep track of the floor within the actor buffer of the scene
	//Gui
	bool m_wireFrame{ false }; // render in wireframe 
	glm::vec3 floorColour = { 1.0f,1.0f,1.0f };// floor colour manipulated by a colour wheel define using ImGui 
	int PointLightNum = 10;

	std::array<const char*, 6> cubeMapPaths = {
	"./assets/textures/Skybox/right.png",
	"./assets/textures/Skybox/left.png",
	"./assets/textures/Skybox/top.png",
	"./assets/textures/Skybox/bottom.png",
	"./assets/textures/Skybox/front.png",
	"./assets/textures/Skybox/back.png",
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