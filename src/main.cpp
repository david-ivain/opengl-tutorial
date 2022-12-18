#include "ngn/ngn.h"
#include "ngn/rendering/model.h"

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
#include <string>
#include <vector>

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 600;
constexpr auto WINDOW_TITLE = "App";
constexpr float MOVEMENT_SPEED = 4;
constexpr float MOUSE_SENSITIVITY = .1;

constexpr float AMBIENT_STRENGTH = .1;

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
void draw_mesh(const ngn::Mesh& mesh, const ngn::Shader& shader);
void draw_model(const ngn::Model& model, const ngn::Shader& shader);

struct ImGuiControls {
    struct {
        glm::vec3 color;
        float ambient_strength;
        float diffuse_strength;
    } direction_light;
    struct {
        glm::vec3 color;
        float diffuse_strength;
    } point_light;
    struct {
        bool enable;
        glm::vec3 color;
        float diffuse_strength;
    } spot_light;
    struct {
        float shininess;
    } material;
    struct {
        float cubes_rotation_speed;
    } elements;
};

void display_imgui_controls(bool& is_open, ImGuiControls& imgui_controls);

int main(int argc, char** argv)
{
    // Init
    if (!glfwInit()) {
        LOGERR("Failed to initialize GLFW.");
        return -1;
    }
    LOG("GLFW initialized.");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    LOG("OpenGl hints set.");

    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (window == NULL) {
        LOGERR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    LOG("GLFW window created.");

    glfwMakeContextCurrent(window);

    // Load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOGERR("Failed to initialize GLAD");
        return -1;
    }
    LOG("GLAD loaded.");

    // Info
    int numberOfAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numberOfAttributes);
    LOGF("Maximum number of vertex attributes supported: %d", numberOfAttributes);

    // Viewport
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    LOG("Framebuffer size callback set.");

    // Mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, click_callback);
    LOG("Mouse callbacks set.");

    // Enable z sorting
    glEnable(GL_DEPTH_TEST);
    LOG("Depth test enabled.");

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    LOG("Imgui initialized.");

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    std::vector<ngn::Vertex> cube_vertices {
        { { -0.5f, -0.5f, -0.5f }, { 0, 0, -1 }, { 0.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0, 0, -1 }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0, 0, -1 }, { 1.0f, 1.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0, 0, -1 }, { 1.0f, 1.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 0, 0, -1 }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0, 0, -1 }, { 0.0f, 0.0f } },

        { { -0.5f, -0.5f, 0.5f }, { 0, 0, 1 }, { 0.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0, 0, 1 }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0, 0, 1 }, { 1.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0, 0, 1 }, { 1.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0, 0, 1 }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, 0.5f }, { 0, 0, 1 }, { 0.0f, 0.0f } },

        { { -0.5f, 0.5f, 0.5f }, { -1, 0, 0 }, { 1.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { -1, 0, 0 }, { 1.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { -1, 0, 0 }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { -1, 0, 0 }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, 0.5f }, { -1, 0, 0 }, { 0.0f, 0.0f } },
        { { -0.5f, 0.5f, 0.5f }, { -1, 0, 0 }, { 1.0f, 0.0f } },

        { { 0.5f, 0.5f, 0.5f }, { 1, 0, 0 }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 1, 0, 0 }, { 1.0f, 1.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 1, 0, 0 }, { 0.0f, 1.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 1, 0, 0 }, { 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 1, 0, 0 }, { 0.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 1, 0, 0 }, { 1.0f, 0.0f } },

        { { -0.5f, -0.5f, -0.5f }, { 0, -1, 0 }, { 0.0f, 1.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0, -1, 0 }, { 1.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0, -1, 0 }, { 1.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0, -1, 0 }, { 1.0f, 0.0f } },
        { { -0.5f, -0.5f, 0.5f }, { 0, -1, 0 }, { 0.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0, -1, 0 }, { 0.0f, 1.0f } },

        { { -0.5f, 0.5f, -0.5f }, { 0, 1, 0 }, { 0.0f, 1.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0, 1, 0 }, { 1.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0, 1, 0 }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0, 1, 0 }, { 1.0f, 0.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0, 1, 0 }, { 0.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 0, 1, 0 }, { 0.0f, 1.0f } }
    };

    std::vector<unsigned> indices;
    indices.reserve(36);
    for (unsigned i = 0; i < indices.capacity(); i++)
        indices.push_back(i);

    ngn::Mesh light_mesh {
        cube_vertices,
        indices,
        {},
    };
    ngn::Mesh container_mesh {
        cube_vertices,
        indices,
        {
            { "assets/images/container2.png", ngn::TextureType::Diffuse },
            { "assets/images/container2_specular.png", ngn::TextureType::Specular },
            { "assets/images/black.png", ngn::TextureType::Emission },
        },
    };
    LOG("Cube mesh loaded.");

    ngn::Model backpack_model { "assets/models/backpack/backpack.obj" };

    ImGuiControls imgui_controls {
        .direction_light {
            .color { 1, 1, 1 },
            .ambient_strength = AMBIENT_STRENGTH,
            .diffuse_strength = 1. },
        .point_light {
            .color { 1, 1, 1 },
            .diffuse_strength = 1. },
        .spot_light {
            .enable = false,
            .color { 1, 1, 1 },
            .diffuse_strength = 1. },
        .material {
            .shininess = 32 },
        .elements {
            .cubes_rotation_speed = 10 }
    };

    ngn::Shader lighted_shader("assets/shaders/light.vert", "assets/shaders/light_all.frag");
    ngn::Shader light_source_shader("assets/shaders/light.vert", "assets/shaders/light_source.frag");
    LOG("Shaders loaded.");

    // std::vector<ngn::Texture> container_textures;
    // container_textures.reserve(3);
    // container_textures.emplace_back("assets/images/container2.png", ngn::TextureType::Diffuse);
    // container_textures.emplace_back("assets/images/container2_specular.png", ngn::TextureType::Specular);
    // container_textures.emplace_back("assets/images/black.png", ngn::TextureType::Emission);
    // LOG("Textures loaded.");

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
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    std::vector<glm::vec3> point_light_positions = {
        glm::vec3(.7, .2, 2),
        glm::vec3(2.3, -3.3, -4),
        glm::vec3(-4, 2, -12),
        glm::vec3(0, 0, -3),
    };

    lighted_shader.use();
    for (size_t i = 0; i < point_light_positions.size(); i++) {
        lighted_shader.set(std::string("pointLights[") + std::to_string(i) + "].position", point_light_positions[i]);
        lighted_shader.set(std::string("pointLights[") + std::to_string(i) + "].ambient", glm::vec3 { 0 });
        lighted_shader.set(std::string("pointLights[") + std::to_string(i) + "].constant", 1.f);
        lighted_shader.set(std::string("pointLights[") + std::to_string(i) + "].linear", .09f);
        lighted_shader.set(std::string("pointLights[") + std::to_string(i) + "].quadratic", .032f);
    }

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
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float current_time = glfwGetTime();
        delta_time = current_time - last_frame;
        last_frame = glfwGetTime();

        // Temporary ?
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        //
        projection = glm::perspective(glm::radians(camera.fov()), (float)width / (float)height, .1f, 100.f);

        glm::mat4 view = camera.get_view_matrix();

        glm::vec3 point_diffuse_color = imgui_controls.point_light.color * imgui_controls.point_light.diffuse_strength;
        glm::vec3 dir_diffuse_color = imgui_controls.direction_light.color * imgui_controls.direction_light.diffuse_strength;

        glm::vec3 ambient_color = glm::vec3 { imgui_controls.direction_light.ambient_strength };

        light_source_shader.use();
        light_source_shader.set("projection", projection);
        light_source_shader.set("view", view);
        light_source_shader.set("color", point_diffuse_color);

        for (size_t i = 0; i < point_light_positions.size(); i++) {
            auto& point_light_position = point_light_positions[i];

            glm::mat4 model(1);
            model = glm::translate(model, point_light_position);
            model = glm::scale(model, glm::vec3 { .2 });
            light_source_shader.use();
            light_source_shader.set("model", model);
            draw_mesh(light_mesh, light_source_shader);

            lighted_shader.use();
            lighted_shader.set(std::string("pointLights[") + std::to_string(i) + "].diffuse", point_diffuse_color);
            lighted_shader.set(std::string("pointLights[") + std::to_string(i) + "].specular", imgui_controls.point_light.color);
        }

        lighted_shader.use();
        lighted_shader.set("projection", projection);
        lighted_shader.set("model", lighted_model);
        lighted_shader.set("view", view);
        lighted_shader.set("viewPos", camera.position());
        lighted_shader.set("material.shininess", imgui_controls.material.shininess);
        lighted_shader.set("dirLight.direction", glm::vec3 { -.2, -1, -.3 });
        lighted_shader.set("dirLight.ambient", ambient_color);
        lighted_shader.set("dirLight.diffuse", dir_diffuse_color);
        lighted_shader.set("dirLight.specular", imgui_controls.direction_light.color);
        lighted_shader.set("spotLight.direction", camera.front());
        lighted_shader.set("spotLight.position", camera.position());
        lighted_shader.set("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lighted_shader.set("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        lighted_shader.set("spotLight.ambient", glm::vec3 { 0 });
        lighted_shader.set("spotLight.diffuse", imgui_controls.spot_light.color * imgui_controls.spot_light.diffuse_strength * static_cast<float>(imgui_controls.spot_light.enable));
        lighted_shader.set("spotLight.specular", imgui_controls.spot_light.color * static_cast<float>(imgui_controls.spot_light.enable));

        glm::mat4 backpack_model_matrix { 1 };
        backpack_model_matrix = glm::translate(backpack_model_matrix, { 5, 0, 0 });
        lighted_shader.set("model", backpack_model_matrix);
        draw_model(backpack_model, lighted_shader);

        for (size_t i = 0; i < cube_positions.size(); i++) {
            glm::mat4 model(1);
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, current_time * glm::radians(imgui_controls.elements.cubes_rotation_speed * (i + 1)) + glm::radians(angle), { 1.f, .3f, .5f });
            lighted_shader.set("model", model);
            draw_mesh(container_mesh, lighted_shader);
        }

        glBindVertexArray(0);

        display_imgui_controls(is_material_controls_open, imgui_controls);

        // After draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    LOG("GLFW terminated. Exiting...");

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

void draw_mesh(const ngn::Mesh& mesh, const ngn::Shader& shader)
{
    // unsigned int diffuse_number = 1;
    // unsigned int specular_number = 1;
    // unsigned int emission_number = 1;
    auto& textures = mesh.textures();
    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        // std::string number;
        std::string name = ngn::TextureType::to_string(textures[i].type());
        // if (name == "diffuse")
        //     number = std::to_string(diffuse_number++);
        // else if (name == "specular")
        //     number = std::to_string(specular_number++);
        // else if (name == "emission")
        //     number = std::to_string(specular_number++);

        shader.set(("material." + name).c_str(), i);
        // shader.set(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id());
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(mesh.VAO());
    glDrawElements(GL_TRIANGLES, mesh.indices().size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void draw_model(const ngn::Model& model, const ngn::Shader& shader)
{
    for (auto& mesh : model.meshes())
        draw_mesh(mesh, shader);
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

        if (ImGui::CollapsingHeader("Direction Light")) {
            ImGui::ColorEdit3("Color", glm::value_ptr(imgui_controls.direction_light.color));
            ImGui::SliderFloat("Diffuse strength", &imgui_controls.direction_light.diffuse_strength, 0, 1);
            ImGui::SliderFloat("Ambient strength", &imgui_controls.direction_light.ambient_strength, 0, 1);
        }

        if (ImGui::CollapsingHeader("Point Lights")) {
            ImGui::ColorEdit3("Color", glm::value_ptr(imgui_controls.point_light.color));
            ImGui::SliderFloat("Diffuse strength", &imgui_controls.point_light.diffuse_strength, 0, 1);
        }

        if (ImGui::CollapsingHeader("Spot Light")) {
            ImGui::Checkbox("Enable", &imgui_controls.spot_light.enable);
            ImGui::ColorEdit3("Color", glm::value_ptr(imgui_controls.spot_light.color));
            ImGui::SliderFloat("Diffuse strength", &imgui_controls.spot_light.diffuse_strength, 0, 1);
        }

        if (ImGui::CollapsingHeader("Material")) {
            ImGui::InputFloat("Shininess", &imgui_controls.material.shininess, 1);
        }

        if (ImGui::CollapsingHeader("Elements")) {
            ImGui::DragFloat("Cubes rotation speed", &imgui_controls.elements.cubes_rotation_speed);
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
