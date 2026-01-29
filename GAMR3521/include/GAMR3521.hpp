#pragma once
#include <DemonRenderer.hpp>
#include <grid.hpp> 
#include <vector>
#include <numeric>


struct particle
{
	glm::vec4 origin;
	glm::vec4 position;
	glm::vec4 velocity;


};
struct vertex
{

	vertex(float x, float y, float z)
	{
		position = { x,y,z };
	}
	glm::vec3 position;
 
};

struct shadowMapVars {
	glm::vec3 center = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	float distanceAlongLightVec = 0.0f;  
	float orthoSize = 0.0f;
	
	shadowMapVars(float distanceAlongLightVector = 90.0f, glm::vec3 c = glm::vec3(0.0f, 0.0f, 0.0f)) : 
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
	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat,std::shared_ptr<VAO> depthVao,std::shared_ptr<Material> depthMat,glm::vec3 scale,std::shared_ptr<Scene> scene);

	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat, size_t & outId);
	void createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat, size_t& outId, std::shared_ptr<Scene>&Scene);
	void generateBilboards(std::vector<float>& positions, std::shared_ptr<VAO>  vao, std::shared_ptr<Material> material);
	void addPointLight(glm::vec3 colour, glm::vec3 position, glm::vec3 attenuation = { 1.f, 0.1f, 0.01f });
	void makePosProcessScreenPass(std::shared_ptr<Material> mat, std::shared_ptr<Scene> scene, std::shared_ptr<VAO> screenVAO,FBOLayout frameBufLayout = FBOLayout());
	void makePosProcessScreenPass(std::shared_ptr<Material> mat, std::shared_ptr<Scene> scene, std::shared_ptr<VAO> screenVAO, size_t & idx, FBOLayout frameBufLayout = FBOLayout());
	void addPointLights(int PointLightNum);
	ShaderDescription makeFragmentShaderDesc(std::filesystem::path vertPath, std::filesystem::path fragPath);
	void initLightPass(std::shared_ptr<VAO> screenQuad, FBOLayout lightPassLayout);
	void addPointLightDataToPass(RenderPass& pass, int pointLightNum);
	void makePaticleEmitter(glm::vec3 origin,std::shared_ptr<Material> particleMat,std::shared_ptr<Scene> scene,std::shared_ptr<Texture> texture, std::shared_ptr<VAO> vao,float particleBilBoardScale);
	void SetUpPostProcessingFlags();
	void makeForwardParticlePass(FBOLayout layout);
	void makePaticleComputePasses(glm::vec3 origin);
	void PreComputeSpecualrIndirectReflectance();
private:

	std::shared_ptr<SSBO> m_initParticleSSBO;
	std::shared_ptr<Material> m_particleMat;
	std::shared_ptr<Scene> m_scene; // Scene where actors reside
	std::shared_ptr<Scene> m_forwardPassScene;
	std::shared_ptr<Scene> m_skyboxScene;
	std::shared_ptr<Scene> m_lightPassScene;
	std::shared_ptr<Scene> m_PBRlightPassScene;
	std::shared_ptr<Scene> m_postProcessScene; 
	std::shared_ptr<Scene> m_colourInversionScene;
	std::shared_ptr<Scene> m_relativeLuminanceTintScene;

	std::shared_ptr<Scene> m_blurScene;
	std::shared_ptr<Scene> m_edgeDetectionScreenScene;
	std::shared_ptr<Texture> m_shadowMapTexture;
	std::shared_ptr<Scene> m_contrastScreenScene; 
	std::shared_ptr<Scene> m_saturationScreenScene;
	std::shared_ptr<Scene> m_visualiseDepthScreenScene;
	std::shared_ptr<Scene> m_PBRscene;
	std::shared_ptr<Scene> m_fogScreenScene;
	std::shared_ptr<Scene> m_shadowPrePassScene;
	std::shared_ptr<Scene> m_shadowPrePassVisualScreenScene;
	std::shared_ptr<Scene> m_normalVisualisationScene;
	std::shared_ptr<Scene> m_normalOverlayScene;
	std::shared_ptr<Scene> m_particleOverlayScene;
	std::shared_ptr<Scene> m_finalResult; 
	std::shared_ptr<Scene> m_combineFowardAndDefScene;
	std::shared_ptr<Material> m_PBRMat;
	Renderer m_particleInit;
	Renderer m_computeRenderer;
	Renderer m_renderer;			// Renderer to draw the scene
	size_t m_cameraIdx;				// Actor index of the camera, used to update scene
	size_t m_FloorIdx;              // Actor id to keep track of the floor within the actor buffer of the scene
	size_t m_skyBoxIdx;
	size_t m_combineForwardAndDefPassIdx;
	size_t m_linDepthPassIdx;
	size_t m_mainPassIdx;

	size_t m_PBRLightPassIdx;
	size_t m_deferredPrePasIdx;
	size_t m_deferredPBRPrePasIdx;
	size_t m_shadowMapPrepassIdx; 
	size_t m_shadowMapVisualisationIdx;
	size_t m_normalVisualIdx;
	size_t m_normalOverlayIdx;
	size_t m_luminanceSaturationIdx;
	size_t m_luminanceContrastIdx;
	size_t m_edgeDetectionIdx;
	size_t m_blurPassIdx;
	size_t m_fogPassIdx;
	size_t m_relativeLumianceTintIdx;
	size_t m_colourInversionIdx;
	uint32_t m_particleNum = 80;
	size_t m_heightMapComputeIdx;
	size_t m_cubeIdx;
	size_t m_updatePaticlesIdx;
	size_t m_skyBoxPassIdx;
	size_t m_forwardPassIdx;
	size_t CDMNormalsComputeIdx;
	size_t m_PBRDebugActorIdx;
	//post processing materials
	std::shared_ptr<Material> m_invertColourMat; 
	std::shared_ptr<Material> m_luminanceMat; 
	std::shared_ptr<Material> m_blurMat;
	std::shared_ptr<Material> m_edgeDetectionMat;
	std::shared_ptr<Material> m_luminanceSaturationMat; 
	std::shared_ptr<Material> m_luminanceContrastMat;
	std::shared_ptr<Material> m_visualiseDepthMat; 
	std::shared_ptr<Material> m_fogMat;
	std::shared_ptr<Material> m_gPassMat;
	std::shared_ptr<Material> m_gPassDiffuseOnly;
	std::shared_ptr<Material> m_phongModelMaterial;
	std::shared_ptr<Material> m_floorModelMaterial;
	std::shared_ptr<Material> m_shadowPrePassMat;
	std::shared_ptr<Material> m_normalVisMat;
	std::shared_ptr<Material> m_normalOverlayMat;
	std::shared_ptr<Material> m_terrainHeightMat;

	std::shared_ptr<Texture> m_computeTex;
	std::shared_ptr<Texture> m_heightMapTex;
	std::vector<std::shared_ptr<Material>> m_postProcessingMaterials;


	std::shared_ptr<Material> m_prefilterEnvMapMat;


	Renderer m_initIBLEnv;
	Renderer m_initIBLIr;
	Renderer m_initIBLPrefilterSpec;
	Renderer m_initBDRFlookUp;




	std::vector<int> m_postProcessingFlags;
	std::vector<std::string> m_PostProcessingNames;
	float m_screenWidth; 
	float m_screenHeight;
	std::vector<float> screenVertices;
	std::vector<uint32_t> screenIndices;
	int m_postProcessQuadIdx; 
	shadowMapVars m_shadowMapVariables;
	float m_farClippingPlane = 1000.0f;
	float m_nearClippingPlane = 0.7f;
	float m_fogFar = 10.0f; 
	int m_shadowAntiAliasingOn = 1;
	int m_normalOverlayOn = 0;
	float m_normalLength = 0.1f;
	glm::ivec2 m_shadowMapSize = { 4096, 4096 };
	int m_shadowMapSampleRadi = 1;
	float dt = 0.0f;
	float currentTime = 0.0f;
	float lastFrameTime = 0.0f;
	//Gui
	bool m_wireFrame{ false }; // render in wireframe 
	glm::vec3 m_tintColour = {1.0f,1.0f,1.0f};
	glm::vec3 m_fogColour = { 1.0f,1.0f,1.0f };
	float m_LuminanceSaturationScalar = 0.0f;// used in the stauration shader to define the distacne from the grey scale colour formed by calcualting the lumiance and usingit is a grey scale vec3 
	float m_LuminanceContrastScalar = 1.5f; // used to represent distance from grey in brightness(how exposed colours are)
	int m_blurRadius = 2;

	float m_LightDistance = 3.0f;
	glm::vec3 m_normalisedLightDir = {};
	glm::vec3 m_dirLightDirection = { -0.041f, -0.312f, -0.472f};
	glm::mat4 m_lightSpaceMat;
	std::shared_ptr<VAO> m_screenQuadVao;

	int PointLightNum = 7;

	int bilboardNum = 4;
	float m_bilboardScale = 10.0f;
	float m_terrainHeightOffset = -20.0f;
	int m_PBRDirLight = 1;
	int m_PBRPointLight = 1;
	float m_terrainFreq = 3.526f;
	float m_terrainAmp = 2.01f;
	float m_terrainLacrunarity = 2.813f;
	float m_terrainPersistance = 0.570f;
	int m_terrainGenOctaves = 4;
	int m_PBRDebugForTexturedModels = 0;

	int m_useRidgedNoise = 0;
	int m_useFBMNoise = 1;
	int m_useTurbulentNoise = 0;
	int m_useCombinedNoise = 0;


	float m_terrainHeightScalar = 30.0f;
	int m_shouldUseCDMNormals = 1;
	int m_perFragNormals = 0;
	int m_useTerrainHeightColour = 0;


	float m_particleAccel = 0.8f;

	float m_metalness = 0.0f;
	float m_roughness = 0.0f;
	glm::vec3 m_albedo = { 1.0f,1.0,1.0f };

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
	std::vector<vertex> m_cubeMapVerts = {
		// positions   
		

		vertex(-100.f, -100.f,  100.f),
		vertex(-100.f, -100.f, -100.f),
		vertex(-100.f,  100.f, -100.f),
		vertex(-100.f,  100.f,  100.f),


		vertex(100.f, -100.f, -100.f),
		vertex(100.f, -100.f,  100.f),
		vertex(100.f,  100.f,  100.f),
		vertex(100.f,  100.f, -100.f),

		vertex(-100.f,  100.f, -100.f),
		vertex(100.f,  100.f, -100.f),
		vertex(100.f,  100.f,  100.f),
		vertex(-100.f,  100.f,  100.f),
	    
		vertex(-100.f, -100.f, -100.f),
		vertex(-100.f, -100.f,  100.f),
		vertex(100.f, -100.f, -100.f),
		vertex(100.f, -100.f,  100.f),
		
		vertex(-100.f, -100.f,  100.f),
		vertex(-100.f,  100.f,  100.f),
		vertex(100.f,  100.f,  100.f),
		vertex(100.f, -100.f,  100.f),

		vertex(-100.f,  100.f, -100.f),
		vertex(-100.f, -100.f, -100.f),
		vertex( 100.f, -100.f, -100.f),
		vertex( 100.f,  100.f, -100.f),




	

	


		

	};								 
};									 