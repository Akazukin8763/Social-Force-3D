#include "Application.h"


Application::Application(const int width, const int height) {
	m_window = nullptr;

	m_screenWidth = width;
	m_screenHeight = height;

	//m_camera = Camera(Vector3::zero);
	m_camera = Camera(glm::vec3(0, 10, 10));
	m_camera.UpdateProjectionMatrix(static_cast<float>(width) / height);

	m_mainShader = m_planeShader = Shader();
	m_shadowMapper = ShadowMapper(m_screenWidth, m_screenHeight,
		m_shadowMapWidth, m_shadowMapHeight,
		m_camera.GetNearPlane(), m_camera.GetFarPlane());

	m_currentTime = m_lastTime = m_deltaTime = 0;
}

bool Application::Setup() {
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, "Social Force 3D", NULL, NULL);
	if (m_window == NULL) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return false;
	}

	// Configure Callback Function
	GLFWCallbackWrapper::SetApplication(this);
	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, GLFWCallbackWrapper::OnReshape);
	glfwSetMouseButtonCallback(m_window, GLFWCallbackWrapper::OnMouse);
	glfwSetCursorPosCallback(m_window, GLFWCallbackWrapper::OnMouseMovement);
	glfwSetScrollCallback(m_window, GLFWCallbackWrapper::OnMouseScroll);
	glfwSetKeyCallback(m_window, GLFWCallbackWrapper::OnKeyBoard);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLAD and load function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return false;
	}

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	return true;
}

void Application::Rendering() {
	m_mainShader = Shader("./Shaders/main.vs", "./Shaders/main.fs");
	m_planeShader = Shader("./Shaders/plane.vs", "./Shaders/plane.fs");

	m_skyboxShader = Shader("./Shaders/skybox.vs", "./Shaders/skybox.fs");

	m_shadowMapper.Setup();

	m_socialForce = SocialForce(MAX_INSTANCES);

	// Pedestrain
	m_pedestrianModel = Model("./Models/pedestrian/idle.dae");
	m_pedestrianModel.SetAnimation(true);
	m_pedestrianModel.SetModelMatrix(glm::scale(Matrix4::identity, glm::vec3(0.2f, 0.2f, 0.2f)));
	m_pedestrianModel.SetShadow(true);

	Animation idleAnimation("./Models/pedestrian/idle.dae", &m_pedestrianModel);
	Animation walkAnimation("./Models/pedestrian/walk.dae", &m_pedestrianModel);
	Animation runAnimation("./Models/pedestrian/run.dae", &m_pedestrianModel);

	PedestrianAnimator animators(MAX_INSTANCES, idleAnimation, walkAnimation, runAnimation);
	m_pedestrianModel.UpdateInstanceBonesMatrices(animators.GetFinalBoneMatrices());

	// Border
	m_borderModel = Model("./Models/cube/cube.obj");
	m_borderModel.SetAnimation(false);
	m_borderModel.SetModelMatrix(glm::translate(Matrix4::identity, Vector3::up * 0.5f));
	m_borderModel.SetShadow(true);

	// Ground
	Model tempGroundModel = Model("./Models/cube/road.obj");
	tempGroundModel.SetAnimation(false);
	tempGroundModel.SetModelMatrix(Matrix4::identity);
	tempGroundModel.SetShadow(true);

	// House
	Model tempHouseModel = Model("./Models/house/building.obj");
	tempHouseModel.SetAnimation(false);
	tempHouseModel.SetModelMatrix(Matrix4::identity);
	tempHouseModel.SetShadow(true);

	m_plane = Plane(100);

	glm::mat4 projection, view, model;
	std::vector<Model*> ptr_models { &m_pedestrianModel, &tempGroundModel, &tempHouseModel };

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	std::vector<std::string> faces
	{
		"./Models/skybox/right.jpg",
		"./Models/skybox/left.jpg",
		"./Models/skybox/top.jpg",
		"./Models/skybox/bottom.jpg",
		"./Models/skybox/front.jpg",
		"./Models/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	m_skyboxShader.Activate();
	m_skyboxShader.SetInt("skybox", 0);

	int socialForceType = CIRCULAR_SPECIFICATION;

	// Main Loop
	while (!glfwWindowShouldClose(m_window)) {
		glClearColor(BACKGROUND_COLOR);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glEnable(GL_DEPTH_TEST); // Enable depth test

		glEnable(GL_BLEND); // Enable blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glCullFace(GL_BACK); // Culling back buffer
		glEnable(GL_CULL_FACE);

		// Timer
		m_currentTime = static_cast<float>(glfwGetTime());
		m_deltaTime = m_currentTime - m_lastTime;
		m_lastTime = m_currentTime;

		if (m_simulateFlag) {
			// Simulate social force
			m_socialForce.SetPedestrianRepulsiveForceType(socialForceType);

			m_socialForce.Simulate(m_deltaTime);

			animators.UpdateStates(m_socialForce.GetPedestrianInstanceStates());
			animators.UpdateAnimations(m_deltaTime);
			m_pedestrianModel.UpdateInstanceBonesMatrices(animators.GetFinalBoneMatrices());

			std::vector<glm::mat4> modelMatrices = animators.GetModelMatrices();
			std::vector<glm::mat4> transformMatrices = m_socialForce.GetPedestrianInstanceTransforms();
			for (int i = 0; i < m_pedestrianNums; i++)
				transformMatrices[i] = transformMatrices[i] * modelMatrices[i];
			m_pedestrianModel.UpdateInstanceTransforms(transformMatrices);
		}
		else {
			m_pedestrianModel.UpdateInstanceTransforms(m_socialForce.GetPedestrianInstanceTransforms());
		}
		m_borderModel.UpdateInstanceTransforms(m_socialForce.GetBorderInstanceTransforms());

		// Matrix
		projection = m_camera.GetProjectionMatrix();
		view = m_camera.GetViewMatrix();
		model = Matrix4::identity;

		/*
		 *  Camera Setting
		 */
		m_camera.HandleMovement(m_cameraTranslate * m_deltaTime * 5.0f);

		/*
		 *  Model Settings
		 */
		if (m_directionLightShadowFlag) // it contains shader inside, so do not change this line's order
			m_shadowMapper.RenderDirectionLightShadowMap(m_directionLightDirection, ptr_models);

		// Shader Settings
		m_mainShader.Activate(); // Activate Shader

		m_mainShader.SetVec3("viewPos", m_camera.GetPosition());

		m_mainShader.SetBool("dirLight.enable", true);
		m_mainShader.SetBool("dirLight.enableShadow", m_directionLightShadowFlag);
		m_mainShader.SetVec3("dirLight.direction", m_directionLightDirection);
		m_mainShader.SetVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		m_mainShader.SetVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		m_mainShader.SetVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		m_mainShader.SetMat4("Projection", projection);
		m_mainShader.SetMat4("ViewMatrix", view);
		//m_mainShader.SetMat4("ModelMatrix", Matrix4::identity); // Global Coordinate

		// Setup Shadow Maps
		if (m_directionLightShadowFlag) {
			std::vector<glm::mat4> lightMatrices = m_shadowMapper.GetDirectionLightSpaceMatrices();
			std::vector<GLuint> shadowMaps = m_shadowMapper.GetDirectionLightShadowMaps();
			int lightNums = m_shadowMapper.GetDirectionLightNums();

			m_mainShader.SetInt("DirectionLightNums", lightNums);
			for (int i = 0; i < lightNums; i++) {
				std::string index = std::to_string(i);
				m_mainShader.SetMat4("DirectionLightSpaceMatrices[" + index + "]", lightMatrices[i]);
				m_mainShader.SetInt("DirectionLightShadowMaps[" + index + "]", shadowMaps[i]);
			}

			for (Model* model : ptr_models)
				model->UpdateShadowMaps(shadowMaps);
		}


		xFactor = 5.7f / 10.f * halfGroundSize;
		yFactor = 5.7f / 10.f * halfGroundSize;
		zFactor = 5.7f / 10.f * halfGroundSize;

		/*
		 *  更新地面和建築物
		 */ 
		int chunk = 5;

		// Ground
		std::vector<glm::mat4> groundTransforms = std::vector<glm::mat4>(chunk * chunk);

		glm::mat4 groundTransform = glm::scale(Matrix4::identity, glm::vec3(halfGroundSize, 0.05f, halfGroundSize));

		for (int i = 0; i < chunk * chunk; i++)
			groundTransforms[i] = glm::translate(
				Matrix4::identity, 
				glm::vec3(halfGroundSize * 2 * (i % chunk - chunk / 2), 
						 0.05f, 
						 halfGroundSize * 2 * (i / chunk - chunk / 2))
			) * groundTransform;
		tempGroundModel.UpdateInstanceTransforms(groundTransforms);

		// Building
		std::vector<glm::mat4> buildingTransforms = std::vector<glm::mat4>(chunk * chunk * 4);
		glm::mat4 buildingScale = glm::scale(Matrix4::identity, glm::vec3(xFactor, yFactor, zFactor));

		glm::mat4 buildingTransforms1 = glm::translate(Matrix4::identity, glm::vec3((halfGroundSize - x * xFactor), 0 - y * yFactor, (halfGroundSize - z * zFactor)));
		glm::mat4 buildingTransforms2 = glm::translate(Matrix4::identity, glm::vec3((halfGroundSize - x * xFactor), 0 - y * yFactor, -(halfGroundSize - z * zFactor)));
		glm::mat4 buildingTransforms3 = glm::translate(Matrix4::identity, glm::vec3(-(halfGroundSize - x * xFactor - halfGroundSize * 40 / 70), 0 - y * yFactor, (halfGroundSize - z * zFactor)));
		glm::mat4 buildingTransforms4 = glm::translate(Matrix4::identity, glm::vec3(-(halfGroundSize - x * xFactor - halfGroundSize * 40 / 70), 0 - y * yFactor, -(halfGroundSize - z * zFactor)));

		for (int i = 0; i < chunk * chunk; i++) {
			glm::mat4 offset = glm::translate(
				Matrix4::identity, 
				glm::vec3(halfGroundSize * 2 * (i % chunk - chunk / 2), 
						  0.05f, 
						  halfGroundSize * 2 * (i / chunk - chunk / 2))
			);

			buildingTransforms[i * 4] = buildingTransforms1 * offset * buildingScale;
			buildingTransforms[i * 4 + 1] = buildingTransforms2 * offset * buildingScale;
			buildingTransforms[i * 4 + 2] = buildingTransforms3 * offset * buildingScale;
			buildingTransforms[i * 4 + 3] = buildingTransforms4 * offset * buildingScale;
		}

		tempHouseModel.UpdateInstanceTransforms(buildingTransforms);

		// Render	
		for (Model* model : ptr_models) {
			m_mainShader.SetMat4("ModelMatrix", model->GetModelMatrix()); // Local Coordinate
			model->Render(m_mainShader);
		}

		/*
		 *  Plane Settings
		 */
		m_planeShader.Activate(); // Activate Shader

		m_planeShader.SetMat4("Projection", projection);
		m_planeShader.SetMat4("ViewMatrix", view);
		m_planeShader.SetMat4("ModelMatrix", Matrix4::identity); // Global Coordinate

		m_plane.Render(m_planeShader); // Render

		 /*
		 *  draw skybox as last
		 */
		 glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		 m_skyboxShader.Activate();
		 view = glm::mat4(glm::mat3(m_camera.GetViewMatrix())); // remove translation from the view matrix
		 m_skyboxShader.SetMat4("view", view);
		 m_skyboxShader.SetMat4("projection", projection);
		 // skybox cube
		 glBindVertexArray(skyboxVAO);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		 glDrawArrays(GL_TRIANGLES, 0, 36);
		 glBindVertexArray(0);
		 glDepthFunc(GL_LESS); // set depth function back to default



		 /*
		  *  ImGui Settings
		  */
		if (m_toggleGUI) {
			ImGui::Begin("Settings");

			// FPS 
			m_smoothedFPS = m_smoothedFPS + m_smoothingFactor * (1.0f / m_deltaTime - m_smoothedFPS);
			ImGui::Text("FPS: %.1f", m_smoothedFPS);

			ImGui::Separator();

			// Simulate
			ImGui::Text("Simulate"); ImGui::SameLine();
			if (ImGui::Button(m_simulateFlag ? "Stop " : "Start"))
				m_simulateFlag = !m_simulateFlag;

			ImGui::Separator();

			// Weight
			if (ImGui::Checkbox("Weight", &m_weightFlag)) {
				m_socialForce.EnableWeight(m_weightFlag);
			}

			// Collision
			if (ImGui::Checkbox("Collision", &m_collisionFlag)) {
				m_socialForce.EnableCollision(m_collisionFlag);
			}

			ImGui::Separator();

			// Light
			if (ImGui::SliderFloat3("Direction Light", &m_directionLightDirection.x, -1, 1)) {
				if (m_directionLightDirection == Vector3::zero)
					m_directionLightDirection = Vector3::down;
				m_directionLightDirection = glm::normalize(m_directionLightDirection);
			}
			ImGui::Checkbox("Direction Light Shadow", &m_directionLightShadowFlag);

			ImGui::Separator();

			// Numbers of Pedestrian
			if (ImGui::SliderInt("Pedestrians Numbers", &m_pedestrianNums, 0, MAX_INSTANCES)) {
				m_socialForce.UpdatePedestrianNums(m_pedestrianNums);
			}

			ImGui::RadioButton("CIRCULAR_SPECIFICATION", &socialForceType, CIRCULAR_SPECIFICATION);
			ImGui::RadioButton("ELLIPTICAL_SPECIFICATION_I", &socialForceType, ELLIPTICAL_SPECIFICATION_I);
			ImGui::RadioButton("ELLIPTICAL_SPECIFICATION_II", &socialForceType, ELLIPTICAL_SPECIFICATION_II);
			ImGui::RadioButton("NEW_ELLIPTICAL_SPECIFICTION", &socialForceType, NEW_ELLIPTICAL_SPECIFICTION);
			ImGui::Separator();

			if (ImGui::Button("Reset")) {
				SFMGlobal::ca_k = CA_K;
				SFMGlobal::ca_kappa = CA_KAPPA;

				SFMGlobal::cs_a = CS_A;
				SFMGlobal::cs_b = CS_B;
				SFMGlobal::cs_lambda = CS_LAMBDA;

				SFMGlobal::es1_tau = ES1_TAU;
				SFMGlobal::es1_a = ES1_A;
				SFMGlobal::es1_b = ES1_B;
				SFMGlobal::es1_lambda = ES1_LAMBDA;

				SFMGlobal::es2_tau = ES2_TAU;
				SFMGlobal::es2_a = ES2_A;
				SFMGlobal::es2_b = ES2_B;
				SFMGlobal::es2_lambda = ES2_LAMBDA;

				SFMGlobal::nes_tau;
				SFMGlobal::nes_a = NES_A;
				SFMGlobal::nes_b = NES_B;
				SFMGlobal::nes_lambda = NES_LAMBDA;

				SFMGlobal::br_a = BR_A;
				SFMGlobal::br_b = BR_B;
			}

			switch (socialForceType) {
			case CIRCULAR_SPECIFICATION:
				ImGui::Text("CIRCULAR_SPECIFICATION");
				ImGui::SliderFloat("cs_a", &SFMGlobal::cs_a, CS_A / 100.f, CS_A * 10000.f);
				ImGui::SliderFloat("cs_b", &SFMGlobal::cs_b, CS_B / 100.f, CS_B * 10000.f);
				ImGui::SliderFloat("cs_lambda", &SFMGlobal::cs_lambda, CS_LAMBDA / 100.f, CS_LAMBDA * 10000.f);
				break;
			case ELLIPTICAL_SPECIFICATION_I:
				ImGui::Text("ELLIPTICAL_SPECIFICATION_I");
				ImGui::SliderFloat("es1_tau", &SFMGlobal::es1_tau, ES1_TAU / 100.f, ES1_TAU * 10000.f);
				ImGui::SliderFloat("es1_a", &SFMGlobal::es1_a, ES1_A / 100.f, ES1_A * 10000.f);
				ImGui::SliderFloat("es1_b", &SFMGlobal::es1_b, ES1_B / 100.f, ES1_B * 10000.f);
				ImGui::SliderFloat("es1_lambda", &SFMGlobal::es1_lambda, ES1_LAMBDA / 100.f, ES1_LAMBDA * 10000.f);
				break;
			case ELLIPTICAL_SPECIFICATION_II:
				ImGui::Text("ELLIPTICAL_SPECIFICATION_II");
				ImGui::SliderFloat("es2_tau", &SFMGlobal::es2_tau, ES2_TAU / 100.f, ES2_TAU * 10000.f);
				ImGui::SliderFloat("es2_a", &SFMGlobal::es2_a, ES2_A / 100.f, ES2_A * 10000.f);
				ImGui::SliderFloat("es2_b", &SFMGlobal::es2_b, ES2_B / 100.f, ES2_B * 10000.f);
				ImGui::SliderFloat("es2_lambda", &SFMGlobal::es2_lambda, ES2_LAMBDA / 100.f, ES2_LAMBDA * 10000.f);
				break;
			case NEW_ELLIPTICAL_SPECIFICTION:
				ImGui::Text("NEW_ELLIPTICAL_SPECIFICTION");
				ImGui::SliderFloat("nes_tau", &SFMGlobal::nes_tau, NES_TAU / 100.f, NES_TAU * 10000.f);
				ImGui::SliderFloat("nes_a", &SFMGlobal::nes_a, NES_A / 100.f, NES_A * 10000.f);
				ImGui::SliderFloat("nes_b", &SFMGlobal::nes_b, NES_B / 100.f, NES_B * 10000.f);
				ImGui::SliderFloat("nes_lambda", &SFMGlobal::nes_lambda, NES_LAMBDA / 100.f, NES_LAMBDA * 10000.f);
				break;
			default:
				break;
			}
			ImGui::Separator();
			ImGui::Text("Border");
			ImGui::SliderFloat("br_a", &SFMGlobal::br_a, BR_A / 100.f, BR_A * 10.f);
			ImGui::SliderFloat("br_b", &SFMGlobal::br_b, BR_B / 100.f, BR_B * 100.f);
			//ImGui::Separator();
			//ImGui::SliderFloat("houseX", &x, -5, 5);
			//ImGui::SliderFloat("houseY", &y, -5, 5);
			//ImGui::SliderFloat("houseZ", &z, -5, 5);
			//ImGui::SliderFloat("houseXScale", &xFactor, -100, 100);
			//ImGui::SliderFloat("houseYScale", &yFactor, -100, 100);
			//ImGui::SliderFloat("houseZScale", &zFactor, -100, 100);

			//ImGui::SliderFloat("groundScale", &halfGroundSize, -50, 50);

			ImGui::End();
		}

		ImGui::Render(); // Render ImGui
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/*
		 *  Swap two buffers(front, back) and poll all IO events(keyboard, mouse...)
		 */
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void Application::ClearResource() {
	// Clear all resources and close window
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Application::Execute() {
	if (!Setup())
		return;

	Rendering();

	ClearResource();
}

