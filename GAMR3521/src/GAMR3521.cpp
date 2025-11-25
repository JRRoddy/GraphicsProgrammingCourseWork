#include "GAMR3521.hpp"
#include "camera.hpp"

MainLayer::MainLayer(GLFWWindowImpl& win) : Layer(win)
{

	// here this essentially refreshes the scene and assings a new refernce to the smart pointer holding the scene 
	// the scene has buffers that hold all of the current data for the scene such as lighting and objects 
	m_scene.reset(new Scene);

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


	// defining descriptor for floor shader
	ShaderDescription BaseFloorShaderDesc; 
	BaseFloorShaderDesc.type = ShaderType::rasterization; 
	BaseFloorShaderDesc.vertexSrcPath = "./assets/shaders/BaseVertexShader.glsl";
	BaseFloorShaderDesc.fragmentSrcPath = "./assets/shaders/BaseFragementShader.glsl";

	std::shared_ptr<Shader> FloorShader; 
	FloorShader = std::make_shared<Shader>(BaseFloorShaderDesc);

	// extracting vertex data from grid 
	std::shared_ptr<Grid> floorGrid = std::make_shared<Grid>(); 
	std::vector<float>  floorGridVertecies = floorGrid->getVertices(); 
	std::vector<unsigned int> FloorElementIndicies = floorGrid->getIndices(); 

	// defining layout for floor vertex data
	VBOLayout FloorLayout{
		{GL_FLOAT,3}, 
	    {GL_FLOAT,2},
	};

	// defining floor vertex data 
	std::shared_ptr<VAO> FloorGridVAO; 

	FloorGridVAO = std::make_shared<VAO>(FloorElementIndicies);

	FloorGridVAO->addVertexBuffer(floorGridVertecies, FloorLayout);
	
	// creating floor texture
	std::shared_ptr<Texture> FloorTexture;
	FloorTexture = std::make_shared<Texture>("./assets/textures/floorTex.png");


	// defining floor material
	std::shared_ptr<Material> FloorMaterial; 
	FloorMaterial = std::make_shared<Material>(FloorShader, "u_model");

	FloorMaterial->setValue("u_albedo", glm::vec3(1.0f));
	FloorMaterial->setValue("u_albedoMap", FloorTexture); 
	// initialsiing the floor actor using the previously defined data 
	Actor FloorActor;
	FloorActor.geometry = FloorGridVAO;
	FloorActor.material = FloorMaterial;
	// define the translation 
	FloorActor.translation = glm::vec3{ -50.0f,-5.0f,-50.0f };
	FloorActor.recalc(); 
	m_FloorIdx = m_scene->m_actors.size();
	m_scene->m_actors.push_back(FloorActor);

	// create the cube model 
	Model cubeModel("./assets/models/whitecube/whitecube.obj");
	// defining the layout of the data that will be allocated on the GPU 
	VBOLayout cubeLayout = {
		{GL_FLOAT, 3}, // Position
		{GL_FLOAT, 3}, // Normal
		{GL_FLOAT, 2},  // UV co-ords
	    {GL_FLOAT,3}   //tangent
	};

	

	// creare the buffer that will store the vertex data that is described by the layout we defined above for the cube VAO 
	std::shared_ptr<VAO> cubeVAO;
	cubeVAO = std::make_shared<VAO>(cubeModel.m_meshes[0].indices); 
	// defining the vertex data for the VBO using the mesh data at index 0 because the cube model only has one mesh
	// also passing in the layout of the data to be allocated on the GPU 
	cubeVAO->addVertexBuffer(cubeModel.m_meshes[0].vertices, cubeLayout);
	// create and define the diffuse texture that will be used as the base colour for the cube 
	std::shared_ptr<Texture> cubeTextureDiffuse;
	// grab the diffuse texture of the cube mesh at index 0 using the defined enmum (as an index) then  taking the cstring of the texture path we accessed 
	cubeTextureDiffuse = std::make_shared<Texture>(cubeModel.m_meshes[0].texturePaths[aiTextureType_DIFFUSE].string().c_str());

	std::shared_ptr<Texture> cubeTexture;

	// making a material out of the shader we made and setting the diffuse map to be used by the phong lighting 
	// we also set a scalar value for the diffuse colour 
	std::shared_ptr<Material> cubeMaterial;
	cubeMaterial = std::make_shared<Material>(phongShader, "u_model");
	cubeMaterial->setValue("u_albedo", glm::vec3(1.0f));
	// atttach the diffuse map we created for the cube after extracting the diffuse texture path from its texture paths 
	cubeMaterial->setValue("u_albedoMap", cubeTextureDiffuse);

	// Actor represents an object
	Actor cube;
	// set the cube geomtry to be the  defined VAO that has the vbo for the vertex data of the cube bound to it 
	cube.geometry = cubeVAO;
	// set the material of the cube to be the one defined above that is using the phong lighting shader and the texture that we extracted from the cubes model data and loaded (using the texture path)
	//cube.material = cubeMaterial;
	//  define model matrix for cube 
	cube.translation = glm::vec3(0.f, -1.f, -6.f);
	cube.rotation = glm::quat(glm::vec3(0.0f, 0.4f, 0.0f));
	cube.recalc();
	/*m_scene->m_actors.push_back(cube);*/

	VBOLayout modelLayout = {
		{GL_FLOAT, 3}, // Position
		{GL_FLOAT, 3}, // Normal
		{GL_FLOAT, 2}, // UV co-ords
	    {GL_FLOAT,3}   // tangent
	};

	uint32_t Attributes = Model::VertexFlags::positions | Model::VertexFlags::normals | 
		                  Model::VertexFlags::uvs | Model::VertexFlags::tangents;


	Model model = Model("./assets/models/Vampire/vampire.obj",Attributes);
	std::shared_ptr<VAO> ModelVAO = std::make_shared<VAO>(model.m_meshes[0].indices); 
	ModelVAO->addVertexBuffer(model.m_meshes[0].vertices, modelLayout);

	std::shared_ptr<Texture> modelDiffuseTexture = std::make_shared<Texture>("./assets/models/Vampire/textures/diffuse.png");
	std::shared_ptr<Texture> modelSpecularTexture = std::make_shared<Texture>("./assets/models/Vampire/textures/specular.png");
	std::shared_ptr<Texture> modelNormalTexture = std::make_shared<Texture>("./assets/models/Vampire/textures/normal.png");

	std::shared_ptr<Material> ModelMaterial =  std::make_shared<Material>(phongShader);

	ModelMaterial->setValue("u_albedo", glm::vec3(1.0f)); 
	ModelMaterial->setValue("u_albedoMap", modelDiffuseTexture);
	ModelMaterial->setValue("u_specularMap", modelSpecularTexture);
	ModelMaterial->setValue("u_normalMap", modelNormalTexture);



	createActor(glm::vec3(2.0f, -1.0f, -5.0f),ModelVAO,ModelMaterial);

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
	std::shared_ptr<CubeMap> skyBoxMap = std::make_shared<CubeMap>(cubeMapPaths,false);

	std::shared_ptr<Material> skyBoxMat = std::make_shared<Material>(skyBoxShader,"u_model");

	skyBoxMat->setValue("u_cubeMap", skyBoxMap);
	
	createActor(glm::vec3(0.0f, 0.0f, 0.0f), skyBoxVao, skyBoxMat, m_skyBoxIdx);

	// add  a directional light to the scene 
	DirectionalLight dl;
	dl.direction = glm::normalize(glm::vec3(1.f, -2.5f, -2.f));
	m_scene->m_directionalLights.push_back(dl);

	addPointLights(PointLightNum);

	// add a camera to the scene 
	Actor camera;
	// take the camera id using the size before pushing back to get the index
	m_cameraIdx = m_scene->m_actors.size();
	m_scene->m_actors.push_back(camera);

	// initialise a particualr pass for the renderer to perfrom rendering is often sperated inot particualr passes 
	// as certain operations need to be performed in particualr order 
	RenderPass mainPass;
	// assigng the scene for the pass to manipulate 
	mainPass.scene = m_scene;
	// extract all the neccessary information from all the actors we defined in the scene to be used by the shader pass(generally things that define 
	// the look or surface of the actors like materials)
	mainPass.parseScene();
	mainPass.target = std::make_shared<FBO>(); // Default framebuffer
	// define the projection matrix to be use 
	mainPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 500.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	mainPass.camera.updateView(m_scene->m_actors.at(m_cameraIdx).transform);

	mainPass.setCachedValue("b_camera", "u_view", mainPass.camera.view);

	mainPass.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);

	mainPass.setCachedValue("b_lights", "u_viewPos", m_scene->m_actors.at(m_cameraIdx).translation);
	mainPass.setCachedValue("b_lights", "dLight.colour", m_scene->m_directionalLights.at(0).colour);
	mainPass.setCachedValue("b_lights", "dLight.direction", m_scene->m_directionalLights.at(0).direction);
	m_scene->m_actors.at(m_cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(m_cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f), 0.5f);

	addPointLightDataToPass(mainPass,PointLightNum);

	
	//// attaching the camera script to the actor 
	//// add the render pass to be used by the renderer 
	m_renderer.addRenderPass(mainPass); 
	
}




void MainLayer::onRender() const
{
	m_renderer.render();
}

void MainLayer::onUpdate(float timestep)
{
	// Update scripts
	for (auto& actor : m_scene->m_actors)
	{
		actor.onUpdate(timestep);
	}

	// Update camera  and its position in UBO
	auto& camera = m_scene->m_actors.at(m_cameraIdx);

	auto& pass = m_renderer.getRenderPass(0);

	pass.camera.updateView(camera.transform);
	pass.setCachedValue("b_camera", "u_view", pass.camera.view);
	pass.setCachedValue("b_camera", "u_viewPos", camera.translation); 
	
	m_scene->m_actors.at(m_skyBoxIdx).material->setValue("u_skyBoxView", glm::mat4(glm::mat3(pass.camera.view)));



}

void MainLayer::onImGUIRender()
{
	// the entire ui is defined within a single frame 
	ImGui::Begin("Demo");
	// here we create a check box within the frame making it render for this particualr frame and checking that it has been 
	//created giving it a name  also passing it the boolean for whether or not the option within the check box is defined 
	if (ImGui::Checkbox("Wireframe ", &m_wireFrame)) {
		auto& mainPass = m_renderer.getRenderPass(0);
		if (m_wireFrame) {
			// prepass is before we apply the shader 
			mainPass.prePassActions.clear();
			// set up the wire frame rendereing for the Pass
			mainPass.prePassActions.emplace_back([]() {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);});
		}
		else {
			// otherwise we draw the sceene as usual without wireframe applied 
			mainPass.prePassActions.clear();
			mainPass.prePassActions.emplace_back([]() {glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);});
		}
	} 
	// creating a colour pciker that manipluates the base colour of the floor object/actor in the scene
	if (ImGui::ColorPicker3("FloorColour", &floorColour.x)) {

	   
		Actor floor = m_scene->m_actors.at(m_FloorIdx); 
		floor.material->setValue("u_albedo", floorColour);

	}


	// end the frame
	ImGui::End();
	
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


void MainLayer::addPointLight(glm::vec3 colour,glm::vec3 position, glm::vec3 attenuation )
{
	PointLight pointLight; 
	pointLight.position = position;
	pointLight.colour = colour;
	pointLight.constants = attenuation;
	m_scene->m_pointLights.push_back(pointLight);

}

void MainLayer::addPointLights(int PointLightNum)
{
	glm::vec3 attenuation = glm::vec3(1.0f, 0.7f, 0.02f);
	for (int i = 0; i < PointLightNum; i++) {

		glm::vec3 pointLightColour  = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
		//glm::vec3 pointLightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		//glm::vec3 position = glm::vec3(Randomiser::uniformFloatBetween(-20.0f, 20.0f), Randomiser::uniformFloatBetween(-2.0f, 2.0f), Randomiser::uniformFloatBetween(-10.0f, 10.0f));
		glm::vec3 position = glm::vec3(0.0f, -4.0f, 0.0f);
		addPointLight(pointLightColour, position, attenuation);

	}


}

void MainLayer::addPointLightDataToPass(RenderPass& pass, int PointLightNum)
{


	for (int i = 0; i < PointLightNum; i++) {
		std::printf("adding point light\n ");
		pass.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_scene->m_pointLights[i].colour);
		pass.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_scene->m_pointLights[i].position);
		pass.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_scene->m_pointLights[i].constants);




	}



}




