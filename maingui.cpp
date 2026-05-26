#include <iostream>
#include <string>
#include <vector>

// Dear ImGui заголовки
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> 

// Твои классы и исключения
#include "ArraySequence.h"
#include "ListSequence.h"
#include "Exceptions.h"

// Вспомогательная функция для инициализации окна
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main() {
    // 1. Инициализация GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Lab 2: Sequence Manager", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Включаем вертикальную синхронизацию

    // 2. Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 3. СОСТОЯНИЕ ТВОЕГО ПРИЛОЖЕНИЯ
    static ArraySequence<int> mySequence; 
    static int valueInput = 0;
    static int indexInput = 0;
    static std::string lastError = "";
    static std::string lastAction = "Welcome!";

    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- ОКНО УПРАВЛЕНИЯ ---
        ImGui::Begin("Sequence Operations");

        ImGui::Text("Current action: %s", lastAction.c_str());
        if (!lastError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", lastError.c_str());
        }

        ImGui::Separator();

        // Секция ввода данных
        ImGui::InputInt("Integer Value", &valueInput);
        ImGui::InputInt("Index", &indexInput);

        if (ImGui::Button("Append")) {
            mySequence.Append(valueInput);
            lastAction = "Appended " + std::to_string(valueInput);
            lastError = "";
        }
        ImGui::SameLine();
        if (ImGui::Button("Prepend")) {
            mySequence.Prepend(valueInput);
            lastAction = "Prepended " + std::to_string(valueInput);
            lastError = "";
        }

        ImGui::Separator();

        // Секция получения данных
        if (ImGui::Button("Get Element")) {
            try {
                int val = mySequence.Get(indexInput);
                lastAction = "Value at [" + std::to_string(indexInput) + "] is " + std::to_string(val);
                lastError = "";
            } catch (const std::out_of_range& e) { // Твои исключения
                lastError = "Index out of range!";
            }
        }

        if (ImGui::Button("Clear Sequence")) {
            mySequence = ArraySequence<int>(); // Простое пересоздание
            lastAction = "Sequence cleared";
            lastError = "";
        }

        ImGui::End();

        // --- ОКНО ВИЗУАЛИЗАЦИИ ---
        ImGui::Begin("Visualizer");
        ImGui::Text("Sequence Elements (%d total):", mySequence.GetSize());
        
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true);
        for (int i = 0; i < mySequence.GetSize(); i++) {
            // Выделяем текущий индекс, если он совпадает с вводом
            bool isSelected = (i == indexInput);
            std::string label = "[" + std::to_string(i) + "]: " + std::to_string(mySequence.Get(i));
            
            if (ImGui::Selectable(label.c_str(), isSelected)) {
                indexInput = i;
            }
        }
        ImGui::EndChild();
        ImGui::End();

        // Рендеринг
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}