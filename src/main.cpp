#define GLEW_STATIC
#define GLEW_NO_GLU
#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../include/Shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Erreur d'initialisation GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "My 3D Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Erreur création fenêtre GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "Erreur GLEW : " << glewGetErrorString(glewStatus) << "\n";
        return -1;
    }

    float houseVertices[] = {
        // base cube + toit pyramide
        -0.5f, 0.0f,  0.5f,  0.5f, 0.0f,  0.5f,  0.5f, 0.5f,  0.5f,
         0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f, -0.5f,
         0.5f, 0.5f, -0.5f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f,
        -0.5f, 0.0f, -0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.5f,  0.5f,
        -0.5f, 0.5f,  0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,
         0.5f, 0.5f,  0.5f,  0.5f, 0.0f,  0.5f,  0.5f, 0.0f, -0.5f,
        -0.5f, 0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.0f, 0.8f,  0.0f,
         0.5f, 0.5f,  0.5f,  0.5f, 0.5f, -0.5f,  0.0f, 0.8f,  0.0f,
         0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.0f, 0.8f,  0.0f,
        -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f,  0.0f, 0.8f,  0.0f,
    };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader shader("../shaders/basic.vert", "../shaders/basic.frag");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, -2.5f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("objectColor", 0.8f, 0.5f, 0.2f);
        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
