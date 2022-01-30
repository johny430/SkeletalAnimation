#include "Engine/Importer.h"
#include <glfw3.h>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(640, 480, "App", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    auto Object = Importer::LoadModel("assets/vampire/vampire.dae");
    Animation animation2 = Object.animations[0];
    Model render_model2 = Object.model;
    float _dt = 0.0f, _last_frame_time = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        const float current_time = float(glfwGetTime());
        _dt = current_time - _last_frame_time;
        _last_frame_time = current_time;
        animation2.Update(_dt);
        const glm::mat4 projection = glm::perspective(glm::radians(70.0f), (640.0f / 480.0f), 0.1f, 100.0f);
        const glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 model = glm::mat4(1.0f);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_model2.Draw(projection, view, model, animation2.transforms());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}