cmake_minimum_required(VERSION 3.25)

project(opengl-az LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
add_compile_definitions(GL_SILENCE_DEPRECATION)
endif ()

find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(app
src/main.cpp
src/ngn/utils/log.h
src/ngn/rendering/shader.h
src/ngn/rendering/shader.cpp
src/ngn/rendering/camera.h
src/ngn/rendering/camera.cpp
src/ngn/rendering/texture.h
src/ngn/rendering/texture.cpp
src/ngn/rendering/mesh.h
src/ngn/rendering/mesh.cpp
)

target_include_directories(app PRIVATE ${STB_INCLUDE_DIRS})
target_link_libraries(app PRIVATE glfw glm::glm glad::glad imgui::imgui assimp::assimp ${OPENGL_LIBRARIES})

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
