cmake_minimum_required(VERSION 3.19)
project(GraphicsEngine_GL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(OpenGL REQUIRED)

# --------------------------------------------------
# Read the projectFile components
# --------------------------------------------------

set(ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GraphicsEngine_GL")
set(PROJECT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Engine.projectFile")
set(GENERATOR_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/generate_components.py")

set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated")
set(GENERATED_CPP "${GENERATED_DIR}/RegisterComponents.cpp")
set(GENERATED_HEADER "${GENERATED_DIR}/RegisterComponents.h")

get_filename_component(PROJECT_DIR "${PROJECT_FILE}" DIRECTORY)

if(NOT EXISTS "${PROJECT_FILE}")
    message(FATAL_ERROR "No existeix: ${PROJECT_FILE}")
endif()

if(NOT EXISTS "${GENERATOR_SCRIPT}")
    message(FATAL_ERROR "No existeix: ${GENERATOR_SCRIPT}")
endif()

set_property(DIRECTORY APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS "${PROJECT_FILE}"
)

# --------------------------------------------------
# Read the projectFile components
# --------------------------------------------------

file(READ "${PROJECT_FILE}" PROJECT_JSON)
string(JSON COMPONENT_COUNT LENGTH "${PROJECT_JSON}" components)

set(COMPONENT_SOURCES "")
set(COMPONENT_HEADERS "")
set(COMPONENT_INCLUDE_DIRS "")

if(COMPONENT_COUNT GREATER 0)
    math(EXPR LAST_COMPONENT "${COMPONENT_COUNT} - 1")

    foreach(INDEX RANGE 0 ${LAST_COMPONENT})
        string(JSON COMPONENT_HEADER
                GET "${PROJECT_JSON}" components ${INDEX} header
        )

        string(JSON COMPONENT_SOURCE
                GET "${PROJECT_JSON}" components ${INDEX} source
        )

        get_filename_component(HEADER_PATH
                "${COMPONENT_HEADER}"
                ABSOLUTE
                BASE_DIR "${PROJECT_DIR}"
        )

        get_filename_component(SOURCE_PATH
                "${COMPONENT_SOURCE}"
                ABSOLUTE
                BASE_DIR "${PROJECT_DIR}"
        )

        if(NOT EXISTS "${HEADER_PATH}")
            message(FATAL_ERROR "No existeix el header: ${HEADER_PATH}")
        endif()

        if(NOT EXISTS "${SOURCE_PATH}")
            message(FATAL_ERROR "No existeix el source: ${SOURCE_PATH}")
        endif()

        get_filename_component(HEADER_DIR "${HEADER_PATH}" DIRECTORY)

        list(APPEND COMPONENT_HEADERS "${HEADER_PATH}")
        list(APPEND COMPONENT_SOURCES "${SOURCE_PATH}")
        list(APPEND COMPONENT_INCLUDE_DIRS "${HEADER_DIR}")
    endforeach()
endif()

list(REMOVE_DUPLICATES COMPONENT_INCLUDE_DIRS)

# --------------------------------------------------
# Generate RegisterComponents
# --------------------------------------------------

file(MAKE_DIRECTORY "${GENERATED_DIR}")

file(CONFIGURE
        OUTPUT "${GENERATED_HEADER}"
        CONTENT "#pragma once\n\nvoid RegisterProjectComponents();\n"
        @ONLY
)

add_custom_command(
        OUTPUT "${GENERATED_CPP}"

        COMMAND "${Python3_EXECUTABLE}"
        "${GENERATOR_SCRIPT}"
        "${PROJECT_FILE}"
        "${GENERATED_CPP}"

        DEPENDS
        "${GENERATOR_SCRIPT}"
        "${PROJECT_FILE}"
        ${COMPONENT_HEADERS}

        COMMENT "Generant RegisterComponents.cpp..."
        VERBATIM
)

# --------------------------------------------------
# Engine fonts
# --------------------------------------------------

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        "${ENGINE_DIR}/*.cpp"
)

list(FILTER SRC_FILES EXCLUDE REGEX "/ThirdParty/include/imgui/")

list(FILTER SRC_FILES EXCLUDE REGEX "/RegisterComponents\\.cpp$")

list(APPEND SRC_FILES ${COMPONENT_SOURCES})
list(REMOVE_DUPLICATES SRC_FILES)

# --------------------------------------------------
# ImGui
# --------------------------------------------------

set(IMGUI_DIR "${ENGINE_DIR}/ThirdParty/include/imgui")

add_library(imgui STATIC
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/imgui_tables.cpp"
        "${IMGUI_DIR}/imgui_impl_glfw.cpp"
        "${IMGUI_DIR}/imgui_impl_opengl3.cpp"
)

target_include_directories(imgui PUBLIC
        "${IMGUI_DIR}"
        "${ENGINE_DIR}/ThirdParty/include"
)

target_link_libraries(imgui PUBLIC
        glfw
        OpenGL::GL
        ${CMAKE_DL_LIBS}
)

# --------------------------------------------------
# Executable
# --------------------------------------------------

add_executable(GraphicsEngine_GL
        ${SRC_FILES}
        "${ENGINE_DIR}/glad.c"
        "${GENERATED_CPP}"
        "${GENERATED_HEADER}"
)

target_include_directories(GraphicsEngine_GL BEFORE PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${PROJECT_DIR}"

        ${COMPONENT_INCLUDE_DIRS}

        "${ENGINE_DIR}"
        "${ENGINE_DIR}/Controllers"
        "${ENGINE_DIR}/Controllers/Camera"
        "${ENGINE_DIR}/Controllers/Input"
        "${ENGINE_DIR}/Controllers/OpenGL"
        "${ENGINE_DIR}/Controllers/UI"
        "${ENGINE_DIR}/Controllers/Window"
        "${ENGINE_DIR}/Controllers/Worlds"
        "${ENGINE_DIR}/Mesh"
        "${ENGINE_DIR}/Parameters"
        "${ENGINE_DIR}/Shader"
        "${ENGINE_DIR}/Texture"
        "${ENGINE_DIR}/ThirdParty/include"
)

target_link_libraries(GraphicsEngine_GL PRIVATE
        OpenGL::GL
        glfw
        ${CMAKE_DL_LIBS}
        imgui
)