//
// Created by ozzadar on 2024-12-22.
//

#include "debug_window.h"

#include <utility>
#include "imgui_internal.h"

namespace OZZ::game::ui {
    DebugWindow::DebugWindow(GetApplicationStateFunction  inAppStateFunction) : appStateFunction(std::move(inAppStateFunction)) {}

    void DebugWindow::registerInDockspace() {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        static auto first_time = true;
        if (first_time)
        {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace );
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowViewport()->Size);

            // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
            //   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
            //                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("Left", dock_id_left);
        }
    }

    void DebugWindow::render() {
        const auto ApplicationState = appStateFunction();

        ImGuiWindowClass windowClass;
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingSplit;

        auto windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowClass(&windowClass);

        ImGui::SetNextWindowClass(&windowClass);
        ImGui::Begin("Left", nullptr, windowFlags);

        if (ImGui::CollapsingHeader("Connection Status", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool bIsConnected = ApplicationState.ConnectionState == ConnectionState::Connected;

            ImGui::Text("Connected: ");
            ImGui::SameLine();
            // Red text if false, green text if true
            ImGui::TextColored(bIsConnected ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), "%s", bIsConnected ? "Yes" : "No");
            // TODO: Implement ping pong message
            ImGui::Text("Ping: %dms", 50);

            // Connect and disconnect buttons that will be disabled greyed out when the connection state is not Disconnected
            ImGui::BeginDisabled(ApplicationState.ConnectionState == ConnectionState::Connected);
            if (ImGui::Button("Connect")) {
                ConnectToServerRequested();
            }
            ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::BeginDisabled(ApplicationState.ConnectionState != ConnectionState::Connected);
            if (ImGui::Button("Disconnect")) {
                DisconnectFromServerRequested();
            }
            ImGui::EndDisabled();
        }


        if (ImGui::CollapsingHeader("User Details", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool bIsLoggedIn = ApplicationState.LoginState == LoginState::LoggedIn;
            ImGui::Text("Logged in: ");
            ImGui::SameLine();
            ImGui::TextColored(bIsLoggedIn ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), "%s", bIsLoggedIn ? "Yes" : "No");
            ImGui::Text("Username: %s", ApplicationState.PlayerState.Username.c_str());

            ImGui::BeginDisabled(!bIsLoggedIn);
            if (ImGui::Button("Logout")) {
                LogoutRequested();
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(bIsLoggedIn);
            if (ImGui::Button("Login")) {
                LoginRequested("p.a.mauviel@gmail.com", "password");
            }
            ImGui::EndDisabled();
        }
        ImGui::End();
    }


} // OZZ