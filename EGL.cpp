#include "EGL.h"
#include <ctime>
#include <shadowhook.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "ImGui/imgui_impl_android.h"
#include "ImGui/imgui_impl_opengl3.h"


//ImGui_ImplAndroid_NewFrame override
void ImGui_ImplAndroid_NewFrameEx() {
    static double g_Time;
    ImGuiIO& io = ImGui::GetIO();

    int32_t window_width = EGL::eglWidth;
    int32_t window_height = EGL::eglHeight;

    auto display_width = (float)window_width;
    auto display_height = (float)window_height;

    io.DisplaySize = ImVec2((float)window_width, (float)window_height);

    if (window_width > 0 && window_height > 0)
        io.DisplayFramebufferScale = ImVec2(display_width / (float)window_width, display_height / (float)window_height);

    struct timespec current_timepiece {};
    clock_gettime(CLOCK_MONOTONIC, &current_timepiece);
    double current_time = (double)(current_timepiece.tv_sec) + (current_timepiece.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

}


using eglSwapBuffers_ = EGLBoolean(*)(EGLDisplay display, EGLSurface surface);
static eglSwapBuffers_ o_eglSwapBuffers = nullptr;
EGLBoolean f_eglSwapBuffers(EGLDisplay display, EGLSurface surface)
{
    eglQuerySurface(display,surface,EGL_WIDTH,&EGL::eglWidth);
    eglQuerySurface(display,surface,EGL_HEIGHT,&EGL::eglHeight);

    static bool init = false;
    static EGLDisplay  lastDisplay = nullptr;
    static EGLSurface lastSurface = nullptr;

    if (display != lastDisplay || surface != lastSurface) {
        if (init) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplAndroid_Shutdown();

            //reset mode
            /*
            if (ImGui::GetCurrentContext() != nullptr) {
                ImGui::DestroyContext();
            }
            */

            init = false;
        }
        lastDisplay = display;
        lastSurface = surface;

    }

    if (!init) {
        if (ImGui::GetCurrentContext() == nullptr) {
            ImGui::CreateContext();

            ImGuiIO &io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.LogFilename = nullptr;

            //UI::Init();
        }

        ImGuiIO &io = ImGui::GetIO();
        float scale = (float)EGL::eglWidth / (float)EGL::eglHeight;
        io.FontGlobalScale = scale * 1.5f;

        ImGui_ImplAndroid_Init(nullptr);
        ImGui_ImplOpenGL3_Init("#version 300 es");

        init = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrameEx();
    ImGui::NewFrame();

    //UI::Display();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return o_eglSwapBuffers(display, surface);

}

void EGL::Init() {
    shadowhook_hook_sym_name("libEGL.so", "eglSwapBuffers",(void*)f_eglSwapBuffers,(void**)&o_eglSwapBuffers);

}
