
#include "Input.h"
#include <shadowhook.h>
#include "ImGui/imgui_impl_android.h"

using initializeMotionEvent_ = int(*)(void*, void*, void*);
initializeMotionEvent_ o_initializeMotionEvent = nullptr;
int f_initializeMotionEvent(void *inputConsumer, void *motionEvent, void *inputMessage) {
    int result = o_initializeMotionEvent(inputConsumer, motionEvent, inputMessage);
    auto* event = (AInputEvent*)inputConsumer;
    ImGui_ImplAndroid_HandleInputEvent(event);

    ImGuiIO& io = ImGui::GetIO();
    float wheel = io.MouseDelta.y ;
    io.MouseWheel = wheel * 0.01f;

    return result;

}

void Input::Init() {
    shadowhook_hook_sym_name("libinput.so","_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE",(void*)f_initializeMotionEvent,(void**)&o_initializeMotionEvent);

}