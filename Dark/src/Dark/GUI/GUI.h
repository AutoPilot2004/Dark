#pragma once
#include "Vendor/imgui/imgui_impl_glfw.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"
#include "Vendor/imgui/imgui_stdlib.h"
#include "Vendor/imgui/imgui_internal.h"
#include "Vendor/imgui/ImGuiFileDialog.h"

typedef struct GLFWwindow GLFWwindow;

namespace Dark {
	class Scene;
}

namespace ImGui
{
	const float DIALOG_MIN_WINDOW_SIZE_X = 960.0f;
	const float DIALOG_MIN_WINDOW_SIZE_Y = (9.0f * DIALOG_MIN_WINDOW_SIZE_X) / 16.0f;

	extern void onInit(GLFWwindow* window);
	extern void onDestroy();
	extern void onUpdate();
	extern void onRender();

	extern bool dockSpace(Dark::Scene* scene, ImGuiWindowFlags flags);

	extern ImVec2 getLargestSizeForViewport();
	extern ImVec2 getCenteredPosForViewport(const ImVec2& windowSize);
}