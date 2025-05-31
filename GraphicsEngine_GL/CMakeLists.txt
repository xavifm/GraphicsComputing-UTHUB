cmake_minimum_required(VERSION 3.10)
project(GraphicsEngine_GL)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES
        "GraphicsEngine_GL/*.cpp"
)

set(GLAD_SRC GraphicsEngine_GL/glad.c)

include_directories(
        GraphicsEngine_GL
        GraphicsEngine_GL/Controllers
        GraphicsEngine_GL/Controllers/Camera
        GraphicsEngine_GL/Controllers/Input
        GraphicsEngine_GL/Controllers/OpenGL
        GraphicsEngine_GL/Controllers/Window
        GraphicsEngine_GL/Controllers/Worlds
        GraphicsEngine_GL/Mesh
        GraphicsEngine_GL/Model
        GraphicsEngine_GL/Parameters
        GraphicsEngine_GL/Shader
        GraphicsEngine_GL/Texture
        GraphicsEngine_GL/ThirdParty/include
)

add_executable(GraphicsEngine_GL ${SRC_FILES} ${GLAD_SRC})

find_package(OpenGL REQUIRED)

target_link_libraries(GraphicsEngine_GL
        OpenGL::GL
        glfw
        dl
)
