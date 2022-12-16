#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ngn/rendering/camera.h"
#include "ngn/rendering/shader.h"
#include "ngn/utils/log.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <math.h>
#include <vector>

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 600;
constexpr auto WINDOW_TITLE = "App";
constexpr float COLOR_RED = 1;
constexpr float COLOR_GREEN = .5;
constexpr float COLOR_BLUE = .3125;
constexpr float COLOR_ALPHA = 1;
constexpr float MOVEMENT_SPEED = 4;
constexpr float MOUSE_SENSITIVITY = .1;

glm::vec3 player_position(0, 0, -5);
ngn::Camera camera({ .position = player_position });

glm::vec3 light_source_position(1.2f, 1.0f, 2.0f);

float delta_time = 0;
float last_frame = 0;

float last_x = WINDOW_WIDTH / 2.;
float last_y = WINDOW_HEIGHT / 2.;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double position_x, double position_y);
void scroll_callback(GLFWwindow* window, double offset_x, double offset_y);
void click_callback(GLFWwindow* window, int input, int action, int mods);
unsigned load_texture(const char* path);

struct ImGuiControls {
    struct {
        glm::vec3 color;
        float ambient_strength;
        float diffuse_strength;
    } light;
    struct {
        glm::vec3 color;
        float specular;
        float shininess;
    } material;
    struct {
        float light_speed;
        float light_distance;
        float cubes_rotation_speed;
    } elements;
};

void display_imgui_controls(bool& is_open, ImGuiControls& imgui_controls);

int main(int argc, char** argv)
{
    LOG("Hello, world!");

    // Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (window == NULL) {
        LOGERR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOGERR("Failed to initialize GLAD");
        return -1;
    }

    // Info
    int numberOfAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numberOfAttributes);
    LOGF("Maximum number of vertex attributes supported: %d", numberOfAttributes);

    // Viewport
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, click_callback);

    // Enable z sorting
    glEnable(GL_DEPTH_TEST);

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Triangle init
    // std::vector<float> vertices {
    //     -.5, .5, 0, 1, 0, 0, 0, 1, // top left
    //     -.5, -.5, 0, 0, 1, 0, 0, 0, // bottom left
    //     .5, -.5, 0, 0, 0, 1, 1, 0, // bottom right
    //     .5, .5, 0, 1, 1, 0, 1, 1, // top right
    // };

    std::vector<float> cube_vertices {
        -0.5f, -0.5f, -0.5f, 0, 0, -1, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0, 0, -1, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0, 0, -1, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0, 0, -1, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0, 0, -1, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0, 0, -1, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0, 0, 1, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0, 0, 1, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0, 0, 1, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0, 0, 1, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0, 0, 1, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0, 0, 1, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1, 0, 0, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1, 0, 0, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1, 0, 0, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1, 0, 0, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1, 0, 0, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1, 0, 0, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1, 0, 0, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1, 0, 0, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1, 0, 0, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1, 0, 0, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1, 0, 0, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1, 0, 0, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0, -1, 0, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0, -1, 0, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0, -1, 0, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0, -1, 0, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0, -1, 0, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0, -1, 0, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0, 1, 0, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0, 1, 0, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0, 1, 0, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0, 1, 0, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0, 1, 0, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0, 1, 0, 0.0f, 1.0f
    };

    // std::vector<unsigned> indices {
    //     0, 1, 2, // t1
    //     2, 3, 0, // t2
    // };

    // Buffers init
    unsigned VAO, VBO, EBO;

    // glGenVertexArrays(1, &VAO);
    // glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube_vertices.size(), cube_vertices.data(), GL_STATIC_DRAW);

    // glGenBuffers(1, &EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), indices.data(), GL_STATIC_DRAW);

    /*
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), NULL);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Safe: the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Light buffers
    unsigned light_VAO;

    glGenVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ngn::Shader shader("assets/shaders/tutorial.vert", "assets/shaders/tutorial.frag");

    ImGuiControls imgui_controls {
        .light {
            .color { 1, 1, 1 },
            .ambient_strength = .1,
            .diffuse_strength = .5 },
        .material {
            .color { 1, .5, .31 },
            .specular = .5,
            .shininess = 32 },
        .elements {
            .light_speed = 1,
            .light_distance = 10,
            .cubes_rotation_speed = 10 }
    };

    ngn::Shader lighted_shader("assets/shaders/light.vert", "assets/shaders/light.frag");

    ngn::Shader light_source_shader("assets/shaders/light.vert", "assets/shaders/light_source.frag");

    // unsigned texture1 = load_texture("assets/images/container.jpg");
    // unsigned texture2 = load_texture("assets/images/awesomeface.png");
    // if (!texture1 || !texture2)
    //     return -1;

    // shader.use();
    // shader.set("texture1", 0);
    // shader.set("texture2", 1);

    glm::mat4 projection;

    std::vector<glm::vec3> cube_positions {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    glm::mat4 lighted_model(1.0);

    glm::mat4 light_source_model(1.0);

    bool is_material_controls_open = true;

// Optional
#ifdef WIREFRAME_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        // Draw
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float current_time = glfwGetTime();
        delta_time = current_time - last_frame;
        last_frame = glfwGetTime();

        // float greenValue = sin(current_time) / 2.0f + 0.5f;

        // Temporary ?
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        //
        projection = glm::perspective(glm::radians(camera.fov()), (float)width / (float)height, .1f, 100.f);

        glm::mat4 view = camera.get_view_matrix();

        light_source_position = glm::vec3 { cos(current_time * imgui_controls.elements.light_speed) * imgui_controls.elements.light_distance, 0, sin(current_time * imgui_controls.elements.light_speed) * imgui_controls.elements.light_distance };
        light_source_model = glm::mat4(1.0);
        light_source_model = glm::translate(light_source_model, light_source_position);
        light_source_model = glm::scale(light_source_model, glm::vec3 { .2 });

        glm::vec3 diffuse_color = imgui_controls.light.color * glm::vec3 { imgui_controls.light.diffuse_strength };
        glm::vec3 ambient_color = imgui_controls.light.color * glm::vec3 { imgui_controls.light.ambient_strength };

        light_source_shader.use();
        light_source_shader.set("projection", projection);
        light_source_shader.set("model", light_source_model);
        light_source_shader.set("view", view);
        light_source_shader.set("color", imgui_controls.light.color);
        glBindVertexArray(light_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lighted_shader.use();
        lighted_shader.set("projection", projection);
        lighted_shader.set("model", lighted_model);
        lighted_shader.set("view", view);
        lighted_shader.set("viewPos", camera.position());
        lighted_shader.set("material.ambient", imgui_controls.material.color);
        lighted_shader.set("material.diffuse", imgui_controls.material.color);
        lighted_shader.set("material.specular", glm::vec3 { imgui_controls.material.specular });
        lighted_shader.set("material.shininess", imgui_controls.material.shininess);
        lighted_shader.set("light.position", light_source_position);
        lighted_shader.set("light.ambient", ambient_color);
        lighted_shader.set("light.diffuse", diffuse_color);
        lighted_shader.set("light.specular", imgui_controls.light.color);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);

        // shader.use();
        // shader.set("ourColor", glm::vec4(COLOR_RED, greenValue, COLOR_BLUE, COLOR_ALPHA));
        // shader.set("projection", projection);
        // shader.set("view", camera.view);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);
        // glBindVertexArray(VAO);

        // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        for (size_t i = 0; i < cube_positions.size(); i++) {
            glm::mat4 model(1);
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, current_time * glm::radians(imgui_controls.elements.cubes_rotation_speed * (i + 1)) + glm::radians(angle), { 1.f, .3f, .5f });
            lighted_shader.set("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

        display_imgui_controls(is_material_controls_open, imgui_controls);

        // After draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.look_at({ 0, 0, 0 });

    const float camera_speed = MOVEMENT_SPEED * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player_position += camera_speed * camera.front();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player_position -= camera_speed * camera.front();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player_position -= glm::normalize(glm::cross(camera.front(), camera.up())) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player_position += glm::normalize(glm::cross(camera.front(), camera.up())) * camera_speed;
    camera.move_to(player_position);
}

void mouse_callback(GLFWwindow* window, double position_x, double position_y)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
        return;
    float offset_x = position_x - last_x;
    float offset_y = last_y - position_y;
    last_x = position_x;
    last_y = position_y;

    offset_x *= MOUSE_SENSITIVITY;
    offset_y *= MOUSE_SENSITIVITY;

    camera.rotate(offset_x, offset_y);
}

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y)
{
    camera.zoom(offset_y);
}

void click_callback(GLFWwindow* window, int input, int action, int mods)
{
    if (input == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        double position_x, position_y;
        glfwGetCursorPos(window, &position_x, &position_y);
        last_x = position_x;
        last_y = position_y;
    }
}

unsigned load_texture(const char* path)
{
    // Texture loading
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data) {
        LOGERR("Failed to load image.");
        return 0;
    }

    // Generate Texture
    unsigned texture;
    glGenTextures(1, &texture);

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // Texture repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // For GL_CLAMP_TO_BORDER
    // float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load Texture
    unsigned color_mode = nrChannels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, color_mode, width, height, 0, color_mode, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free Image data
    stbi_image_free(data);

    return texture;
}

void display_imgui_controls(bool& is_open, ImGuiControls& imgui_controls)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

    if (is_open) {
        ImGui::Begin("Controls", &is_open, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Close")) {
                    is_open = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::CollapsingHeader("Light")) {
            ImGui::ColorEdit3("Color", glm::value_ptr(imgui_controls.light.color));
            ImGui::SliderFloat("Diffuse strength", &imgui_controls.light.diffuse_strength, 0, 1);
            ImGui::SliderFloat("Ambient strength", &imgui_controls.light.ambient_strength, 0, 1);
        }

        if (ImGui::CollapsingHeader("Material")) {
            ImGui::ColorEdit3("Color", glm::value_ptr(imgui_controls.material.color));
            ImGui::SliderFloat("Specular", &imgui_controls.material.specular, 0, 1);
            ImGui::InputFloat("Shininess", &imgui_controls.material.shininess, 1);
        }

        if (ImGui::CollapsingHeader("Elements")) {
            ImGui::DragFloat("Light speed", &imgui_controls.elements.light_speed, .01);
            ImGui::DragFloat("Light distance", &imgui_controls.elements.light_distance);
            ImGui::DragFloat("Cubes rotation speed", &imgui_controls.elements.cubes_rotation_speed);
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
