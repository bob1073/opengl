#include "Application.h"

#include "TextureManager.h"

#include <spdlog/spdlog.h>

#include <iostream>

Application::Application()
    :
    m_Camera{glm::vec3(0.0f, 0.0f, 3.0f)}, m_FirstMouse{true}, m_Dt{0.0f}, m_LastFrame{0.0f}
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting application...");

    initGLFW();
    initGLAD();

    // Enable Depth testing for Z-Buffer
    glEnable(GL_DEPTH_TEST);

    // Shaders
    m_Shader.create("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    m_Shader.use();
    
    // Textures
    TextureManager::create("container", "resources/container.jpg");
    TextureManager::create("wall", "resources/wall.jpg");

    // Create awesome cube grid
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    for (auto& cube : m_Cubes)
    {
        cube.create(m_Shader, glm::vec3(x, y, z));
        cube.setTexture("container");
        
        
        x += 1.0f;
        if (x == 10.0f) 
        {
            cube.setTexture("wall");
            z += 1.0f;
            x = 0.0f;
        }
        if (z == 10.0f)
        {
            cube.setTexture("");
            y += 1.0f;
            z = 0.0f;
        }
    }

    // MODEL MATRIX
    m_Model = glm::mat4(1.0f);

    // VIEW MATRIX
    // note that we're translating the scene in the reverse direction of where we want to move
    m_View = glm::mat4(1.0f);
    m_View = glm::translate(m_View, glm::vec3(0.0f, 0.0f, -3.0f));
    
    // PROJECTION MATRIX
    m_AspectRatio = static_cast<float>(s_WindowWidth) / s_WindowHeight;
    m_Projection = glm::mat4(1.0f);
    m_Projection = glm::perspective(glm::radians(45.0f), m_AspectRatio, 0.1f, 100.0f);
;
    m_Shader.setMatrix4("view", m_View);
    m_Shader.setMatrix4("projection", m_Projection);

}

Application::~Application()
{
    spdlog::info("Closing application...");
    // Delete textures
    TextureManager::freeAll();
    glfwTerminate();
}

void Application::updateInput()
{
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        close();
    }

    // Toggle on/off wireframe mode
    static bool lock = 0, wf_mode = 0;
    if (!glfwGetKey(m_Window, GLFW_KEY_SPACE)) {
        lock = 0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_SPACE) && lock == 0) {
        glPolygonMode(GL_FRONT_AND_BACK, (wf_mode = 1 - wf_mode) ? GL_LINE : GL_FILL);
        lock = 1;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(FORWARD, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(BACKWARD, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(LEFT, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(RIGHT, m_Dt);

        

    /*
    if (glfwGetKey(m_Window, GLFW_KEY_UP) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(0.0f, 0.0f, m_Dt));
    if (glfwGetKey(m_Window, GLFW_KEY_DOWN) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(0.0f, 0.0f, -m_Dt));
    if (glfwGetKey(m_Window, GLFW_KEY_LEFT) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(-m_Dt, 0.0f, 0.0f));
    if (glfwGetKey(m_Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(m_Dt, 0.0f, 0.0f));

    if (glfwGetKey(m_Window, GLFW_KEY_Z) == GLFW_PRESS)
        m_Cube.rotate(m_Shader, m_Dt, glm::vec3(1.0f, 1.0f, 0.0f));
    if (glfwGetKey(m_Window, GLFW_KEY_C) == GLFW_PRESS)
        m_Cube.rotate(m_Shader, -m_Dt, glm::vec3(1.0f, 1.0f, 0.0f));

    if (glfwGetKey(m_Window, GLFW_KEY_F) == GLFW_PRESS)
        m_Cube.scale(m_Shader, glm::vec3(1.0f - m_Dt));
    if (glfwGetKey(m_Window, GLFW_KEY_G) == GLFW_PRESS)
        m_Cube.scale(m_Shader, glm::vec3(1.0f + m_Dt));*/

    
    glfwPollEvents();
}

void Application::update()
{
    
    m_Projection = glm::perspective(glm::radians(m_Camera.GetZoom()), m_AspectRatio, 0.1f, 100.0f);
    m_Shader.setMatrix4("projection", m_Projection);
    m_View = m_Camera.GetViewMatrix();
    m_Shader.setMatrix4("view", m_View);

    // Update DT
    float currentFrame = static_cast<float>(glfwGetTime());
    m_Dt = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;
}

void Application::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& cube : m_Cubes)
    {
        cube.render(m_Shader);
    }

    glfwSwapBuffers(m_Window);
}

void Application::close()
{
    glfwSetWindowShouldClose(m_Window, true);
}

void Application::initGLFW()
{
    spdlog::info("Initializing GLFW...");
    glfwInit();

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Set core-profile mode
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Create GLFW window and set some properties
    m_Window = glfwCreateWindow(s_WindowWidth, s_WindowHeight, "OpenGL Testing", nullptr, nullptr);

    // Check for window creation failure
    if (m_Window == nullptr)
    {
        spdlog::critical("Failed to create GFLW window");
        glfwTerminate();
    }

    spdlog::info("GLFW successfully initialized.");

    // Set the window surface as main context
    glfwMakeContextCurrent(m_Window);

    // tell GLFW to capture our mouse
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(m_Window, this);
}

void Application::initGLAD()
{
    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        spdlog::critical("Failed to initialize GLAD");
        glfwTerminate();
    }
    
    spdlog::info("GLAD successfully initialized.");

    // Set viewport as window size
    glViewport(0, 0, s_WindowWidth, s_WindowHeight);

    // Set callback for window resize
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
        {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->framebufferSizeCallback(window, width, height);
            }
            
        });

        
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xposIn, double yposIn)
        {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->mouse_callback(window, xposIn, yposIn);
            }
        });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->scroll_callback(window, xoffset, yoffset);
            }
        });

    // Show max vertex attrib number
    int numAttrib;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttrib);
    spdlog::info("Maximum vertex attributes supported: {}", numAttrib);

    // Compile shaders
    //compileShaders();
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    // Recalcular la matriz de proyección
    m_AspectRatio = (float)width / (float)height;
}

void Application::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top

    m_LastX = xpos;
    m_LastY = ypos;

    m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

