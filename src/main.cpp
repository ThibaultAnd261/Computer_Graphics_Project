#define GLEW_STATIC
#define GLEW_NO_GLU

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Callback pour ajuster la taille de la fenêtre (viewport)
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

    // Création de la fenêtre et du contexte OpenGL
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

    // Rendu en couleurs sRGB 
    glEnable(GL_FRAMEBUFFER_SRGB);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Nettoyage framebuffer

        // TODO: affichage objet

        glfwSwapBuffers(window);  // Affiche le rendu
        glfwPollEvents();
    }

    // Nettoyage
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}