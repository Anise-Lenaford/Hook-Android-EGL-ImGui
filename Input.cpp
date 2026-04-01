
#include "Input.h"
#include <shadowhook.h>
#include "imgui_impl_android.h"

using AInputQueue_preDispatchEvent_ = int(*)(void*, void*);
static AInputQueue_preDispatchEvent_ o_AInputQueue_preDispatchEvent = nullptr;
int f_AInputQueue_preDispatchEvent(void *inputQueue, void *motionEvent) {
    int result = o_AInputQueue_preDispatchEvent(inputQueue, motionEvent);

    if (motionEvent != nullptr) {
        ImGui_ImplAndroid_HandleInputEvent(reinterpret_cast<AInputEvent*>(motionEvent));

        ImGuiIO& io = ImGui::GetIO();
        float wheel = io.MouseDelta.y;
        io.MouseWheel = wheel * 0.01f;
    }

    return result;

}


void Input::Init() {
    shadowhook_hook_sym_name("libandroid.so","AInputQueue_preDispatchEvent",(void*)f_AInputQueue_preDispatchEvent,(void**)&o_AInputQueue_preDispatchEvent);

}

