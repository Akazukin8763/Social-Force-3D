#include "Application.h"

/*
 *  Callback Wrapper
 */
Application* Application::GLFWCallbackWrapper::s_application = nullptr;

void Application::GLFWCallbackWrapper::OnReshape(GLFWwindow* window, int width, int height) {
    s_application->OnReshape(window, width, height);
}

void Application::GLFWCallbackWrapper::OnMouse(GLFWwindow* window, int button, int action, int mods) {
    s_application->OnMouse(window, button, action, mods);
}

void Application::GLFWCallbackWrapper::OnMouseMovement(GLFWwindow* window, double xpos, double ypos) {
    s_application->OnMouseMovement(window, xpos, ypos);
}

void Application::GLFWCallbackWrapper::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    s_application->OnMouseScroll(window, xoffset, yoffset);
}

void Application::GLFWCallbackWrapper::OnKeyBoard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    s_application->OnKeyBoard(window, key, scancode, action, mods);
}

void Application::GLFWCallbackWrapper::SetApplication(Application* application) {
    GLFWCallbackWrapper::s_application = application;
}

/*
 *  Application Callback Function
 */
void Application::OnReshape(GLFWwindow* window, int width, int height) {
    m_screenWidth = width;
    m_screenHeight = height;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    m_shadowMapper.UpdateScreenSize(width, height);
    m_camera.UpdateProjectionMatrix(static_cast<float>(width) / height);
}

void Application::OnMouse(GLFWwindow* window, int button, int action, int mods) {
    /*
    if (IsMouseOnGui)
        return;

    if (button == GLFW_MOUSE_BUTTON_1) {
        IsLeftButtonPress = (action == GLFW_PRESS);
        RequireUpdateMousePos = IsLeftButtonPress;
    }
    */
}

void Application::OnMouseMovement(GLFWwindow* window, double xpos, double ypos) {
    // Mouse part
    static double lastx = 0, lasty = 0, mouseMoveSpeed = 0.001f;

    if (m_toggleGUI) {
        lastx = lasty = 0;
        return;
    }

    if (lastx == 0 && lasty == 0) {
        lastx = xpos;
        lasty = ypos;
    }
    else {
        float dx = mouseMoveSpeed * static_cast<float>(xpos - lastx);
        float dy = mouseMoveSpeed * static_cast<float>(lasty - ypos);
        lastx = xpos;
        lasty = ypos;

        if (dx != 0 || dy != 0) {
            glm::quat rx(glm::angleAxis(dx, Vector3::down));
            glm::quat ry(glm::angleAxis(dy, Vector3::right));
            m_camera.HandleRotation(rx, ry);
        }
    }
}

void Application::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    /*
    if (IsMouseOnGui)
        return;

    camera.HandleZoom(yoffset);
    */
}

void Application::OnKeyBoard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Press ESC to close the window.
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    // Camera
    if (key == GLFW_KEY_W && action == GLFW_PRESS || key == GLFW_KEY_S && action == GLFW_RELEASE) {
        m_cameraTranslate += Vector3::forward;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS || key == GLFW_KEY_W && action == GLFW_RELEASE) {
        m_cameraTranslate += Vector3::back;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS || key == GLFW_KEY_D && action == GLFW_RELEASE) {
        m_cameraTranslate += Vector3::left;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS || key == GLFW_KEY_A && action == GLFW_RELEASE) {
        m_cameraTranslate += Vector3::right;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS || key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
        m_cameraTranslate += Vector3::up;
    }
    else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS || key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        m_cameraTranslate += Vector3::down;
    }

    // Tab UI
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        m_toggleGUI = !m_toggleGUI;

        if (m_toggleGUI) {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}