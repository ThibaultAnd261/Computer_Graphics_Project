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
#include "../include/Camera.hpp"
#include "../external/tinyobjloader/tiny_obj_loader.h"
#include "../include/mat4.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GLuint fbo, fboTexture, fboDepth;
GLuint quadVAO, quadVBO;
int postEffect = 0;
bool showCursor = true;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f; // temps entre deux frames
float lastFrame = 0.0f;
float lastX = 400.0f; // position souris
float lastY = 300.0f; // position souris
bool firstMouse = true; // pour éviter le saut de la souris

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    lastX = xpos;
    lastY = ypos;
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLenum glewStatus = glewInit();

    // === ImGui Init ===
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // === FBO + quad init ===
    int width = 800, height = 600;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    glGenTextures(1, &fboDepth);
    glBindTexture(GL_TEXTURE_2D, fboDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepth, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        -1.f,  1.f,  0.0f, 1.0f,
        -1.f, -1.f,  0.0f, 0.0f,
         1.f,  1.f,  1.0f, 1.0f,
         1.f, -1.f,  1.0f, 0.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


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

    // Shader shader("../shaders/basic.vert", "../shaders/basic.frag");
    // Shader phong
    Shader shader("../shaders/phong.vert", "../shaders/phong.frag");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.008f));


    // glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, -2.5f));
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);


    // Chargement snow_covered_cottage .obj avec TinyObjLoader
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    tinyobj::ObjReaderConfig config;
    config.triangulate = true;

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile("../models/snow_covered_cottage_obj.obj", config)) {
        std::cerr << "Erreur TinyObjLoader : " << reader.Error() << std::endl;
        return -1;
    }

    attrib = reader.GetAttrib();
    shapes = reader.GetShapes();

    // Construction des données pour OpenGL
    std::vector<float> vertices;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            // Normale
            vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
            vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
            vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
        }
    }

    // VAO/VBO pour l'objet chargé
    GLuint vaoHouse, objVBO;
    glGenVertexArrays(1, &vaoHouse);
    glGenBuffers(1, &objVBO);
    glBindVertexArray(vaoHouse);
    glBindBuffer(GL_ARRAY_BUFFER, objVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normale
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Chargement .obj plante 1
    tinyobj::attrib_t attribPlante1;
    std::vector<tinyobj::shape_t> shapesPlante1;
    std::vector<tinyobj::material_t> materialsPlante1;
    tinyobj::ObjReader readerPlante1;
    if (!readerPlante1.ParseFromFile("../models/eb_house_plant_02.obj", config)) {
        std::cerr << "Erreur chargement plante 1 : " << readerPlante1.Error() << std::endl;
        return -1;
    }
    attribPlante1 = readerPlante1.GetAttrib();
    shapesPlante1 = readerPlante1.GetShapes();

    std::vector<float> verticesPlante1;
    for (const auto& shape : shapesPlante1) {
        for (const auto& index : shape.mesh.indices) {
            verticesPlante1.push_back(attribPlante1.vertices[3 * index.vertex_index + 0]);
            verticesPlante1.push_back(attribPlante1.vertices[3 * index.vertex_index + 1]);
            verticesPlante1.push_back(attribPlante1.vertices[3 * index.vertex_index + 2]);

            verticesPlante1.push_back(attribPlante1.normals[3 * index.normal_index + 0]);
            verticesPlante1.push_back(attribPlante1.normals[3 * index.normal_index + 1]);
            verticesPlante1.push_back(attribPlante1.normals[3 * index.normal_index + 2]);
        }
    }

    GLuint vaoPlante1, vboPlante1;
    glGenVertexArrays(1, &vaoPlante1);
    glGenBuffers(1, &vboPlante1);
    glBindVertexArray(vaoPlante1);
    glBindBuffer(GL_ARRAY_BUFFER, vboPlante1);
    glBufferData(GL_ARRAY_BUFFER, verticesPlante1.size() * sizeof(float), verticesPlante1.data(), GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normale
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Chargement .obj plante 2
    tinyobj::attrib_t attribPlante2;
    std::vector<tinyobj::shape_t> shapesPlante2;
    std::vector<tinyobj::material_t> materialsPlante2;
    tinyobj::ObjReader readerPlante2;
    if (!readerPlante2.ParseFromFile("../models/eb_house_plant_03.obj", config)) {
        std::cerr << "Erreur chargement plante 2 : " << readerPlante2.Error() << std::endl;
        return -1;
    }
    attribPlante2 = readerPlante2.GetAttrib();
    shapesPlante2 = readerPlante2.GetShapes();

    std::vector<float> verticesPlante2;
    for (const auto& shape : shapesPlante2) {
        for (const auto& index : shape.mesh.indices) {
            verticesPlante2.push_back(attribPlante2.vertices[3 * index.vertex_index + 0]);
            verticesPlante2.push_back(attribPlante2.vertices[3 * index.vertex_index + 1]);
            verticesPlante2.push_back(attribPlante2.vertices[3 * index.vertex_index + 2]);

            verticesPlante2.push_back(attribPlante2.normals[3 * index.normal_index + 0]);
            verticesPlante2.push_back(attribPlante2.normals[3 * index.normal_index + 1]);
            verticesPlante2.push_back(attribPlante2.normals[3 * index.normal_index + 2]);
        }
    }

    GLuint vaoPlante2, vboPlante2;
    glGenVertexArrays(1, &vaoPlante2);
    glGenBuffers(1, &vboPlante2);
    glBindVertexArray(vaoPlante2);
    glBindBuffer(GL_ARRAY_BUFFER, vboPlante2);
    glBufferData(GL_ARRAY_BUFFER, verticesPlante2.size() * sizeof(float), verticesPlante2.data(), GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normale
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mat4 worldMatrix;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, 800, 600);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        GLuint worldLoc = glGetUniformLocation(shader.ID, "WorldMatrix");
        glUniformMatrix4fv(worldLoc, 1, GL_FALSE, worldMatrix.data.data());

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);
        
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setVec3("lightPos", 2.0f, 2.0f, 2.0f);
        shader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        shader.setVec3("objectColor", 0.8f, 0.5f, 0.2f);    // couleur objet
        shader.setMat4("model", glm::value_ptr(model));

        // Cottage 3D
        glm::mat4 modelMaison = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -0.5f, 0.f));
        modelMaison = glm::scale(modelMaison, glm::vec3(0.008f));
        shader.setVec3("objectColor", 0.8f, 0.5f, 0.2f);
        shader.setMat4("model", glm::value_ptr(modelMaison));
        glBindVertexArray(vaoHouse);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

        // Plante1
        glm::mat4 modelPlante1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.f, -0.5f, 0.f));
        modelPlante1 = glm::scale(modelPlante1, glm::vec3(0.01f));
        shader.setVec3("objectColor", 0.2f, 0.8f, 0.3f);
        shader.setMat4("model", glm::value_ptr(modelPlante1));
        glBindVertexArray(vaoPlante1);
        glDrawArrays(GL_TRIANGLES, 0, verticesPlante1.size() / 3);

        // Plante2
        glm::mat4 modelPlante2 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.f, -0.5f, 0.f));
        modelPlante2 = glm::scale(modelPlante2, glm::vec3(0.01f));
        shader.setVec3("objectColor", 0.2f, 0.8f, 0.3f);
        shader.setMat4("model", glm::value_ptr(modelPlante2));
        glBindVertexArray(vaoPlante2);
        glDrawArrays(GL_TRIANGLES, 0, verticesPlante2.size() / 3);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Post-processing");
        ImGui::Text("Choix du shader :");

        ImGui::RadioButton("Aucun", &postEffect, 0);
        ImGui::RadioButton("Grayscale", &postEffect, 1);
        ImGui::RadioButton("Inversion", &postEffect, 2);
        ImGui::RadioButton("Sépia", &postEffect, 3);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 800, 600);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        Shader postShader("../shaders/post.vert", "../shaders/post.frag");
        postShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        postShader.setInt("screenTexture", 0);
        postShader.setInt("postEffect", postEffect);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
