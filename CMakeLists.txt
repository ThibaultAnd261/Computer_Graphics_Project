cmake_minimum_required(VERSION 3.10)
project(3DViewer)

set(CMAKE_CXX_STANDARD 17)

# Définir le chemin vers GLEW installé via Homebrew
set(GLEW_ROOT "/opt/homebrew/opt/glew")

# Inclure les headers et libs de GLEW et GLFW
include_directories(${GLEW_ROOT}/include)
link_directories(${GLEW_ROOT}/lib)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
link_directories(${GLFW_LIBRARY_DIRS})

add_executable(3DViewer
    src/main.cpp
)

# Link avec OpenGL, GLFW et GLEW
if (APPLE)
    target_link_libraries(3DViewer
        ${GLFW_LIBRARIES}
        GLEW
        "-framework OpenGL"
    )
elseif (WIN32)
    target_link_libraries(3DViewer
        ${GLFW_LIBRARIES}
        glew32
        opengl32
    )
else()
    target_link_libraries(3DViewer
        ${GLFW_LIBRARIES}
        GLEW
        GL
    )
endif()
