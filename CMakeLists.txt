cmake_minimum_required(VERSION 3.10)
project(3DViewer)

set(CMAKE_CXX_STANDARD 17)

# Trouver les dépendances avec pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)

# Include
include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/tinyobjloader
)

# Link
link_directories(
    ${GLFW_LIBRARY_DIRS}
    ${GLEW_LIBRARY_DIRS}
)

# GLM (header only)
find_path(GLM_INCLUDE_DIR glm/glm.hpp)
if (GLM_INCLUDE_DIR)
    include_directories(${GLM_INCLUDE_DIR})
else()
    message(FATAL_ERROR "GLM not found! Install with: pacman -S mingw-w64-x86_64-glm")
endif()

# Fichiers sources
add_executable(3DViewer
    src/main.cpp
    src/Shader.cpp
    src/Camera.cpp
    external/tinyobjloader/tiny_obj_loader.cc
)

# Link libs selon OS
if (WIN32)
    target_link_libraries(3DViewer ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} opengl32)
else()
    target_link_libraries(3DViewer ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} GL)
endif()
