#include "demo.hpp"
#include "camera.hpp"

DemoLayer::DemoLayer(GLFWWindowImpl& win) : Layer(win)
{
	ShaderDescription phongShaderDesc;
	phongShaderDesc.type = ShaderType::rasterization;
	phongShaderDesc.vertexSrcPath = "./assets/shaders/Phong/Vert.glsl";
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/Phong/Frag.glsl";

	std::shared_ptr<Shader> phongShader;
	phongShader = std::make_shared<Shader>(phongShaderDesc);

	Model cubeModel("./assets/models/whitecube/whitecube.obj");

	VBOLayout cubeLayout = {
		{GL_FLOAT, 3}, // Position
		{GL_FLOAT, 3}, // Normal
		{GL_FLOAT, 2}  // UV co-ords
	};

	std::shared_ptr<VAO> cubeVAO;
	cubeVAO = std::make_shared<VAO>(cubeModel.m_meshes[0].indices);
	cubeVAO->addVertexBuffer(cubeModel.m_meshes[0].vertices, cubeLayout);

	std::shared_ptr<Texture> cubeTexture;
	cubeTexture = std::make_shared<Texture>(cubeModel.m_meshes[0].texturePaths[aiTextureType_DIFFUSE].string().c_str());

	std::shared_ptr<Material> cubeMaterial;
	cubeMaterial = std::make_shared<Material>(phongShader, "u_model");
	cubeMaterial->setValue("u_albedo", glm::vec3(1.0f));
	cubeMaterial->setValue("u_albedoMap", cubeTexture);

	m_scene.reset(new Scene);

	Actor cube;
	cube.geometry = cubeVAO;
	cube.material = cubeMaterial;

	cube.translation = glm::vec3(0.f, -1.f, -6.f);
	cube.rotation = glm::quat(glm::vec3(0.0f, 0.4f, 0.0f));
	cube.recalc();
	m_scene->m_actors.push_back(cube);

	DirectionalLight dl;
	dl.direction = glm::normalize(glm::vec3(1.f, -2.5f, -2.f));
	m_scene->m_directionalLights.push_back(dl);

	Actor camera;
	m_cameraIdx = m_scene->m_actors.size();
	m_scene->m_actors.push_back(camera);

	RenderPass mainPass;

	mainPass.scene = m_scene;
	mainPass.parseScene();
	mainPass.target = std::make_shared<FBO>(); // Default framebuffer
	mainPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 100.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	mainPass.camera.updateView(m_scene->m_actors.at(m_cameraIdx).transform);

	mainPass.setCachedValue("b_camera", "u_view", mainPass.camera.view);
	mainPass.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);

	mainPass.setCachedValue("b_lights", "u_viewPos", m_scene->m_actors.at(m_cameraIdx).translation);
	mainPass.setCachedValue("b_lights", "dLight.colour", m_scene->m_directionalLights.at(0).colour);
	mainPass.setCachedValue("b_lights", "dLight.direction", m_scene->m_directionalLights.at(0).direction);

	m_renderer.addRenderPass(mainPass);

	m_scene->m_actors.at(m_cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(m_cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f), 0.5f);
}


void DemoLayer::onRender() const
{
	m_renderer.render();
}

void DemoLayer::onUpdate(float timestep)
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
}

void DemoLayer::onImGUIRender()
{

	ImGui::Begin("Demo");
	if (ImGui::Checkbox("Wireframe ", &m_wireFrame)) {
		auto& mainPass = m_renderer.getRenderPass(0);
		if (m_wireFrame) {
			mainPass.prePassActions.clear();
			mainPass.prePassActions.emplace_back([]() {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);});
		}
		else {
			mainPass.prePassActions.clear();
			mainPass.prePassActions.emplace_back([]() {glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);});
		}
	}
	ImGui::End();
	
}
