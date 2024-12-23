//
// Created by ozzadar on 2024-12-22.
//

#include "lights/ui/user_interface.h"
#include "spdlog/spdlog.h"
#include "imgui_internal.h"


#ifdef OZZ_RENDERER_OPENGL

#include <backends/imgui_impl_opengl3.h>

#define RendererInitFunction ImGui_ImplOpenGL3_Init
#define RendererNewFrameFunction ImGui_ImplOpenGL3_NewFrame
#define RendererRenderDrawDataFunction ImGui_ImplOpenGL3_RenderDrawData
#define RendererShutdownFunction ImGui_ImplOpenGL3_Shutdown
#else
#define RendererInitFunction NOT_A_FUNCTION
#define RendererNewFrameFunction NOT_A_FUNCTION
#define RendererRenderDrawDataFunction NOT_A_FUNCTION
#define RendererShutdownFunction NOT_A_FUNCTION
#endif

#ifdef OZZ_WINDOW_GLFW
// TODO: This is hacky. I coded myself into a corner I don't really feel like climbing myself out of
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>

#define WindowType GLFWwindow*
#define WindowNewFrameFunction ImGui_ImplGlfw_NewFrame
#define WindowShutdownFunction ImGui_ImplGlfw_Shutdown
#else
#define WindowType NOT_A_TYPE
#define WindowNewFrameFunction NOT_A_FUNCTION
#define WindowShutdownFunction NOT_A_FUNCTION
#endif

#if defined(OZZ_WINDOW_GLFW) && defined(OZZ_RENDERER_OPENGL)
#define WindowInitFunction ImGui_ImplGlfw_InitForOpenGL
#else
#define WindowInitFunction NOT_A_FUNCTION
#define WindowNewFrameFunction NOT_A_FUNCTION
#endif

namespace OZZ {
    void UserInterface::Init(void *windowHandle) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        auto &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        RendererInitFunction();
        WindowInitFunction(static_cast<WindowType>(windowHandle), true);
    }

    void UserInterface::Render() {
        RendererNewFrameFunction();
        WindowNewFrameFunction();
        ImGui::NewFrame();

        initImGuiDockSpace();
        for (auto &Component: components) {
            if (auto componentRef = Component.lock()) {
                componentRef->registerInDockspace();
            }
        }
        for (auto &Component: components) {
            if (auto componentRef = Component.lock()) {
                componentRef->render();
            }
        }
        finishImGuiDockSpace();

        ImGui::ShowDemoWindow();
        ImGui::Render();
        RendererRenderDrawDataFunction(ImGui::GetDrawData());

#ifdef _WIN32
        auto &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
#ifdef OZZ_WINDOW_GLFW
            WindowType backupContext = glfwGetCurrentContext();
#endif
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
#ifdef OZZ_WINDOW_GLFW
            glfwMakeContextCurrent(backupContext);
#endif
        }
#endif
    }

    void UserInterface::Shutdown() {
        for (auto &Component: components) {
            Component.reset();
        }
        components.clear();
        RendererShutdownFunction();
        WindowShutdownFunction();
        ImGui::DestroyContext();
    }

    void UserInterface::initImGuiDockSpace() {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
// because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGuiWindowClass windowClass;
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingSplit;
        ImGui::SetNextWindowClass(&windowClass);

// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
// all active windows docked into it will lose their parent and become undocked.
// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
//        ImGui::PopStyleVar(2);
    }

        void UserInterface::finishImGuiDockSpace() {
            ImGui::End();
        }

    } // OZZ