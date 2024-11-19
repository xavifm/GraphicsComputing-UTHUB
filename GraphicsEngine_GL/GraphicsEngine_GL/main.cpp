#include <iostream>
#include <glad//glad.h>
#include <GLFW/glfw3.h>
#include "Shader/ShaderProgram.h"

//Simple Colibri
float vertices[] = {
    0.4, 0.4, 0.0,
    0.1, 0.4, 0.0,
    0.1, 0.2, 0.0,
    -0.2, -0.1, 0.0,
    -0.2, 0.4, 0.0,
    -0.1, 0.1, 0.0,
    -0.2, 0.0, 0.0,
    0.2, -0.1, 0.0,
    -0.5, -0.2, 0.0
};

unsigned int indices[] = {
    0, 1, 2,
    1, 2, 3,
    4, 2, 5
};

std::string ReadTextFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return "";

    std::stringstream stringbuffer;
    stringbuffer << file.rdbuf();
    file.close();

    return stringbuffer.str();
}

void framebuffer_size_callback(GLFWwindow* _window, int _width, int _height) 
{
    glViewport(0, 0, _width, _height);
}

int main(void)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GraphicsEngineGL", nullptr, nullptr);

    if (window == nullptr) 
    {
        std::cout << "Failed to create the Window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::string vertexShaderText = ReadTextFile("vertex.glsl");
    std::string fragmentShaderText = ReadTextFile("fragment.glsl");

    Shader* vertexShader = new Shader(GL_VERTEX_SHADER, vertexShaderText);
    Shader* fragmentShader = new Shader(GL_FRAGMENT_SHADER, fragmentShaderText);

    ShaderProgram* shaderProgram = new ShaderProgram();
    shaderProgram->AttachShader(*vertexShader);
    shaderProgram->AttachShader(*fragmentShader);
    shaderProgram->Link();

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram->Use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
