cmake_minimum_required(VERSION 3.25)

project(opengl-az LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
add_compile_definitions(GL_SILENCE_DEPRECATION)
endif ()

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(app
src/main.cpp
src/ngn/utils/log.h
src/ngn/rendering/shader.h
src/ngn/rendering/shader.cpp
)

target_link_libraries(app PRIVATE glfw glm::glm glad::glad ${OPENGL_LIBRARIES})

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
