#include "GUI.h"
#include "Scene/SceneSerializer.h"
#include "Events/InputManager.h"

#include "yaml-cpp/exceptions.h"

#include <string>

namespace
{
    const std::string S_SAVE_SCENE_KEY = "SaveScene";
    const std::string S_LOAD_SCENE_KEY = "LoadScene";

    std::string s_configFilePath;
    std::string s_darkFilePath;
    std::string s_darkFileName;

    bool s_isLoadDirty = false;

    namespace FileError
    {
        bool openModal = false;
        std::string msg;
    }
}

namespace ImGui
{
	void onInit(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(R"(resources\fonts\AovelSansRounded-rdDL.ttf)", 16.0f);
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

        ImGui::LoadIniSettingsFromDisk("imgui_views/imgui1.ini");

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
        style.GrabRounding = style.FrameRounding = 2.3f;
	}

	void onDestroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void onUpdate()
	{
        if (s_isLoadDirty) {
            ImGui::LoadIniSettingsFromDisk(s_configFilePath.c_str());
            s_isLoadDirty = false;
        }

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void onRender()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

    static void openDialogForSavingScene()
    {
        ImGuiFileDialog::Instance()->OpenDialog(S_SAVE_SCENE_KEY, "Save", ".dark", ".", 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
    }

    static void openDialogForLoadingScene()
    {
        ImGuiFileDialog::Instance()->OpenDialog(S_LOAD_SCENE_KEY, "Load", ".dark", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
    } 

    bool dockSpace(Dark::Scene* scene, ImGuiWindowFlags flags)
    {
        bool registryCleared = false;

        // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
        // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
        // In this specific demo, we are not using DockSpaceOverViewport() because:
        // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
        // - we allow the host window to have padding (when opt_padding == true)
        // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
        // TL;DR; this demo is more complicated than what you would normally use.
        // If we removed all the options we are showcasing, this demo would become:
        //     void ShowExampleAppDockSpace()
        //     {
        //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        //     }

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= flags;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", 0, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }



        /////////////////////////////////////////////////////////////////////
        //////////////////             MENU BAR              ///////////////
        ///////////////////////////////////////////////////////////////////

        static int view = 0;

        static auto loadIni = [&](int num, const std::string& filePath)
        {
            view = num;
            s_configFilePath = filePath;
            s_isLoadDirty = true;
        };

        static auto save = [&]()
        {
            if (s_darkFilePath.empty())
                openDialogForSavingScene();
            else
                Dark::SceneSerializer::serialize(scene, s_darkFilePath, s_darkFileName);
        };

        if (Dark::InputManager::isDown(DARK_KEY_LEFT_CONTROL) &&
                Dark::InputManager::isDown(DARK_KEY_LEFT_SHIFT) &&
                Dark::InputManager::isPressed(DARK_KEY_S))
            openDialogForSavingScene();
        else if (Dark::InputManager::isDown(DARK_KEY_LEFT_CONTROL) &&
            Dark::InputManager::isPressed(DARK_KEY_S))
            save();

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project")) {
                    Dark::SceneSerializer::resetAndClearScene(scene);
                    s_darkFilePath = std::string();
                    s_darkFileName = std::string();
                    registryCleared = true;
                }

                if (ImGui::MenuItem("Load...")) {
                    openDialogForLoadingScene();
                }

                if (ImGui::BeginMenu("Save")) {
                    if (ImGui::MenuItem("Save", "Ctrl+S")) {
                        save();
                    }
                    if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) {
                        openDialogForSavingScene();
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("View 1", nullptr, view == 0)) {
                    loadIni(0, "imgui_views/imgui1.ini");
                }
                if (ImGui::MenuItem("View 2", nullptr, view == 1)) {
                    loadIni(1, "imgui_views/imgui2.ini");
                }
                if (ImGui::MenuItem("View 3", nullptr, view == 2)) {
                    loadIni(2, "imgui_views/imgui3.ini");
                }
                if (ImGui::MenuItem("View 4", nullptr, view == 3)) {
                    loadIni(3, "imgui_views/imgui4.ini");
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Save", nullptr)) {
                    ImGui::SaveIniSettingsToDisk(s_configFilePath.c_str());
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        if (ImGuiFileDialog::Instance()->Display(S_SAVE_SCENE_KEY, 32, { DIALOG_MIN_WINDOW_SIZE_X, DIALOG_MIN_WINDOW_SIZE_Y })) {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk()) {
                Dark::SceneSerializer::serialize(scene, ImGuiFileDialog::Instance()->GetFilePathName(), ImGuiFileDialog::Instance()->GetCurrentFileName());
                s_darkFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                s_darkFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
        else if (ImGuiFileDialog::Instance()->Display(S_LOAD_SCENE_KEY, 32, { DIALOG_MIN_WINDOW_SIZE_X, DIALOG_MIN_WINDOW_SIZE_Y })) {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk()) {
                try {
                    Dark::SceneSerializer::deserialize(scene, ImGuiFileDialog::Instance()->GetFilePathName(), ImGuiFileDialog::Instance()->GetCurrentFileName());

                    s_darkFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                    s_darkFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                }
                catch (YAML::Exception e) {
                    OpenPopup("Couldn't Load File");
                    FileError::openModal = true;
                    FileError::msg = e.msg;

                    if (!s_darkFilePath.empty())
                        Dark::SceneSerializer::deserialize(scene, s_darkFilePath, s_darkFileName);
                }

                registryCleared = true;
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        if (BeginPopupModal("Couldn't Load File", &FileError::openModal)) {
            Text("Couldn't Load File! Contact the developer if you need assistance!\n");
            Text("Error: %s\n", FileError::msg.c_str());
            Separator();
            if (Button("Ok"))
                FileError::openModal = false;
            EndPopup();
        }

        ImGui::End();

        return registryCleared;
    }

    inline static float getTargetAR()
    {
        return 16.0f / 9.0f;
    }

    ImVec2 getLargestSizeForViewport()
    {
        ImVec2 winSize = ImGui::GetContentRegionAvail();
        winSize.x -= ImGui::GetScrollX();
        winSize.y -= ImGui::GetScrollY();

        float aspectWidth = winSize.x;
        float aspectHeight = aspectWidth / getTargetAR();

        if (aspectHeight > winSize.y) {
            aspectHeight = winSize.y;
            aspectWidth = aspectHeight * getTargetAR();
        }

        return { aspectWidth, aspectHeight };
    }

    ImVec2 getCenteredPosForViewport(const ImVec2& windowSize)
    {
        ImVec2 winSize = ImGui::GetContentRegionAvail();
        winSize.x -= ImGui::GetScrollX();
        winSize.y -= ImGui::GetScrollY();

        float viewPX = (winSize.x / 2.0f) - (windowSize.x / 2.0f);
        float viewPY = (winSize.y / 2.0f) - (windowSize.y / 2.0f);

        return { viewPX + ImGui::GetCursorPosX(), viewPY + ImGui::GetCursorPosY() };
    }
}