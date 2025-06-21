cmake_minimum_required(VERSION 3.10)
project(3DViewer)

set(CMAKE_CXX_STANDARD 17)

# Dépendances GLFW et GLEW via pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)

# Inclusions
include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/tinyobjloader
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

# Liste manuelle des fichiers ImGui
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# GLM (header-only)
find_path(GLM_INCLUDE_DIR glm/glm.hpp)
if (GLM_INCLUDE_DIR)
    include_directories(${GLM_INCLUDE_DIR})
else()
    message(FATAL_ERROR "GLM not found! Install with: pacman -S mingw-w64-x86_64-glm")
endif()

# Exécutable
add_executable(3DViewer
    src/main.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/mat4.cpp
    external/tinyobjloader/tiny_obj_loader.cc
    ${IMGUI_SOURCES}
)

# Link selon OS
if (WIN32)
    target_link_libraries(3DViewer ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} opengl32)
else()
    target_link_libraries(3DViewer ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} GL)
endif()
