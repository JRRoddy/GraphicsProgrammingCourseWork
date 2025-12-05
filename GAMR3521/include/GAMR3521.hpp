#pragma once
#include <DemonRenderer.hpp>
#include <grid.hpp> 
#include <vector>
#include <numeric>


struct shadowMapVars {
	glm::vec3 center = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	float distanceAlongLightVec = 0.0f;  
	float orthoSize = 0.0f;
	
	shadowMapVars(float distanceAlongLightVector = 60.0f, glm::vec3 c = glm::vec3(0.0f, 0.0f, 0.0f)) : 
	 distanceAlongLightVec(distanceAlongLightVector), center(c) {

		orthoSize = distanceAlongLightVec * 0.75f;

	}


	




};

class MainLayer : public Layer
{
public:
	MainLayer(GLFWWindowImpl& win);
protected:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onImGUIRender() override;
	void createActors(int num,float coordRangeMin, float coordRangeMax, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat);
	void createActors(int num, float coordRangeMin, float coordRangeMax, std::shared_ptr<VAO> Vao,std::shared_ptr<VAO> depthVAO, std::shared_ptr<Material> mat,std::shared_ptr<Material> depthMat);

	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat , std::shared_ptr<VAO> depthVao, std::shared_ptr<Material> depthMat);

	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat);
	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat, size_t & outId);

	void addPointLight(glm::vec3 colour, glm::vec3 position, glm::vec3 attenuation = { 1.f, 0.1f, 0.01f });

	void addPointLights(int PointLightNum);


	void addPointLightDataToPass(RenderPass& pass, int pointLightNum);
	void SetUpPostProcessingFlags();
private:



	std::shared_ptr<Scene> m_scene; // Scene where actors reside
	std::shared_ptr<Scene> m_postProcessScene; 

	std::shared_ptr<Scene> m_blurScene;
	std::shared_ptr<Scene> m_edgeDetectionScreenScene;
	std::shared_ptr<Texture> m_shadowMapTexture;
	std::shared_ptr<Scene> m_contrastScreenScene; 
	std::shared_ptr<Scene> m_saturationScreenScene;
	std::shared_ptr<Scene> m_visualiseDepthScreenScene;
	std::shared_ptr<Scene> m_fogScreenScene;
	std::shared_ptr<Scene> m_shadowPrePassScene;
	std::shared_ptr<Scene> m_shadowPrePassVisualScreenScene;
	std::shared_ptr<Scene> m_finalResult; 

	Renderer m_renderer;			// Renderer to draw the scene
	size_t m_cameraIdx;				// Actor index of the camera, used to update scene
	size_t m_FloorIdx;              // Actor id to keep track of the floor within the actor buffer of the scene
	size_t m_skyBoxIdx;
	size_t m_linDepthPassIdx;
	size_t m_mainPassIdx;
	size_t m_zPrePasIdx;
	size_t m_shadowMapPrepassIdx; 
	size_t m_shadowMapVisualisationIdx;
	//post processing materials
	std::shared_ptr<Material> m_invertColourMat; 
	std::shared_ptr<Material> m_luminanceMat; 
	std::shared_ptr<Material> m_blurMat;
	std::shared_ptr<Material> m_edgeDetectionMat;
	std::shared_ptr<Material> m_luminanceSaturationMat; 
	std::shared_ptr<Material> m_luminanceContrastMat;
	std::shared_ptr<Material> m_visualiseDepthMat; 
	std::shared_ptr<Material> m_fogMat;
	std::shared_ptr<Material> m_phongModelMaterial;
	std::shared_ptr<Material> m_floorModelMaterial;

	std::vector<std::shared_ptr<Material>> m_postProcessingMaterials;
	std::vector<int> m_postProcessingFlags;
	std::vector<std::string> m_PostProcessingNames;
	float m_screenWidth; 
	float m_screenHeight;
	std::vector<float> screenVertices;
	std::vector<uint32_t> screenIndices;
	int m_postProcessQuadIdx; 
	shadowMapVars m_shadowMapVariables;
	float m_farClippingPlane = 1000.0f;
	float m_nearClippingPlane = 0.1f;
	float m_fogFar = 10.0f; 
	int m_shadowAntiAliasingOn = 1;

	glm::ivec2 m_shadowMapSize = { 4096, 4096 };
	int m_shadowMapSampleRadi = 1;
	//Gui
	bool m_wireFrame{ false }; // render in wireframe 
	glm::vec3 m_floorColour = { 0.35f,0.0f,0.0f };// floor colour manipulated by a colour wheel define using ImGui 
	glm::vec3 m_tintColour = {1.0f,1.0f,1.0f};
	glm::vec3 m_fogColour = { 1.0f,1.0f,1.0f };
	float m_LuminanceSaturationScalar = 0.0f;// used in the stauration shader to define the distacne from the grey scale colour formed by calcualting the lumiance and usingit is a grey scale vec3 
	float m_LuminanceContrastScalar = 1.5f; // used to represent distance from grey in brightness(how exposed colours are)
	int m_blurRadius = 2;

	float m_LightDistance = 3.0f;
	glm::vec3 m_normalisedLightDir = {};
	glm::vec3 m_dirLightDirection = { -0.041f, -0.312f, -0.472};
	glm::mat4 m_lightSpaceMat;


	int PointLightNum = 7;

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