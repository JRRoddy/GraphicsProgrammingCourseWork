#include "GAMR3521.hpp"
#include "camera.hpp"

MainLayer::MainLayer(GLFWWindowImpl& win) : Layer(win)
{

	// here this essentially refreshes the scene and assings a new refernce to the smart pointer holding the scene 
	// the scene has buffers that hold all of the current data for the scene such as lighting and objects 
	m_scene.reset(new Scene);
	m_forwardPassScene.reset(new Scene);
	m_skyboxScene.reset(new Scene);
	m_lightPassScene.reset(new Scene);
	m_postProcessScene.reset(new Scene);
	m_relativeLuminanceTintScene.reset(new Scene);
	m_colourInversionScene.reset(new Scene);
	m_blurScene.reset(new Scene);
	m_edgeDetectionScreenScene.reset(new Scene);
	m_contrastScreenScene.reset(new Scene);
	m_saturationScreenScene.reset(new Scene);
	m_visualiseDepthScreenScene.reset(new Scene);
	m_fogScreenScene.reset(new Scene);
	m_shadowPrePassScene.reset(new Scene);
	m_shadowPrePassVisualScreenScene.reset(new Scene);
	m_combineFowardAndDefScene.reset(new Scene);
	m_finalResult.reset(new Scene);
	m_normalVisualisationScene.reset(new Scene);
	m_normalOverlayScene.reset(new Scene);

	m_particleInit = Renderer();
	m_computeRenderer = Renderer();
	// creating zpre pass mat here so we can use it for all actors below 
	ShaderDescription deferredPassDesc;
	deferredPassDesc.type = ShaderType::rasterization;
	deferredPassDesc.vertexSrcPath = "./assets/shaders/deferredPrePassVert.glsl";
	deferredPassDesc.fragmentSrcPath = "./assets/shaders/deferredPrePassFrag.glsl";

	std::shared_ptr<Shader> deferredPrePassShader = std::make_shared<Shader>(deferredPassDesc);

	m_gPassMat = std::make_shared<Material>(deferredPrePassShader);

	ShaderDescription deferredDiffuseOnlyPassDesc;
	deferredDiffuseOnlyPassDesc.type = ShaderType::tessellationAndGeometry;
	deferredDiffuseOnlyPassDesc.vertexSrcPath = "./assets/shaders/diffuseOnlyDRPassVert.glsl";
	deferredDiffuseOnlyPassDesc.controlSrcPath = "./assets/shaders/floorTSControl.glsl";
	deferredDiffuseOnlyPassDesc.evaluationSrcPath = "./assets/shaders/floorTSEval.glsl";
	deferredDiffuseOnlyPassDesc.geometrySrcPath = "./assets/shaders/floorGeo.glsl";
	deferredDiffuseOnlyPassDesc.fragmentSrcPath = "./assets/shaders/diffuseOnlyDRPassFrag.glsl";

	std::shared_ptr<Shader> deferredDiffuseOnlyPrePassShader = std::make_shared<Shader>(deferredDiffuseOnlyPassDesc);

	m_gPassDiffuseOnly = std::make_shared<Material>(deferredDiffuseOnlyPrePassShader);

	m_gPassDiffuseOnly->setPrimitive(GL_PATCHES);
	m_shadowMapVariables = shadowMapVars();

	ShaderDescription shadowPrePassDesc;

	shadowPrePassDesc.type = ShaderType::rasterization;
	shadowPrePassDesc.vertexSrcPath = "./assets/shaders/shadowMapPrePassVert.glsl";
	shadowPrePassDesc.fragmentSrcPath = "./assets/shaders/shadowMapPrePassFrag.glsl";

	std::shared_ptr<Shader> shadowPrePassShader = std::make_shared<Shader>(shadowPrePassDesc);

	m_shadowPrePassMat = std::make_shared<Material>(shadowPrePassShader);



	glm::vec3 lightPosition = (m_shadowMapVariables.center - (m_dirLightDirection)) * m_shadowMapVariables.distanceAlongLightVec;
	glm::mat4 lightSpaceView = glm::lookAt(lightPosition, m_shadowMapVariables.center, m_shadowMapVariables.up);

	// create a descriptor for a particualr shader we want to make deifning the type of shader(int this case rasterization shader that uses both a vertex and fragment shader ) 

	ShaderDescription phongShaderDesc;
	phongShaderDesc.type = ShaderType::rasterization;
	// define path for vertex shader 
	phongShaderDesc.vertexSrcPath = "./assets/shaders/phongVert.glsl";
	// define path for the fragement shader
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/phongFrag.glsl";

	std::shared_ptr<Shader> phongShader;
	// create the shader and assing it as a refernce to a shared ptr passing in the shader description 
	phongShader = std::make_shared<Shader>(phongShaderDesc);


	// extracting vertex data from grid 
	std::shared_ptr<Grid> floorGrid = std::make_shared<Grid>();
	std::vector<float>  floorGridVertecies = floorGrid->getVertices();
	std::vector<unsigned int> FloorElementIndicies = floorGrid->getIndices();
	std::vector<float>  floorGridPositions = floorGrid->getVertexPositions();

	// defining layout for floor vertex data
	VBOLayout FloorLayout{
		{GL_FLOAT,3},
		{GL_FLOAT,2},
	};


	VBOLayout depthLayout{
	{GL_FLOAT,3},

	};

	// defining floor vertex data 
	std::shared_ptr<VAO> FloorGridVAO;

	FloorGridVAO = std::make_shared<VAO>(FloorElementIndicies);

	FloorGridVAO->addVertexBuffer(floorGridVertecies, FloorLayout);

	std::shared_ptr<VAO> floorVaoDepth = std::make_shared<VAO>(FloorElementIndicies);
	floorVaoDepth->addVertexBuffer(floorGridPositions, depthLayout);


	// creating floor textures
	


	std::shared_ptr<Texture> FloorTexture;
	FloorTexture = std::make_shared<Texture>("./assets/textures/rock_terrain.jpg");
	std::shared_ptr<Texture> FloorSecondaryTexture;
	FloorSecondaryTexture = std::make_shared<Texture>("./assets/textures/rock_face.jpg");

	std::shared_ptr<Texture> heightMap = std::make_shared<Texture>("./assets/textures/heightMap.jpg");
	std::shared_ptr<Texture> normalMapRock = std::make_shared<Texture>("./assets/textures/rock_face_normal.png");
	std::shared_ptr<Texture> normalMapTerrain = std::make_shared<Texture>("./assets/textures/rocky_terrain_normal.png");
	m_gPassDiffuseOnly->setValue("u_albedoMap", FloorTexture);	// initialsiing the floor actor using the previously defined data 
	m_gPassDiffuseOnly->setValue("u_secondaryAlbedoMap", FloorSecondaryTexture);	// initialsiing the floor actor using the previously defined data 
	m_gPassDiffuseOnly->setValue("u_heightScalar", m_terrainHeightScalar);
	m_gPassDiffuseOnly->setValue("u_terrainHeightOffset", m_terrainHeightOffset);
	m_gPassDiffuseOnly->setValue("u_shouldUseCDM", m_shouldUseCDMNormals);
	m_gPassDiffuseOnly->setValue("u_heightColActive", m_useTerrainHeightColour);
	m_gPassDiffuseOnly->setValue("u_normalMap", normalMapTerrain);
	m_gPassDiffuseOnly->setValue("u_secondNormalMap", normalMapRock);

	Actor FloorActor;
	FloorActor.geometry = FloorGridVAO;
	FloorActor.material = m_gPassDiffuseOnly;
	FloorActor.depthGeometry = floorVaoDepth;
	//FloorActor.depthMaterial = m_shadowPrePassMat;
	// define the translation 
	FloorActor.translation = glm::vec3{ -50.0f,-5.0f,-50.0f };
	FloorActor.recalc();
	m_FloorIdx = m_scene->m_actors.size();
	m_scene->m_actors.push_back(FloorActor);


	ShaderDescription bilboardShaderDesc;
	bilboardShaderDesc.type = ShaderType::geometry;
	bilboardShaderDesc.vertexSrcPath = "./assets/shaders/bilboardVert.glsl";
	bilboardShaderDesc.geometrySrcPath = "./assets/shaders/bilboardGeo.glsl";
	bilboardShaderDesc.fragmentSrcPath = "./assets/shaders/bilboardFrag.glsl";

	std::shared_ptr<Shader> bilboardShader = std::make_shared<Shader>(bilboardShaderDesc);
	std::shared_ptr<Texture> bilboardTexture = std::make_shared<Texture>("./assets/textures/tree.png");
	std::shared_ptr<Material> bilboardMat = std::make_shared<Material>(bilboardShader);

	bilboardMat->setPrimitive(GL_POINTS);
	bilboardMat->setValue("u_scale", m_bilboardScale);
	bilboardMat->setValue("u_bilBoardTexture", bilboardTexture);

	std::vector<float> bilboardPositions = {};
	std::vector<uint32_t> bilboardIndicies;

	for (int i = 0; i < bilboardNum; i++) {

		bilboardPositions.push_back(Randomiser::uniformFloatBetween(-40.0f, 40.0f));
		bilboardPositions.push_back(m_bilboardScale - 5.0f);
		bilboardPositions.push_back(Randomiser::uniformFloatBetween(-40.0f, 40.0f));
		bilboardIndicies.push_back(i);

	}


	std::shared_ptr<VAO> bilboardVAO = std::make_shared<VAO>(bilboardIndicies);

	bilboardVAO->addVertexBuffer(bilboardPositions, { {GL_FLOAT,3} });


	generateBilboards(bilboardPositions, bilboardVAO, bilboardMat);



	uint32_t Attributes = Model::VertexFlags::positions | Model::VertexFlags::normals |
		Model::VertexFlags::uvs | Model::VertexFlags::tangents;
	// create the cube model 
	Model cubeModel("./assets/models/whitecube/whitecube.obj", Attributes);
	// defining the layout of the data that will be allocated on the GPU 
	VBOLayout cubeLayout = {
		{GL_FLOAT, 3}, // Position
		{GL_FLOAT, 3}, // Normal
		{GL_FLOAT, 2}, // UV co-ords
		{GL_FLOAT,3}   // tangent

	};



	// creare the buffer that will store the vertex data that is described by the layout we defined above for the cube VAO 
	std::shared_ptr<VAO> cubeVAO;
	cubeVAO = std::make_shared<VAO>(cubeModel.m_meshes[0].indices);
	//// defining the vertex data for the VBO using the mesh data at index 0 because the cube model only has one mesh
	//// also passing in the layout of the data to be allocated on the GPU 
	cubeVAO->addVertexBuffer(cubeModel.m_meshes[0].vertices, cubeLayout);
	//// create and define the diffuse texture that will be used as the base colour for the cube 
	std::shared_ptr<Texture> cubeTextureDiffuse;
	//// grab the diffuse texture of the cube mesh at index 0 using the defined enmum (as an index) then  taking the cstring of the texture path we accessed 
	cubeTextureDiffuse = std::make_shared<Texture>(cubeModel.m_meshes[0].texturePaths[aiTextureType_DIFFUSE].string().c_str());

	std::shared_ptr<Texture> cubeNormal = std::make_shared<Texture>("./assets/textures/normal.jpg");
	std::shared_ptr<Shader> cubeShader = std::make_shared<Shader>(deferredPassDesc);
	std::shared_ptr<Material> gpass = std::make_shared<Material>(cubeShader);
	// making a material out of the shader we made and setting the diffuse map to be used by the phong lighting 
	// we also set a scalar value for the diffuse colour 
	std::shared_ptr<Material> cubeMaterial;
	cubeMaterial = gpass;

	//// atttach the diffuse map we created for the cube after extracting the diffuse texture path from its texture paths 
	cubeMaterial->setValue("u_albedoMap", cubeTextureDiffuse);
	cubeMaterial->setValue("u_normalMap", cubeNormal);
	std::shared_ptr<VAO> cubeDepth = std::make_shared<VAO>(cubeModel.m_meshes[0].indices);
	cubeDepth->addVertexBuffer(cubeModel.m_meshes[0].positions, depthLayout);
	//// Actor represents an object
	Actor cube;
	//// set the cube geomtry to be the  defined VAO that has the vbo for the vertex data of the cube bound to it 
	cube.geometry = cubeVAO;
	//// set the material of the cube to be the one defined above that is using the phong lighting shader and the texture that we extracted from the cubes model data and loaded (using the texture path)
	cube.material = cubeMaterial;
	cube.depthMaterial = m_shadowPrePassMat;
	cube.depthGeometry = cubeDepth;
	////  define model matrix for cube 
	cube.translation = glm::vec3(0.f, -1.f, -6.f);
	cube.rotation = glm::quat(glm::vec3(0.0f, 0.4f, 0.0f));
	cube.recalc();
	m_cubeIdx = m_scene->m_actors.size();
	m_scene->m_actors.push_back(cube);

	VBOLayout modelLayout = {
		{GL_FLOAT, 3}, // Position
		{GL_FLOAT, 3}, // Normal
		{GL_FLOAT, 2}, // UV co-ords
		{GL_FLOAT,3}   // tangent
	};




	Model model = Model("./assets/models/Vampire/vampire.obj", Attributes);
	std::shared_ptr<VAO> ModelVAO = std::make_shared<VAO>(model.m_meshes[0].indices);
	ModelVAO->addVertexBuffer(model.m_meshes[0].vertices, modelLayout);

	std::shared_ptr<VAO> modelVaoDepth = std::make_shared<VAO>(model.m_meshes[0].indices);
	modelVaoDepth->addVertexBuffer(model.m_meshes[0].positions, depthLayout);

	std::shared_ptr<Texture> modelDiffuseTexture = std::make_shared<Texture>("./assets/models/Vampire/textures/diffuse.png");
	std::shared_ptr<Texture> modelSpecularTexture = std::make_shared<Texture>("./assets/models/Vampire/textures/specular.png");
	std::shared_ptr<Texture> modelNormalTexture = std::make_shared<Texture>("./assets/models/Vampire/textures/normal.png");

	m_phongModelMaterial = std::make_shared<Material>(phongShader);

	//ModelMaterial->setValue("u_albedo", glm::vec3(1.0f)); 
	/*m_phongModelMaterial->setValue("u_albedoMap", modelDiffuseTexture);
	m_phongModelMaterial->setValue("u_specularMap", modelSpecularTexture);
	m_phongModelMaterial->setValue("u_normalMap", modelNormalTexture);
	*/
	m_gPassMat->setValue("u_albedoMap", modelDiffuseTexture);
	m_gPassMat->setValue("u_specularMap", modelSpecularTexture);
	m_gPassMat->setValue("u_normalMap", modelNormalTexture);

	createActor(glm::vec3(0.0f, -3.0f, -11.0f), ModelVAO, m_gPassMat, modelVaoDepth, m_shadowPrePassMat);
	createActors(10, -10.0f, 10.0f, ModelVAO, modelVaoDepth, m_gPassMat, m_shadowPrePassMat);
	ShaderDescription sandingShaderDesc;
	sandingShaderDesc.type = ShaderType::geometry;
	sandingShaderDesc.vertexSrcPath = "./assets/shaders/sandingVert.glsl";
	sandingShaderDesc.geometrySrcPath = "./assets/shaders/sandingGeo.glsl";
	sandingShaderDesc.fragmentSrcPath = "./assets/shaders/sandingFrag.glsl";

	std::shared_ptr<Shader> sandingShader = std::make_shared<Shader>(sandingShaderDesc);

	std::shared_ptr<Material> sandingMat = std::make_shared<Material>(sandingShader);


	createActor(glm::vec3(-3.0f, -3.0f, -11.0f), ModelVAO, sandingMat, modelVaoDepth, m_shadowPrePassMat);


	ShaderDescription normalVisShaderDesc;
	normalVisShaderDesc.type = ShaderType::geometry;
	normalVisShaderDesc.vertexSrcPath = "./assets/shaders/normalVisualVert.glsl";
	normalVisShaderDesc.geometrySrcPath = "./assets/shaders/normalVisualGeo.glsl";
	normalVisShaderDesc.fragmentSrcPath = "./assets/shaders/normalVisualFrag.glsl";

	std::shared_ptr<Shader> normalVisShader = std::make_shared<Shader>(normalVisShaderDesc);


	std::shared_ptr<Material> normalVisMat = std::make_shared<Material>(normalVisShader);
	normalVisMat->setValue("u_normalLength", m_normalLength);
	m_normalVisualisationScene->m_actors = std::vector(m_scene->m_actors);



	//skybox 

	// calculate the number of indidces required for the sky box data 
	// to ensure that it can be drawn via an element buffer 
	skyboxIndices = std::vector<uint32_t>(skyboxVertices.size() / 3);
	// this algorithm will start at the value defined in the third argument 
	// and increment and assign the value to each subbsequent element we allocated space for in the vector 
	// so we get the indices 0-35 in this case 
	std::iota(skyboxIndices.begin(), skyboxIndices.end(), 0);


	//We only need the positions for each vertex of the sky box as it is centred at the origin at all times 
	// we can use the coordinates derrived from the vertex positions of the skybox as the texture look up coordinates 
	// to determine which texture face of the sky box we need to sample from 
	VBOLayout skyBoxLayout = { {GL_FLOAT,3} };

	std::shared_ptr<VAO> skyBoxVao = std::make_shared<VAO>(skyboxIndices);

	skyBoxVao->addVertexBuffer(skyboxVertices, skyBoxLayout);

	ShaderDescription skyBoxShaderDesc;
	skyBoxShaderDesc.type = ShaderType::rasterization;
	skyBoxShaderDesc.vertexSrcPath = "./assets/shaders/skyBoxVert.glsl";
	skyBoxShaderDesc.fragmentSrcPath = "./assets/shaders/skyBoxFrag.glsl";

	std::shared_ptr<Shader> skyBoxShader = std::make_shared<Shader>(skyBoxShaderDesc);
	std::shared_ptr<CubeMap> skyBoxMap = std::make_shared<CubeMap>(cubeMapPaths, false, false);

	std::shared_ptr<Material> skyBoxMat = std::make_shared<Material>(skyBoxShader, "u_model");

	skyBoxMat->setValue("u_cubeMap", skyBoxMap);

	createActor(glm::vec3(0.0f, 0.0f, 0.0f), skyBoxVao, skyBoxMat, m_skyBoxIdx, m_skyboxScene);




	// add  a directional light to the scene 
	DirectionalLight dl;
	dl.direction = glm::normalize(m_dirLightDirection);




	m_lightPassScene->m_directionalLights.push_back(dl);

	addPointLights(PointLightNum);


	// add a camera to the scene 
	Actor camera;
	// take the camera id using the size before pushing back to get the index
	m_cameraIdx = m_scene->m_actors.size();
	m_scene->m_actors.push_back(camera);

	// sepcifcy that we want the colour buffer to use HDR and set sample to true meaning it will be used 
	// also specifcy the depth attachement which will not be used currently
	FBOLayout TypicalLayout = {
		{AttachmentType::ColourHDR, true, true},
		{AttachmentType::Depth, false, false}

	};



	m_screenWidth = m_winRef.getWidthf();
	m_screenHeight = m_winRef.getHeightf();
	VBOLayout screenQuadLayout = {
		{GL_FLOAT, 3},
		{GL_FLOAT, 2}
	};
	screenVertices = {
		// Position            UV
		0.0f,  0.0f,   0.0f,  0.0f, 1.0f,  // Bottom-left corner
		m_screenWidth, 0.0f,   0.0f,  1.0f, 1.0f,  // Bottom-right corner
		m_screenWidth, m_screenHeight, 0.0f,  1.0f, 0.0f,  // Top-right corner
		0.0f,  m_screenHeight, 0.0f,  0.0f, 0.0f   // Top-left corner
	};
	screenIndices = { 0,1,2,2,3,0 };

	std::shared_ptr<VAO> ScreenQuadVAO = std::make_shared<VAO>(screenIndices);

	ScreenQuadVAO->addVertexBuffer(screenVertices, screenQuadLayout);
	// defining post processing shader




	FBOLayout colAndDepthLayout = {
	   {AttachmentType::ColourHDR, true, true},
	   {AttachmentType::Depth, true, false}
	};
	FBOLayout depthLayoutFBO{
	   {AttachmentType::Depth, true, false},

	};

	FBOLayout g_BufferLayout
	{
		{AttachmentType::ColourHDR,true},
		{AttachmentType::ColourHDR,true},
		{AttachmentType::ColourHDR,true},
		{AttachmentType::ColourHDR,true},
		{AttachmentType::Depth,true},

	};


	/*uint32_t particleBuffSize = (sizeof(particle) * m_particleNum);
	m_initParticleSSBO = std::make_shared<SSBO>(particleBuffSize, m_particleNum);
	m_initParticleSSBO->bind(0);

	ShaderDescription particleInitShaderDesc;
	particleInitShaderDesc.type = ShaderType::compute;
	particleInitShaderDesc.computeSrcPath = "./assets/shaders/computeParticleInit.glsl";
	std::shared_ptr<Shader> particleInitShader = std::make_shared<Shader>(particleInitShaderDesc);

	std::shared_ptr<Material> particleInitMat = std::make_shared<Material>(particleInitShader);


	ComputePass initParticles;
	initParticles.barrier = MemoryBarrier::ShaderStorageAccess;
	initParticles.material = particleInitMat;
	initParticles.ssbo = m_initParticleSSBO;

	initParticles.workgroups = { 32,32,1 };

	m_particleInit.addComputePass(initParticles);
	ShaderDescription particleUpdateShaderDesc;
	particleUpdateShaderDesc.type = ShaderType::compute;
	particleUpdateShaderDesc.computeSrcPath = "./assets/shaders/computeParticleUpdate.glsl";
	std::shared_ptr<Shader> particleUpdateShader = std::make_shared<Shader>(particleUpdateShaderDesc);

	std::shared_ptr<Material> particleUpdateMat = std::make_shared<Material>(particleUpdateShader);
	particleUpdateMat->setValue("u_particleAcceleration", m_particleAccel);
	ComputePass updateParticles;
	updateParticles.barrier = MemoryBarrier::ShaderStorageAccess;
	updateParticles.material = particleUpdateMat;
	updateParticles.ssbo = m_initParticleSSBO;
	updateParticles.workgroups = { 32,32,1 };

	m_updatePaticlesIdx = m_computeRenderer.getPassCount();
	m_computeRenderer.addComputePass(updateParticles);
	ShaderDescription particleShaderDesc;
	particleShaderDesc.type = ShaderType::geometry;
	particleShaderDesc.vertexSrcPath = "./assets/shaders/particleEmitVert.glsl";
	particleShaderDesc.geometrySrcPath = "./assets/shaders/particleEmitGeo.glsl";
	particleShaderDesc.fragmentSrcPath = "./assets/shaders/particleEmitFrag.glsl";
	std::shared_ptr<Shader> particleShader = std::make_shared<Shader>(particleShaderDesc);
	std::shared_ptr<Material> particleMat = std::make_shared<Material>(particleShader);
	particleMat->setPrimitive(GL_POINTS);
	std::shared_ptr<Texture> particleTexture = std::make_shared<Texture>("./assets/textures/spark.png");
	glm::vec3 particleOrigin = glm::vec3(-3.0f, -3.0f, -11.0f);
	makePaticleEmitter(particleOrigin, particleMat, m_particleScene, particleTexture, 0.3f);
	initParticles.material->setValue("u_particleOrigin", particleOrigin);*/
	ShaderDescription particleShaderDesc;
	particleShaderDesc.type = ShaderType::geometry;
	particleShaderDesc.vertexSrcPath = "./assets/shaders/particleEmitVert.glsl";
	particleShaderDesc.geometrySrcPath = "./assets/shaders/particleEmitGeo.glsl";
	particleShaderDesc.fragmentSrcPath = "./assets/shaders/particleEmitFrag.glsl";
	std::shared_ptr<Shader> particleShader = std::make_shared<Shader>(particleShaderDesc);
	std::shared_ptr<Material> particleMat = std::make_shared<Material>(particleShader);
	std::shared_ptr<Texture> particleTexture = std::make_shared<Texture>("./assets/textures/spark.png");
	particleMat->setPrimitive(GL_POINTS);
	particleMat->setValue("u_particleTexture", particleTexture);
	particleMat->setValue("u_particleScale", 0.3f);
	glm::vec3 particleOrigin = glm::vec3(-3.0f, -3.0f, -11.0f);
	std::vector<float> vect = {};
	std::vector<uint32_t> vert = std::vector<uint32_t>();
	for (int i = 0; i < m_particleNum; i++)
	{
		vert.push_back(i);
	}
	
	std::shared_ptr<VAO> vao = std::make_shared<VAO>(vert);

	
	createActor(glm::vec3(-5.0f, -5.0f, -11.0f), vao, particleMat);

	makePaticleComputePasses();

	



	RenderPass deferredPrePass;
	deferredPrePass.scene = m_scene;
	deferredPrePass.parseScene();
	deferredPrePass.target = std::make_shared<FBO>(m_winRef.getSize(), g_BufferLayout);
	deferredPrePass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	deferredPrePass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	deferredPrePass.camera.updateView(m_scene->m_actors.at(m_cameraIdx).transform);
	deferredPrePass.setCachedValue("b_camera", "u_view", deferredPrePass.camera.view);
	deferredPrePass.setCachedValue("b_camera", "u_projection", deferredPrePass.camera.projection);
	deferredPrePass.setCachedValue("b_camera", "u_viewPos", m_scene->m_actors.at(m_cameraIdx).translation);
	deferredPrePass.prePassActions.push_back([]() {glDisable(GL_BLEND); });
	deferredPrePass.postPassActions.push_back([]() {glEnable(GL_BLEND); });
	

	m_deferredPrePasIdx = m_renderer.getPassCount();
	m_renderer.addRenderPass(deferredPrePass);

	/*RenderPass forwardPass;
	forwardPass.scene = m_forwardPassScene;
	forwardPass.parseScene();
	forwardPass.target = std::make_shared<FBO>(m_winRef.getSize(), colAndDepthLayout);
	forwardPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	forwardPass.camera.projection = deferredPrePass.camera.projection;
	forwardPass.setCachedValue("b_camera", "u_view", deferredPrePass.camera.view);
	forwardPass.setCachedValue("b_camera", "u_projection", deferredPrePass.camera.projection);
	forwardPass.setCachedValue("b_camera", "u_viewPos", m_scene->m_actors.at(m_cameraIdx).translation);
	m_forwardPassIdx = m_renderer.getPassCount();
	m_renderer.addRenderPass(forwardPass);*/

	DepthPass shadowMapPrePass;

	shadowMapPrePass.scene = m_scene;
	shadowMapPrePass.parseScene();
	shadowMapPrePass.target = std::make_shared<FBO>(m_shadowMapSize, depthLayoutFBO);
	
	 
	shadowMapPrePass.camera.view = lightSpaceView;
	shadowMapPrePass.camera.projection = glm::ortho(
		-m_shadowMapVariables.orthoSize,
		m_shadowMapVariables.orthoSize,
		-m_shadowMapVariables.orthoSize,
		m_shadowMapVariables.orthoSize,
		-m_shadowMapVariables.orthoSize / 5,
		m_shadowMapVariables.orthoSize * 5);

	shadowMapPrePass.viewPort = { 0, 0, 4096, 4096 };

	shadowMapPrePass.setCachedValue("b_lightCamera", "u_view", shadowMapPrePass.camera.view);
	shadowMapPrePass.setCachedValue("b_lightCamera", "u_projection", shadowMapPrePass.camera.projection);
	m_shadowMapPrepassIdx = m_renderer.getPassCount();
	m_renderer.addDepthPass(shadowMapPrePass);
	
	/*m_phongModelMaterial->setValue("u_lightSpaceMatrix", shadowMapPrePass.camera.projection * shadowMapPrePass.camera.view);
	m_floorModelMaterial->setValue("u_lightSpaceMatrix", shadowMapPrePass.camera.projection * shadowMapPrePass.camera.view);
	*/



	RenderPass skyBoxPass;

	skyBoxPass.scene = m_skyboxScene;
	skyBoxPass.parseScene();
	skyBoxPass.target = std::make_shared<FBO>(m_winRef.getSize(), colAndDepthLayout);
	skyBoxPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	skyBoxPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);

	skyBoxPass.setCachedValue("b_camera", "u_projection", skyBoxPass.camera.projection);
   
	skyBoxMat->setValue("u_skyBoxView", glm::mat(glm::mat3(deferredPrePass.camera.view)));
	m_skyBoxPassIdx = m_renderer.getPassCount();
	m_renderer.addRenderPass(skyBoxPass);

	
	m_phongModelMaterial->setValue("u_shadowMap", m_renderer.getDepthPass(m_shadowMapPrepassIdx).target->getTarget(0));
	m_phongModelMaterial->setValue("u_antiAliasingOn", m_shadowAntiAliasingOn);
	m_phongModelMaterial->setValue("u_fragmentPositions", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(0));
	m_phongModelMaterial->setValue("u_normalMap", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(1));
	m_phongModelMaterial->setValue("u_diffSpecMap", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(2));
	m_phongModelMaterial->setValue("u_prePassDepthTexture", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(4));
	m_phongModelMaterial->setValue("u_skyBoxColBuffer", m_renderer.getRenderPass(m_skyBoxPassIdx).target->getTarget(0));
	m_phongModelMaterial->setValue("u_fragmentId", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(3));
	//m_phongModelMaterial->setValue("u_forwardDepth", m_renderer.getRenderPass(m_forwardPassIdx).target->getTarget(0));
	//m_phongModelMaterial->setValue("u_forwardCol", m_renderer.getRenderPass(m_forwardPassIdx).target->getTarget(0));

	//m_phongModelMaterial->setValue("u_fpd", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(0));
	//m_phongModelMaterial->setValue("u_forwardPassColBuf", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(1));
	Actor lightPassQuad;
	lightPassQuad.geometry = ScreenQuadVAO;
	lightPassQuad.material = m_phongModelMaterial;

	m_lightPassScene->m_actors.push_back(lightPassQuad);




	// initialise a particualr pass for the renderer to perfrom rendering is often sperated inot particualr passes 
	// as certain operations need to be performed in particualr order 
	RenderPass mainPass;
	// assigng the scene for the pass to manipulate 
	mainPass.scene = m_lightPassScene;
	// extract all the neccessary information from all the actors we defined in the scene to be used by the shader pass(generally things that define 
	// the look or surface of the actors like materials)
	mainPass.parseScene();
	//mainPass.target = std::make_shared<FBO>();
	// in process of adding post processing
	mainPass.target = std::make_shared<FBO>(m_winRef.getSize(), colAndDepthLayout); // Default framebuffer

	// main pass writes to the colour buffer which we extract from in the post processing pass 
	// and this colour buffer we pass stores the outputs of all the fragement shaders in the colour buffer 

	// define the projection matrix to be use 
	mainPass.camera.projection = glm::ortho(0.f, m_screenWidth, m_screenHeight, 0.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	mainPass.setCachedValue("b_lightPassCamera", "u_lightPassview", mainPass.camera.view);
	mainPass.setCachedValue("b_lightPassCamera", "u_lightPassProjection", mainPass.camera.projection);



	mainPass.setCachedValue("b_camera", "u_view", m_renderer.getRenderPass(m_deferredPrePasIdx).camera.view);

	mainPass.setCachedValue("b_camera", "u_projection", glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f));

	mainPass.setCachedValue("b_lights", "u_viewPos", m_scene->m_actors.at(m_cameraIdx).translation);
	mainPass.setCachedValue("b_lights", "dLight.colour", m_lightPassScene->m_directionalLights.at(0).colour);
	mainPass.setCachedValue("b_lights", "dLight.direction", m_lightPassScene->m_directionalLights.at(0).direction);
	//// attaching the camera script to the actor 
	m_scene->m_actors.at(m_cameraIdx).attachScript<CameraScript>(m_renderer.getRenderPass(m_deferredPrePasIdx).scene->m_actors.at(m_cameraIdx), m_winRef, glm::vec3(5.0f, 5.0f, 5.0f), 1.0f);
	// add main initial pass with all the actors we want the main lighting to impact
	addPointLightDataToPass(mainPass, PointLightNum);
	m_mainPassIdx = m_renderer.getPassCount();
	m_renderer.addRenderPass(mainPass);
	



	
	

	
	for (int i = 0; i < m_normalVisualisationScene->m_actors.size(); i++) {
		m_normalVisualisationScene->m_actors[i].material = normalVisMat;
		m_normalVisualisationScene->m_actors[i].material->setValue("u_lightPassDepth", deferredPrePass.target->getTarget(4));

	}
	
	RenderPass normalVisPass; 
	normalVisPass.scene = m_normalVisualisationScene;
	normalVisPass.parseScene();
	normalVisPass.target = std::make_shared<FBO>(m_winRef.getSize(), TypicalLayout);
	normalVisPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	normalVisPass.viewPort = ViewPort{ 0,0,m_winRef.getWidth(),m_winRef.getHeight() };
	
	normalVisPass.setCachedValue("b_camera", "u_projection", deferredPrePass.camera.projection);
	normalVisPass.setCachedValue("b_camera", "u_view", deferredPrePass.camera.view);

	m_normalVisualIdx = m_renderer.getPassCount();
	m_renderer.addRenderPass(normalVisPass);


	ShaderDescription normalOverlayShaderDesc;
	normalOverlayShaderDesc.type = ShaderType::rasterization;
	normalOverlayShaderDesc.vertexSrcPath = "./assets/shaders/normalOverlayVert.glsl";
	normalOverlayShaderDesc.fragmentSrcPath = "./assets/shaders/normalOverlayFrag.glsl";



	std::shared_ptr<Shader> normalOverlayShader = std::make_shared<Shader>(normalOverlayShaderDesc); 

	m_normalOverlayMat = std::make_shared<Material>(normalOverlayShader);
	m_normalOverlayMat->setValue("u_sceneCol", m_renderer.getRenderPass(m_mainPassIdx).target->getTarget(0));
	m_normalOverlayMat->setValue("u_normalSceneCol", normalVisPass.target->getTarget(0));
	m_normalOverlayMat->setValue("u_active", m_normalOverlayOn);

	makePosProcessScreenPass(m_normalOverlayMat, m_normalOverlayScene, ScreenQuadVAO, m_normalOverlayIdx, TypicalLayout);


	ShaderDescription colourInverseShaderDesc; 
	colourInverseShaderDesc.type = ShaderType::rasterization; 
	colourInverseShaderDesc.vertexSrcPath = "./assets/shaders/PostProcessingVert.glsl";
	colourInverseShaderDesc.fragmentSrcPath = "./assets/shaders/colourInverseFrag.glsl";

	std::shared_ptr<Shader> colourInverseShader = std::make_shared<Shader>(colourInverseShaderDesc);

	m_invertColourMat = std::make_shared<Material>(colourInverseShader);
	m_invertColourMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_normalOverlayIdx).target->getTarget(0));
    
	makePosProcessScreenPass(m_invertColourMat, m_colourInversionScene, ScreenQuadVAO, m_colourInversionIdx, TypicalLayout);

	// relative luminance shader
	ShaderDescription relativeLuminanceShaderDesc;
	relativeLuminanceShaderDesc.type = ShaderType::rasterization;
	relativeLuminanceShaderDesc.vertexSrcPath = "./assets/shaders/PostProcessingVert.glsl";
	relativeLuminanceShaderDesc.fragmentSrcPath = "./assets/shaders/relativeLuminanceFrag.glsl";

	std::shared_ptr<Shader> relativeLuminanceShader = std::make_shared<Shader>(relativeLuminanceShaderDesc);

	m_luminanceMat = std::make_shared<Material>(relativeLuminanceShader);
	m_luminanceMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_colourInversionIdx).target->getTarget(0));
	m_luminanceMat->setValue("u_tint", m_tintColour);



	makePosProcessScreenPass(m_luminanceMat, m_relativeLuminanceTintScene, ScreenQuadVAO, m_relativeLumianceTintIdx, TypicalLayout);
	



	// initialse quad for blur post processing 
	ShaderDescription blurShaderDesc;
	blurShaderDesc.type = ShaderType::rasterization;
	blurShaderDesc.vertexSrcPath = "./assets/shaders/PostProcessingVert.glsl";
	blurShaderDesc.fragmentSrcPath = "./assets/shaders/blurFrag.glsl";


	std::shared_ptr<Shader> blurShader = std::make_shared<Shader>(blurShaderDesc);
	m_blurMat = std::make_shared<Material>(blurShader);


	m_blurMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_relativeLumianceTintIdx).target->getTarget(0));
	m_blurMat->setValue("u_blurRadius", m_blurRadius);
	m_blurMat->setValue("u_imageSize", m_winRef.getSizef());
	makePosProcessScreenPass(m_blurMat, m_blurScene, ScreenQuadVAO, m_blurPassIdx, TypicalLayout);

	

	// initialse quad for edge detection post processing 
	ShaderDescription edgeDetectionShaderDesc;
	edgeDetectionShaderDesc.type = ShaderType::rasterization;
	edgeDetectionShaderDesc.vertexSrcPath = "./assets/shaders/PostProcessingVert.glsl";
	edgeDetectionShaderDesc.fragmentSrcPath = "./assets/shaders/edgeDetectionFrag.glsl";


	std::shared_ptr<Shader> edgeDetectionShader = std::make_shared<Shader>(edgeDetectionShaderDesc);
	m_edgeDetectionMat = std::make_shared<Material>(edgeDetectionShader);
	m_edgeDetectionMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_blurPassIdx).target->getTarget(0));
	
	makePosProcessScreenPass(m_edgeDetectionMat, m_edgeDetectionScreenScene, ScreenQuadVAO, m_edgeDetectionIdx, TypicalLayout);


	ShaderDescription fogShaderDesc;
	fogShaderDesc.type = ShaderType::rasterization;
	fogShaderDesc.vertexSrcPath = "./assets/shaders/PostProcessingVert.glsl";
	fogShaderDesc.fragmentSrcPath = "./assets/shaders/fogFrag.glsl";

	std::shared_ptr<Shader> fogShader = std::make_shared<Shader>(fogShaderDesc);
	m_fogMat = std::make_shared<Material>(fogShader);
	m_fogMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_edgeDetectionIdx).target->getTarget(0));
	m_fogMat->setValue("u_depthTexture", deferredPrePass.target->getTarget(4));
	m_fogMat->setValue("u_fogColour", m_fogColour); 
	m_fogMat->setValue("u_farClip", m_fogFar);
	m_fogMat->setValue("u_nearClip", m_nearClippingPlane);
	makePosProcessScreenPass(m_fogMat, m_fogScreenScene, ScreenQuadVAO, m_fogPassIdx, TypicalLayout);

	// initialse quad for luminance contrast post processing 
	ShaderDescription luminanceContrastShaderDesc;
	luminanceContrastShaderDesc.type = ShaderType::rasterization;
	luminanceContrastShaderDesc.vertexSrcPath = "./assets/shaders/PostProcessingVert.glsl";
	luminanceContrastShaderDesc.fragmentSrcPath = "./assets/shaders/luminanceContrastFrag.glsl";

	std::shared_ptr<Shader> luminanceContrastShader = std::make_shared<Shader>(luminanceContrastShaderDesc);
	m_luminanceContrastMat = std::make_shared<Material>(luminanceContrastShader);
	m_luminanceContrastMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_fogPassIdx).target->getTarget(0));
	m_luminanceContrastMat->setValue("u_contrast", m_LuminanceContrastScalar);
	// pass for contrast using luminance
	makePosProcessScreenPass(m_luminanceContrastMat, m_contrastScreenScene, ScreenQuadVAO, m_luminanceContrastIdx, TypicalLayout);



	// initialse quad for luminance saturation post processing 
	ShaderDescription luminanceSaturationShaderDesc;
	luminanceSaturationShaderDesc.type = ShaderType::rasterization;
	luminanceSaturationShaderDesc.vertexSrcPath = "./assets/shaders/PostProcessingVert.glsl";
	luminanceSaturationShaderDesc.fragmentSrcPath = "./assets/shaders/luminanceSaturationFrag.glsl";


	std::shared_ptr<Shader> luminanceSaturationShader = std::make_shared<Shader>(luminanceSaturationShaderDesc);
	m_luminanceSaturationMat = std::make_shared<Material>(luminanceSaturationShader);
	
	m_luminanceSaturationMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_luminanceContrastIdx).target->getTarget(0));
	m_luminanceSaturationMat->setValue("u_saturation", m_LuminanceSaturationScalar);
	// pass for saturation using luminance

	makePosProcessScreenPass(m_luminanceSaturationMat, m_saturationScreenScene, ScreenQuadVAO, m_luminanceSaturationIdx,TypicalLayout);

	//m_renderer.addRenderPass(luminanceSaturationPass);
	

	// setting up shader to visualise depth
	ShaderDescription visualiseDepthShaderDesc;
	visualiseDepthShaderDesc.type = ShaderType::rasterization;
	visualiseDepthShaderDesc.vertexSrcPath = "./assets/shaders/visualiseDepthVert.glsl";
	visualiseDepthShaderDesc.fragmentSrcPath = "./assets/shaders/visualiseDepthFrag.glsl";

	std::shared_ptr<Shader> visualiseDepthShader = std::make_shared<Shader>(visualiseDepthShaderDesc); 

	m_visualiseDepthMat = std::make_shared<Material>(visualiseDepthShader); 
	m_visualiseDepthMat->setValue("u_depthBufferTexture", deferredPrePass.target->getTarget(4));
	m_visualiseDepthMat->setValue("u_nearClip", m_nearClippingPlane);
	m_visualiseDepthMat->setValue("u_farClip",m_farClippingPlane);
	
	// visualise depth pass

    makePosProcessScreenPass(m_visualiseDepthMat, m_visualiseDepthScreenScene, ScreenQuadVAO, m_linDepthPassIdx,TypicalLayout);
	
	

	ShaderDescription shadowMapOuputShaderDesc;
	shadowMapOuputShaderDesc.type = ShaderType::rasterization;
	shadowMapOuputShaderDesc.vertexSrcPath = "./assets/shaders/shadowMapVisualisationVert.glsl";
	shadowMapOuputShaderDesc.fragmentSrcPath = "./assets/shaders/shadowMapVisualiseFrag.glsl";


	std::shared_ptr<Shader> shadowMapVisualShader = std::make_shared<Shader>(shadowMapOuputShaderDesc);
	std::shared_ptr<Material> shadowMapVisMat = std::make_shared<Material>(shadowMapVisualShader);

	shadowMapVisMat->setValue("u_depthBufferTexture", shadowMapPrePass.target->getTarget(0));
	shadowMapVisMat->setValue("u_nearClip", -(m_shadowMapVariables.orthoSize/5.0f));
	shadowMapVisMat->setValue("u_farClip", m_shadowMapVariables.orthoSize * 5.0f);

	Actor visualiseShadowDepthQuad;
	visualiseShadowDepthQuad.geometry = ScreenQuadVAO;
	visualiseShadowDepthQuad.material = shadowMapVisMat ;
	m_shadowPrePassVisualScreenScene->m_actors.push_back(visualiseShadowDepthQuad);

	RenderPass visualiseShadowPreDepthPass;

	visualiseShadowPreDepthPass.scene = m_shadowPrePassVisualScreenScene;
	visualiseShadowPreDepthPass.parseScene();
	visualiseShadowPreDepthPass.target = std::make_shared<FBO>(m_winRef.getSize(), TypicalLayout);
	visualiseShadowPreDepthPass.camera.projection = glm::ortho(0.f, m_screenWidth, m_screenHeight, 0.f);

	visualiseShadowPreDepthPass.viewPort = ViewPort{ 0,0,m_winRef.getWidth(),m_winRef.getHeight() };
	visualiseShadowPreDepthPass.setCachedValue("b_shadowMapVisualisation", "u_view", visualiseShadowPreDepthPass.camera.view);
	visualiseShadowPreDepthPass.setCachedValue("b_shadowMapVisualisation", "u_projection", visualiseShadowPreDepthPass.camera.projection);
	
	m_shadowMapVisualisationIdx = m_renderer.getPassCount();
	m_renderer.addRenderPass(visualiseShadowPreDepthPass);
	
	// defining gamma correction shader
	ShaderDescription gammaAndToneMapDes;
	gammaAndToneMapDes.type = ShaderType::rasterization;
	gammaAndToneMapDes.vertexSrcPath = "./assets/shaders/GammaCorrectionVert.glsl";
	gammaAndToneMapDes.fragmentSrcPath = "./assets/shaders/GammaCorrectionFrag.glsl";

	std::shared_ptr<Shader> gammaCorrectionShader = std::make_shared<Shader>(gammaAndToneMapDes);

	std::shared_ptr<Material> gammaCorrectionMat = std::make_shared<Material>(gammaCorrectionShader, "u_model");

	gammaCorrectionMat->setValue("u_colourBufferTexture", m_renderer.getRenderPass(m_luminanceSaturationIdx).target->getTarget(0));

	Actor screenQuad;
	screenQuad.geometry = ScreenQuadVAO;
	screenQuad.material = gammaCorrectionMat;
	m_finalResult->m_actors.push_back(screenQuad);
	// gamma correction
	RenderPass GammaPass;
	GammaPass.scene = m_finalResult;
	GammaPass.parseScene();
	GammaPass.target = std::make_shared<FBO>();
	GammaPass.camera.projection = glm::ortho(0.f, m_screenWidth, m_screenHeight, 0.f);
	GammaPass.viewPort = ViewPort{ 0,0,m_winRef.getWidth(),m_winRef.getHeight() };

	GammaPass.setCachedValue("b_camera2D", "u_view", GammaPass.camera.view);
	GammaPass.setCachedValue("b_camera2D", "u_projection", GammaPass.camera.projection);

	m_renderer.addRenderPass(GammaPass);
	


	m_PostProcessingNames = {
		"ColourInversionPostPass",
		"RelativeLuminanceTintPass",
		"blur",
		"edgeDetection",
		"fog",
		"RelativeLuminanceContrastPass",
		"RelativeLuminanceSaturationPass", 
	};

	m_postProcessingMaterials = {
		m_invertColourMat,
		m_luminanceMat,
		m_blurMat,
		m_edgeDetectionMat,
		m_fogMat,
		m_luminanceContrastMat,
		m_luminanceSaturationMat,
	};

	SetUpPostProcessingFlags();
	makeComputePasses();
	m_particleInit.render();



}




void MainLayer::onRender() const
{
	m_computeRenderer.render();
	m_renderer.render();
	

}

void MainLayer::onUpdate(float timestep)
{
	// Update scripts
	for (auto& actor : m_scene->m_actors)
	{
		actor.onUpdate(timestep);
	}
	currentTime = glfwGetTime();
	dt = currentTime - lastFrameTime;
	lastFrameTime = currentTime;
	/*if(dt > 0.15f)
	{
		dt = 0.15f;
	}*/
	m_computeRenderer.getComputePass(m_updatePaticlesIdx).material->setValue("dt",dt);
	// Update camera  and its position in UBO
	auto& camera = m_scene->m_actors.at(m_cameraIdx);

	RenderPass& defferedPass = m_renderer.getRenderPass(m_deferredPrePasIdx);

	defferedPass.camera.updateView(camera.transform);
	defferedPass.setCachedValue("b_camera", "u_view", defferedPass.camera.view);
	defferedPass.setCachedValue("b_camera", "u_viewPos", camera.translation);

	/*RenderPass& forwardPass = m_renderer.getRenderPass(m_forwardPassIdx);
	forwardPass.setCachedValue("b_camera", "u_view", defferedPass.camera.view);
	forwardPass.setCachedValue("b_camera", "u_viewPos", camera.translation);*/


	auto& pass = m_renderer.getRenderPass(m_mainPassIdx);

	
	pass.setCachedValue("b_camera", "u_view", defferedPass.camera.view);
	pass.setCachedValue("b_camera", "u_viewPos", camera.translation); 
	m_lightPassScene->m_directionalLights.at(0).direction = glm::normalize(m_dirLightDirection);
	pass.setCachedValue("b_lights", "dLight.direction", m_lightPassScene->m_directionalLights.at(0).direction);



	// ensure that skybox wont move with view matrix 
	m_skyboxScene->m_actors.at(m_skyBoxIdx).material->setValue("u_skyBoxView", glm::mat4(glm::mat3(defferedPass.camera.view)));

	glm::vec3 newLightPos = (m_shadowMapVariables.center - m_dirLightDirection) * m_shadowMapVariables.distanceAlongLightVec;
	glm::mat4 newLightSpaceMat = glm::lookAt(newLightPos, m_shadowMapVariables.center, m_shadowMapVariables.up);
	DepthPass& shadowMapPass = m_renderer.getDepthPass(m_shadowMapPrepassIdx);
	shadowMapPass.setCachedValue("b_lightCamera","u_view",newLightSpaceMat);


	RenderPass &normalVis = m_renderer.getRenderPass(m_normalVisualIdx);
	normalVis.setCachedValue("b_camera", "u_view", defferedPass.camera.view);
	m_phongModelMaterial->setValue("u_lightSpaceMatrix", shadowMapPass.camera.projection * newLightSpaceMat );




}

void MainLayer::onImGUIRender()
{




	// the entire ui is defined within a single frame 
  ImGui::Begin("Demo");
	// here we create a check box within the frame making it render for this particualr frame and checking that it has been 
	//created giving it a name  also passing it the boolean for whether or not the option within the check box is defined 
	if (ImGui::Checkbox("Wireframe ", &m_wireFrame)) {
		auto& defferedPass = m_renderer.getRenderPass(m_deferredPrePasIdx);
		if (m_wireFrame) {
			// prepass is before we apply the shader 
			defferedPass.prePassActions.clear();
			defferedPass.postPassActions.clear();
			// set up the wire frame rendereing for the Pass
			defferedPass.prePassActions.emplace_back([]() {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 	glDisable(GL_BLEND); });
			defferedPass.postPassActions.emplace_back([]() {glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); glEnable(GL_BLEND); });
		}
		else {
			// otherwise we draw the sceene as usual without wireframe applied 
			defferedPass.prePassActions.clear();
			defferedPass.prePassActions.emplace_back([]() {glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); glDisable(GL_BLEND); });
			

		}
	} 
	
	// end the frame
  ImGui::End();
  
  ImGui::Begin("Before Post Process And Gamma/Tone");
	GLuint textureId = m_renderer.getRenderPass(m_mainPassIdx).target->getTarget(0)->getID();
	// deifne size for imgui image
	ImVec2 imageSize = ImVec2(512, 512);
	// deifne uvs for the image
	ImVec2 UvTop = ImVec2(0.0f, 1.0f);
	ImVec2 UvBottom = ImVec2(1.0f, 0.0f);
	ImGui::Image((void*)(intptr_t)textureId, imageSize, UvTop, UvBottom);

  ImGui::End();





  ImGui::Begin("PostProcessingFlags");
	
	 for (int i = 0; i < m_postProcessingFlags.size();i++) {
		const char* name = m_PostProcessingNames[size_t(i)].c_str();
		//std::printf("itteration of flags %d  \n", i);
		if (ImGui::Checkbox(name, (bool*)(m_postProcessingFlags.data() + i))) {

		    
			m_postProcessingMaterials[i]->setValue("u_active", m_postProcessingFlags[i]);


		}
		


	 }

  ImGui::End();





  ImGui::SetNextWindowSize({ 512,512 });

  ImGui::Begin("PostProcessingProperties");

	if (ImGui::ColorPicker3("Tint for luminance pass", &m_tintColour.x)) {

		m_luminanceMat->setValue("u_tint", m_tintColour);


	 }
	
	if (ImGui::SliderFloat("Luminance Saturation", &m_LuminanceSaturationScalar, 0.0f, 1.0f)) {

		m_luminanceSaturationMat->setValue("u_saturation", m_LuminanceSaturationScalar);


	} 

	if (ImGui::SliderFloat("Luminance Contrast", &m_LuminanceContrastScalar, 0.0f, 3.0f)) {

		m_luminanceContrastMat->setValue("u_contrast", m_LuminanceContrastScalar);


	}

	if (ImGui::SliderInt("Blur Radius", &m_blurRadius, 1, 10)) {

		m_blurMat->setValue("u_blurRadius", m_blurRadius);


	}

	
  ImGui::End();





  ImVec2 ImageSize = ImVec2(512, 512);

  ImGui::Begin("generic settings");
    
  if (ImGui::BeginTabBar("shadow settings")) {
	  if (ImGui::BeginTabItem("shadowMapVisual")) { 
		  ImGui::SliderFloat3("light direction", &m_dirLightDirection.x, -1.0f, 1.0f);
		  GLuint shadowDepthTextureId = m_renderer.getRenderPass(m_shadowMapVisualisationIdx).target->getTarget(0)->getID();
		  ImGui::Image((void*)(intptr_t)shadowDepthTextureId, ImageSize, UvTop, UvBottom);

		  ImGui::EndTabItem();
	  }

	  if (ImGui::BeginTabItem("shadowProperties")) {


		  if (ImGui::Checkbox("shadow anti aliasing", (bool*)&m_shadowAntiAliasingOn)) {

			  m_phongModelMaterial->setValue("u_antiAliasingOn", m_shadowAntiAliasingOn);

		  }
		  ImGui::EndTabItem();
	  }

	  if (ImGui::BeginTabItem("normals")) {


		  if (ImGui::Checkbox("visualise normals", (bool*)&m_normalOverlayOn)) {

			  m_normalOverlayMat->setValue("u_active", m_normalOverlayOn);

		  }
		  ImGui::EndTabItem();
	  }

	  if (ImGui::BeginTabItem("Tessellation")) {


		  if (ImGui::SliderFloat("terrain height scalar", &m_terrainHeightScalar, 2.0f, 50.0f)) 
		  {
			  m_gPassDiffuseOnly->setValue("u_heightScalar", m_terrainHeightScalar);
		  
		  }

		  if (ImGui::Checkbox("enable cdm normals ", (bool*)&m_shouldUseCDMNormals)) {

			  m_gPassDiffuseOnly->setValue("u_shouldUseCDM", m_shouldUseCDMNormals);
		  
		  }


		  if (ImGui::Checkbox("show terrain height colour ", (bool*)&m_useTerrainHeightColour)) {

			  m_gPassDiffuseOnly->setValue("u_heightColActive", m_useTerrainHeightColour);

		  }
		


		  
		  ImGui::EndTabItem();
	  }
	  ImGui::EndTabBar();
  }
 
  ImGui::End();
  
  ImGui::Begin("compute heightMap");
  
   GLuint id = m_computeRenderer.getComputePass(m_heightMapComputeIdx).images[0].getID();

   ImGui::Image((void*)(intptr_t)id, imageSize, UvTop, UvBottom);
   


  ImGui::End();


  ImGui::Begin("deferred rendering data");

  GLuint vertexPositionsId = m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(0)->getID();

  ImGui::Image((void*)(intptr_t)vertexPositionsId, ImageSize, UvTop, UvBottom);

  ImGui::SameLine();
  GLuint normalsId = m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(1)->getID();

  ImGui::Image((void*)(intptr_t)normalsId, ImageSize, UvTop, UvBottom);

  GLuint diffSpecId = m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(2)->getID();

  ImGui::Image((void*)(intptr_t)diffSpecId, ImageSize, UvTop, UvBottom);
  
  ImGui::SameLine();
  GLuint linDepthTextureId = m_renderer.getRenderPass(m_linDepthPassIdx).target->getTarget(0)->getID();
  


  ImGui::Image((void*)(intptr_t)linDepthTextureId, ImageSize, UvTop, UvBottom);



  ImGui::End();



  ImGui::Begin("Terrain settings");

  if (ImGui::SliderFloat("Frequency", &m_terrainFreq, 0.0f, 5.0f)) 
  {
     
	  m_terrainHeightMat->setValue("u_frequency", m_terrainFreq);
  }

  if (ImGui::SliderFloat("Amplitude", &m_terrainAmp, 0.0f, 5.0f))
  {

	  m_terrainHeightMat->setValue("u_amplitude", m_terrainAmp);
  }

  if (ImGui::SliderFloat("lacrunarity", &m_terrainLacrunarity, 0.0f, 5.0f))
  {

	  m_terrainHeightMat->setValue("u_lacrunarity", m_terrainLacrunarity);
  }
  if (ImGui::SliderFloat("persistance", &m_terrainPersistance, 0.0f, 5.0f))
  {

	  m_terrainHeightMat->setValue("u_persistance", m_terrainPersistance);
  }
  if (ImGui::SliderInt("octaves", &m_terrainGenOctaves, 1,5))
  {

	  m_terrainHeightMat->setValue("u_octaves", m_terrainGenOctaves);
  }
  if (ImGui::Checkbox("FBM", (bool*)&m_useFBMNoise)) {

	  m_terrainHeightMat->setValue("u_FBM", m_useFBMNoise);

  }
  if (ImGui::Checkbox("ridged noise", (bool*)&m_useRidgedNoise)) {

	  m_terrainHeightMat->setValue("u_useRidgedNoise", m_useRidgedNoise);

  }
  if (ImGui::Checkbox("turbulent noise", (bool*)&m_useTurbulentNoise)) {

	  m_terrainHeightMat->setValue("u_turbulentNoise", m_useTurbulentNoise);

  }
  if (ImGui::Checkbox("combination", (bool*)&m_useCombinedNoise)) {

	  m_terrainHeightMat->setValue("u_combination", m_useCombinedNoise);

  }
  
  ImGui::End();
  

   

}


void MainLayer::createActors(int num ,float coordRangeMin, float coordRangeMax, std::shared_ptr< VAO> Vao, std::shared_ptr< Material> mat)
{

	for (int i = 0; i < num; i++) {
		Actor Object;
		Object.geometry = Vao;
		Object.material = mat;

		Object.translation = glm::vec3(Randomiser::uniformFloatBetween(coordRangeMin,coordRangeMax), -3.0f, Randomiser::uniformFloatBetween(coordRangeMin, coordRangeMax));

		Object.recalc();
		m_scene->m_actors.push_back(Object);
	}
	



}

void MainLayer::createActors(int num, float coordRangeMin, float coordRangeMax, std::shared_ptr<VAO> Vao, std::shared_ptr<VAO> depthVAO, std::shared_ptr<Material> mat, std::shared_ptr<Material> depthMat)
{

	for (int i = 0; i < num; i++) {
		Actor Object;
		Object.geometry = Vao;
		Object.material = mat;
		Object.depthGeometry = depthVAO;
		Object.depthMaterial = depthMat;
		Object.translation = glm::vec3(Randomiser::uniformFloatBetween(coordRangeMin, coordRangeMax), -3.0f, Randomiser::uniformFloatBetween(coordRangeMin, coordRangeMax));
		
		Object.recalc();
		m_scene->m_actors.push_back(Object);
	}


}

void MainLayer::createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat, std::shared_ptr<VAO> depthVao, std::shared_ptr<Material> depthMat)
{

	Actor Object;
	Object.geometry = Vao;
	Object.material = mat;
	Object.depthGeometry = depthVao;
	Object.depthMaterial = depthMat;
	Object.translation = initialPos;
	
	Object.recalc();
	m_scene->m_actors.push_back(Object);



}

void MainLayer::createActor(glm::vec3 initialPos, std::shared_ptr< VAO> Vao, std::shared_ptr< Material> mat)
{


	Actor Object;
	Object.geometry = Vao;
	Object.material = mat;

	Object.translation = initialPos;
	
	Object.recalc();
	m_scene->m_actors.push_back(Object);



}

void MainLayer::createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat, size_t& outId)
{

	Actor Object;
	Object.geometry = Vao;
	Object.material = mat;

	Object.translation = initialPos;
	
	Object.recalc(); 
	outId = m_scene->m_actors.size();
	m_scene->m_actors.push_back(Object);


}

void MainLayer::createActor(glm::vec3 initialPos, std::shared_ptr<VAO> Vao, std::shared_ptr<Material> mat, size_t& outId, std::shared_ptr<Scene>& Scene)
{

	Actor Object;
	Object.geometry = Vao;
	Object.material = mat;

	Object.translation = initialPos;

	Object.recalc();
	outId = Scene->m_actors.size();
	Scene->m_actors.push_back(Object);

}

void MainLayer::generateBilboards(std::vector<float>& positions, std::shared_ptr<VAO> vao, std::shared_ptr<Material> material)
{
	Actor bilboard;

	for (int i = 0; i <= positions.size() - 3; i+=3) {

		bilboard.geometry = vao;
		bilboard.material = material;
		bilboard.depthMaterial = m_shadowPrePassMat;
		bilboard.recalc();
		m_scene->m_actors.push_back(bilboard);


	}
	

	std::vector<uint32_t>moonIndices = { 0 };
	std::vector<float> moonboardPositions = { 0.0f,30.0f,-50.0f };

	ShaderDescription moonBilboardShaderDesc;
	moonBilboardShaderDesc.type = ShaderType::geometry;
	moonBilboardShaderDesc.vertexSrcPath = "./assets/shaders/bilboardVert.glsl";
	moonBilboardShaderDesc.geometrySrcPath = "./assets/shaders/bilboardGeo.glsl";
	moonBilboardShaderDesc.fragmentSrcPath = "./assets/shaders/moonBilboardFrag.glsl";



	std::shared_ptr<Shader> moonBilBoardShader = std::make_shared<Shader>(moonBilboardShaderDesc);
	std::shared_ptr<Texture> moonBilboardTexture = std::make_shared<Texture>("./assets/textures/moon.png");
	std::shared_ptr<Material> moonBilboardMat = std::make_shared<Material>(moonBilBoardShader);

	moonBilboardMat->setPrimitive(GL_POINTS);
	moonBilboardMat->setValue("u_scale", m_bilboardScale);
	moonBilboardMat->setValue("u_bilBoardMoonTexture", moonBilboardTexture);

	std::shared_ptr<VAO> moonBilboardVAO = std::make_shared<VAO>(moonIndices);

	moonBilboardVAO->addVertexBuffer(moonboardPositions, { {GL_FLOAT,3} });


	Actor moonBilboard;
	moonBilboard.geometry = moonBilboardVAO;
	moonBilboard.material = moonBilboardMat;
	moonBilboard.depthMaterial = m_shadowPrePassMat;
	moonBilboard.recalc();
	m_scene->m_actors.push_back(moonBilboard);
}


void MainLayer::addPointLight(glm::vec3 colour,glm::vec3 position, glm::vec3 attenuation )
{
	PointLight pointLight; 
	pointLight.position = position;
	pointLight.colour = colour;
	pointLight.constants = attenuation;
	m_lightPassScene->m_pointLights.push_back(pointLight);

}

void MainLayer::makePosProcessScreenPass(std::shared_ptr<Material> mat, std::shared_ptr<Scene> scene, std::shared_ptr<VAO> screenVAO, FBOLayout frameBufLayout )
{
	Actor screenQuad;
	screenQuad.geometry = screenVAO;
	screenQuad.material = mat;
	scene->m_actors.push_back(screenQuad);
	// gamma correction
	RenderPass Pass;
	Pass.scene = scene;
	Pass.parseScene();
	Pass.target = std::make_shared<FBO>(m_winRef.getSizef(), frameBufLayout);
	Pass.camera.projection = glm::ortho(0.f, m_screenWidth, m_screenHeight, 0.f);
	Pass.viewPort = ViewPort{ 0,0,m_winRef.getWidth(),m_winRef.getHeight() };

	Pass.setCachedValue("b_camera2D", "u_view", Pass.camera.view);
	Pass.setCachedValue("b_camera2D", "u_projection", Pass.camera.projection);

	m_renderer.addRenderPass(Pass);

}

void MainLayer::makePosProcessScreenPass(std::shared_ptr<Material> mat, std::shared_ptr<Scene> scene, std::shared_ptr<VAO> screenVAO, size_t& idx, FBOLayout frameBufLayout )
{
	Actor screenQuad;
	screenQuad.geometry = screenVAO;
	screenQuad.material = mat;
	scene->m_actors.push_back(screenQuad);
	// gamma correction
	RenderPass Pass;
	Pass.scene = scene;
	Pass.parseScene();
	Pass.target = frameBufLayout.begin()._Ptr ? Pass.target = std::make_shared<FBO>(m_winRef.getSizef(), frameBufLayout) : Pass.target = std::make_shared<FBO>();

	Pass.camera.projection = glm::ortho(0.f, m_screenWidth, m_screenHeight, 0.f);
	Pass.viewPort = ViewPort{ 0,0,m_winRef.getWidth(),m_winRef.getHeight() };

	Pass.setCachedValue("b_camera2D", "u_view", Pass.camera.view);
	Pass.setCachedValue("b_camera2D", "u_projection", Pass.camera.projection);
	idx = m_renderer.getPassCount();
	m_renderer.addRenderPass(Pass);


}

void MainLayer::addPointLights(int PointLightNum)
{
	glm::vec3 attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
	for (int i = 0; i < PointLightNum; i++) {

		glm::vec3 pointLightColour  = glm::vec3(Randomiser::uniformFloatBetween(0.0, 0.90f), Randomiser::uniformFloatBetween(0.0, 0.90f), Randomiser::uniformFloatBetween(0.0, 0.90f));
		glm::vec3 position = glm::vec3(Randomiser::uniformFloatBetween(-30.0f, 30.0f), -1.0f, Randomiser::uniformFloatBetween(-30.0f, 30.0f));
		addPointLight(pointLightColour, position, attenuation);

	}


}

ShaderDescription MainLayer::makeFragmentShaderDesc(std::filesystem::path vertPath, std::filesystem::path fragPath)
{

	ShaderDescription shaderDesc;
	shaderDesc.type = ShaderType::rasterization;
	shaderDesc.vertexSrcPath = vertPath;
	shaderDesc.fragmentSrcPath = fragPath;

	return shaderDesc;


}

void MainLayer::initLightPass(std::shared_ptr<VAO> screenQuad, FBOLayout lightPassLayout)
{


	m_phongModelMaterial->setValue("u_shadowMap", m_renderer.getDepthPass(m_shadowMapPrepassIdx).target->getTarget(0));
	m_phongModelMaterial->setValue("u_antiAliasingOn", m_shadowAntiAliasingOn);
	m_phongModelMaterial->setValue("u_fragmentPositions", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(0));
	m_phongModelMaterial->setValue("u_normalMap", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(1));
	m_phongModelMaterial->setValue("u_diffSpecMap", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(2));
	m_phongModelMaterial->setValue("u_prePassDepthTexture", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(4));
	m_phongModelMaterial->setValue("u_skyBoxColBuffer", m_renderer.getRenderPass(m_skyBoxPassIdx).target->getTarget(0));
	m_phongModelMaterial->setValue("u_fragmentId", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(3));
	//m_phongModelMaterial->setValue("u_forwardDepth", m_renderer.getRenderPass(m_forwardPassIdx).target->getTarget(1));
	//m_phongModelMaterial->setValue("u_forwardCol", m_renderer.getRenderPass(m_forwardPassIdx).target->getTarget(0));

	//m_phongModelMaterial->setValue("u_fpd", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(0));
	//m_phongModelMaterial->setValue("u_forwardPassColBuf", m_renderer.getRenderPass(m_deferredPrePasIdx).target->getTarget(1));
	Actor lightPassQuad;
	lightPassQuad.geometry = screenQuad;
	lightPassQuad.material = m_phongModelMaterial;

	m_lightPassScene->m_actors.push_back(lightPassQuad);




	// initialise a particualr pass for the renderer to perfrom rendering is often sperated inot particualr passes 
	// as certain operations need to be performed in particualr order 
	RenderPass mainPass;
	// assigng the scene for the pass to manipulate 
	mainPass.scene = m_lightPassScene;
	// extract all the neccessary information from all the actors we defined in the scene to be used by the shader pass(generally things that define 
	// the look or surface of the actors like materials)
	mainPass.parseScene();
	//mainPass.target = std::make_shared<FBO>();
	// in process of adding post processing
	mainPass.target = std::make_shared<FBO>(m_winRef.getSize(), lightPassLayout); // Default framebuffer

	// main pass writes to the colour buffer which we extract from in the post processing pass 
	// and this colour buffer we pass stores the outputs of all the fragement shaders in the colour buffer 

	// define the projection matrix to be use 
	mainPass.camera.projection = glm::ortho(0.f, m_screenWidth, m_screenHeight, 0.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	mainPass.setCachedValue("b_lightPassCamera", "u_lightPassview", mainPass.camera.view);
	mainPass.setCachedValue("b_lightPassCamera", "u_lightPassProjection", mainPass.camera.projection);



	mainPass.setCachedValue("b_camera", "u_view", m_renderer.getRenderPass(m_deferredPrePasIdx).camera.view);

	mainPass.setCachedValue("b_camera", "u_projection", glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f));

	mainPass.setCachedValue("b_lights", "u_viewPos", m_scene->m_actors.at(m_cameraIdx).translation);
	mainPass.setCachedValue("b_lights", "dLight.colour", m_lightPassScene->m_directionalLights.at(0).colour);
	mainPass.setCachedValue("b_lights", "dLight.direction", m_lightPassScene->m_directionalLights.at(0).direction);
	//// attaching the camera script to the actor 
	m_scene->m_actors.at(m_cameraIdx).attachScript<CameraScript>(m_renderer.getRenderPass(m_deferredPrePasIdx).scene->m_actors.at(m_cameraIdx), m_winRef, glm::vec3(5.0f, 5.0f, 5.0f), 1.0f);
	// add main initial pass with all the actors we want the main lighting to impact
	addPointLightDataToPass(mainPass, PointLightNum);
	m_mainPassIdx = m_renderer.getPassCount();
	m_renderer.addRenderPass(mainPass);
}

void MainLayer::addPointLightDataToPass(RenderPass& pass, int PointLightNum)
{


	for (int i = 0; i < PointLightNum; i++) {
		std::printf("adding point light\n ");
		pass.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_lightPassScene->m_pointLights[i].colour);
		pass.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_lightPassScene->m_pointLights[i].position);
		pass.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_lightPassScene->m_pointLights[i].constants);

	}



}

void MainLayer::makePaticleEmitter(glm::vec3 origin, std::shared_ptr<Material> particleMat,std::shared_ptr<Scene> scene,std::shared_ptr<Texture> texture,std::shared_ptr<VAO> vao,float particleBilboardScale)
{

	Actor Emitter;
	Emitter.translation = origin;
	Emitter.recalc();
	//Emitter.geometry = vao;
	Emitter.material = particleMat;
	Emitter.material->setValue("u_particleTexture", texture);
	Emitter.material->setValue("u_particleScale", particleBilboardScale);

	scene->m_actors.push_back(Emitter);

	 

  
	


}


void MainLayer::SetUpPostProcessingFlags()
{


	

	for (int i = 0; i < m_postProcessingMaterials.size(); i++) {

		m_postProcessingFlags.emplace_back(0);
		
		m_postProcessingMaterials[i]->setValue("u_active", 0);
	}


}

void MainLayer::makeForwardParticlePass(FBOLayout layout)
{

	//RenderPass forwardParticlePass;
	//forwardParticlePass.scene = m_forwardPassScene;
	//forwardParticlePass.parseScene();
	//forwardParticlePass.target = std::make_shared<FBO>(m_winRef.getSize(), layout);
	//forwardParticlePass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	//forwardParticlePass.camera.projection = glm::perspective(45.0f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.0f);
	//forwardParticlePass.camera.updateView(m_scene->m_actors.at(m_cameraIdx).transform);
	//forwardParticlePass.setCachedValue("b_camera", "u_view", forwardParticlePass.camera.view);
	//forwardParticlePass.setCachedValue("b_camera", "u_projection", forwardParticlePass.camera.projection);
	//forwardParticlePass.setCachedValue("b_camera", "u_viewPos", m_scene->m_actors.at(m_cameraIdx).translation);

	// = m_renderer.getPassCount();
	//m_renderer.addRenderPass(forwardParticlePass);
}

void MainLayer::makeComputePasses()
{
	// describing image that can be written to by the compute shader
	TextureDescription computeTextureDesc;

	computeTextureDesc.width = 512;
	computeTextureDesc.height = 512;
	computeTextureDesc.channels = 4;
	// ensure sampling is always limited to the bounds of the texture
	computeTextureDesc.wrapMethod = GL_CLAMP_TO_EDGE;
	computeTextureDesc.type = TextureDataType::HDR;
	std::shared_ptr<Texture> computeTex = std::make_shared<Texture>(computeTextureDesc);


	ShaderDescription computeHeightMap;
	computeHeightMap.computeSrcPath = "./assets/shaders/heightMapCompute.glsl";
	computeHeightMap.type = ShaderType::compute;

	std::shared_ptr<Shader> computeHeightMapShader = std::make_shared<Shader>(computeHeightMap);

	m_terrainHeightMat = std::make_shared<Material>(computeHeightMapShader);
	ComputePass heightMapCompute;
	m_gPassDiffuseOnly->setValue("u_cdmNormalMap", computeTex);
	m_terrainHeightMat->setValue("u_lacrunarity", m_terrainLacrunarity);
	m_terrainHeightMat->setValue("u_persistance", m_terrainPersistance);
	m_terrainHeightMat->setValue("u_frequency", m_terrainFreq);
	m_terrainHeightMat->setValue("u_amplitude", m_terrainAmp);
	m_terrainHeightMat->setValue("u_octaves", m_terrainGenOctaves);
	m_terrainHeightMat->setValue("u_heightMapSize", glm::vec2(0.0f, 0.0f));
	m_terrainHeightMat->setValue("u_useRidgedNoise", m_useRidgedNoise);
	m_terrainHeightMat->setValue("u_FBM", m_useFBMNoise);
	m_terrainHeightMat->setValue("u_turbulentNoise", m_useTurbulentNoise);

	heightMapCompute.barrier = MemoryBarrier::ShaderImageAccess;
	heightMapCompute.material = m_terrainHeightMat;
	heightMapCompute.workgroups = { 32,32,1 };

	std::shared_ptr<Texture> heightMapTex = std::make_shared<Texture>(computeTextureDesc);


	ImageDescWithTexture computeHeightMapDesc;
	computeHeightMapDesc.texture = heightMapTex;
	computeHeightMapDesc.imageUnit = heightMapCompute.material->m_shader->m_imageBindingPoints["outputImage"];
	computeHeightMapDesc.access = TextureAccess::ReadWrite;
	Image heightImg = Image(computeHeightMapDesc);
	heightMapCompute.images.push_back(heightImg);

	m_heightMapComputeIdx = m_computeRenderer.getPassCount();

	m_computeRenderer.addComputePass(heightMapCompute);

	// compute shader for cdm normals
	ShaderDescription computeDesc = ShaderDescription();
	computeDesc.type = ShaderType::compute;
	computeDesc.computeSrcPath = "./assets/shaders/computeNormalsCDM.glsl";

	std::shared_ptr<Shader> computeNormals = std::make_shared<Shader>(computeDesc);

	std::shared_ptr<Material> computeMat = std::make_shared<Material>(computeNormals);



	// defining the compute pass for central difference normals
	ComputePass CDNormCompute;
	CDNormCompute.material = computeMat;
	CDNormCompute.workgroups = { 32,32,1 };
	CDNormCompute.barrier = MemoryBarrier::ShaderImageAccess;



	ImageDescWithTexture computeImageDesc;
	computeImageDesc.texture = computeTex;
	computeImageDesc.imageUnit = CDNormCompute.material->m_shader->m_imageBindingPoints["outputImg"];
	computeImageDesc.access = TextureAccess::ReadWrite;
	Image img = Image(computeImageDesc);
	CDNormCompute.images.push_back(img);

	glm::vec2 heightMapSize = glm::vec2(512.0f, 512.0f);
	CDNormCompute.material->setValue("u_heightMap", heightMapTex);
	CDNormCompute.material->setValue("u_heightScalar", m_terrainHeightScalar);
	m_computeRenderer.addComputePass(CDNormCompute);

}

void MainLayer::makePaticleComputePasses()
{

	uint32_t particleBuffSize = (sizeof(particle) * m_particleNum);
	m_initParticleSSBO = std::make_shared<SSBO>(particleBuffSize, m_particleNum);
	m_initParticleSSBO->bind(0);

	ShaderDescription particleInitShaderDesc;
	particleInitShaderDesc.type = ShaderType::compute;
	particleInitShaderDesc.computeSrcPath = "./assets/shaders/computeParticleInit.glsl";
	std::shared_ptr<Shader> particleInitShader = std::make_shared<Shader>(particleInitShaderDesc);

	std::shared_ptr<Material> particleInitMat = std::make_shared<Material>(particleInitShader);
	particleInitMat->setValue("u_particleOrigin", glm::vec3(-3.0f, -3.0f, -11.0f));

	ComputePass initParticles;
	initParticles.barrier = MemoryBarrier::ShaderStorageAccess;
	initParticles.material = particleInitMat;
	initParticles.ssbo = m_initParticleSSBO;

	initParticles.workgroups = { 32,32,1 };

	m_particleInit.addComputePass(initParticles);
	ShaderDescription particleUpdateShaderDesc;
	particleUpdateShaderDesc.type = ShaderType::compute;
	particleUpdateShaderDesc.computeSrcPath = "./assets/shaders/computeParticleUpdate.glsl";
	std::shared_ptr<Shader> particleUpdateShader = std::make_shared<Shader>(particleUpdateShaderDesc);

	std::shared_ptr<Material> particleUpdateMat = std::make_shared<Material>(particleUpdateShader);
	particleUpdateMat->setValue("u_particleAcceleration", m_particleAccel);
	ComputePass updateParticles;
	updateParticles.barrier = MemoryBarrier::ShaderStorageAccess;
	updateParticles.material = particleUpdateMat;
	updateParticles.ssbo = m_initParticleSSBO;
	updateParticles.workgroups = { 32,32,1 };

	m_updatePaticlesIdx = m_computeRenderer.getPassCount();
	m_computeRenderer.addComputePass(updateParticles);
	

}




