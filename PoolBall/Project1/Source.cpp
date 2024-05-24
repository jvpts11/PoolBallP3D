#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <windows.h>

// Vertex data for a paralelepiped
float vertices[] = {
    // positions          // colors
    -0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  // red
     0.5f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  // green
     0.5f,  0.5f, -1.0f,  0.0f, 0.0f, 1.0f,  // blue
     0.5f,  0.5f, -1.0f,  0.0f, 0.0f, 1.0f,  // blue
    -0.5f,  0.5f, -1.0f,  1.0f, 1.0f, 0.0f,  // yellow
    -0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  // red

    -0.5f, -0.5f,  1.0f,  0.0f, 1.0f, 1.0f,  // cyan
     0.5f, -0.5f,  1.0f,  1.0f, 0.0f, 1.0f,  // magenta
     0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 1.0f,  // white
     0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 1.0f,  // white
    -0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 0.0f,  // yellow
    -0.5f, -0.5f,  1.0f,  0.0f, 1.0f, 1.0f,  // cyan

    -0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 0.0f,  // yellow
    -0.5f,  0.5f, -1.0f,  1.0f, 1.0f, 0.0f,  // yellow
    -0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  // red
    -0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  // red
    -0.5f, -0.5f,  1.0f,  0.0f, 1.0f, 1.0f,  // cyan
    -0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 0.0f,  // yellow

     0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 1.0f,  // white
     0.5f,  0.5f, -1.0f,  0.0f, 0.0f, 1.0f,  // blue
     0.5f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  // green
     0.5f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  // green
     0.5f, -0.5f,  1.0f,  1.0f, 0.0f, 1.0f,  // magenta
     0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 1.0f,  // white

    -0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  // red
     0.5f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  // green
     0.5f, -0.5f,  1.0f,  1.0f, 0.0f, 1.0f,  // magenta
     0.5f, -0.5f,  1.0f,  1.0f, 0.0f, 1.0f,  // magenta
    -0.5f, -0.5f,  1.0f,  0.0f, 1.0f, 1.0f,  // cyan
    -0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  // red

    -0.5f,  0.5f, -1.0f,  1.0f, 1.0f, 0.0f,  // yellow
     0.5f,  0.5f, -1.0f,  0.0f, 0.0f, 1.0f,  // blue
     0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 1.0f,  // white
     0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 1.0f,  // white
    -0.5f,  0.5f,  1.0f,  1.0f, 1.0f, 0.0f,  // yellow
    -0.5f,  0.5f, -1.0f,  1.0f, 1.0f, 0.0f   // yellow
};

bool firstMouse = true;
float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
float zoom = 45.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool leftMousePressed = false;
float lastMouseX, lastMouseY;

std::string readFile(const char* filePath);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main() {
    if (!glfwInit()) {
        std::cerr << "Error to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Project", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Error creating the OpenGL Window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    std::string vertexShaderSource = readFile("vertex_shader.vert");
    std::string fragmentShaderSource = readFile("fragment_shader.frag");

    const char* vertexShaderCode = vertexShaderSource.c_str();
    const char* fragmentShaderCode = fragmentShaderSource.c_str();

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(zoom), (float)width / (float)height, 0.1f, 100.0f);

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

std::string readFile(const char* filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }
    std::stringstream sstr;
    sstr << fileStream.rdbuf();
    fileStream.close();
    return sstr.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftMousePressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftMousePressed = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (leftMousePressed) {
        float xoffset = xpos - lastMouseX;
        float yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

        lastMouseX = xpos;
        lastMouseY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
}
