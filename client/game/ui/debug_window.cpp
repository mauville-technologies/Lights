//
// Created by ozzadar on 2024-12-22.
//

#include "debug_window.h"
#include "imgui_internal.h"

namespace OZZ {
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
            auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);

            // we now dock our windows into the docking node we made above
            ImGui::DockBuilderDockWindow("Down", dock_id_down);
            ImGui::DockBuilderDockWindow("Left", dock_id_left);
        }
    }

    void DebugWindow::render() {
        ImGuiWindowClass windowClass;
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingSplit;

        auto windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowClass(&windowClass);

        // TODO: Need a patch in Imgui.cppL18133 to make this work. Bug in ImGui docking branch
        const ImVec4 bgColor = ImVec4(1.f, 0.f, 0.f, 0.3f);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, bgColor);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor);

        ImGui::Begin("Left", nullptr, 0);
        ImGui::Text("Hello, left!");
        ImGui::End();

        ImGui::SetNextWindowClass(&windowClass);
        ImGui::Begin("Down", nullptr, windowFlags);
        ImGui::Text("Hello, down!");

        ImGui::End();
        ImGui::PopStyleColor(3);
    }

} // OZZ