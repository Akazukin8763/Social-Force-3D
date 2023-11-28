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
    m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, "Final Project", NULL, NULL);
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
    m_shadowMapper.Setup();

    m_socialForce = SocialForce();

    //m_pedestrianModel = Model("./Models/Hibiki/Hibiki.obj");
    m_pedestrianModel = Model("./Models/sphere/sphere.obj");
    m_pedestrianModel.SetModelMatrix(glm::translate(Matrix4::identity, Vector3::up * 0.5f));
    m_pedestrianModel.SetShadow(true);

    m_borderModel = Model("./Models/cube/cube.obj");
    m_borderModel.SetModelMatrix(glm::translate(Matrix4::identity, Vector3::up * 0.5f));
    m_borderModel.SetShadow(true);

    Model tempGroundModel = Model("./Models/cube/cube.obj");
    tempGroundModel.SetModelMatrix(Matrix4::identity);
    tempGroundModel.UpdateInstanceDatas(std::vector<InstanceData> { InstanceData { glm::scale(Matrix4::identity, glm::vec3(40, 0.1, 20)) } });
    tempGroundModel.SetShadow(true);

    m_plane = Plane(25);

    glm::mat4 projection, view, model;
    std::vector<Model*> ptr_models { &m_pedestrianModel, &m_borderModel, &tempGroundModel };

    // Main Loop
    while (!glfwWindowShouldClose(m_window)) {
        glClearColor(BACKGROUND_COLOR);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //IsMouseOnGui = io.WantCaptureMouse;

        glEnable(GL_DEPTH_TEST); // Enable depth test

        glEnable(GL_BLEND); // Enable blending mode
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glCullFace(GL_BACK); // Culling back buffer
        glEnable(GL_CULL_FACE);

        // Timer
        m_currentTime = static_cast<float>(glfwGetTime());
        m_deltaTime = m_currentTime - m_lastTime;
        m_lastTime = m_currentTime;

        if (m_simulateFlag)
            m_socialForce.Simulate(m_deltaTime); // Simulate
        m_pedestrianModel.UpdateInstanceDatas(m_socialForce.GetPedestrianInstances());
        m_borderModel.UpdateInstanceDatas(m_socialForce.GetBorderInstances());

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
        // Shadow
        if (m_directionLightShadowFlag) m_shadowMapper.RenderDirectionLightShadowMap(m_directionLightDirection, ptr_models);

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

        // Setup Shadow Map
        if (m_directionLightShadowFlag) {
            std::vector<glm::mat4> lightMatrices = m_shadowMapper.GetDirectionLightSpaceMatrix();
            std::vector<GLuint> shadowMaps = m_shadowMapper.GetDirectionLightShadowMap();

            m_mainShader.SetInt("DirectionLightSpaceMatrixNums", CASCADES_NUMS);
            m_mainShader.SetInt("DirectionLightShadowMapNums", CASCADES_NUMS);
            for (int i = 0; i < CASCADES_NUMS; i++) {
                std::string index = std::to_string(i);
                m_mainShader.SetMat4("DirectionLightSpaceMatrix[" + index + "]", lightMatrices[i]);
                m_mainShader.SetInt("DirectionLightShadowMap[" + index + "]", shadowMaps[i]);
            }

            for (Model* model : ptr_models)
                model->UpdateShadowMaps(shadowMaps);
        }
        
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