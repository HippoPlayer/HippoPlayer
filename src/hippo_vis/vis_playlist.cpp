#include "imgui/imgui.h"

void show_playlist() {
    //static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

    static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    if (ImGui::BeginTable("3ways", 3, flags))
    {

        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
        ImGui::TableHeadersRow();

        // Simple storage to output a dummy file-system.
        struct MyTreeNode
        {
            const char*     Name;
            const char*     Type;
            int             Size;
            int             ChildIdx;
            int             ChildCount;
            static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                const bool is_folder = (node->ChildCount > 0);
                if (is_folder)
                {
                    bool open = ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_SpanFullWidth);
                    ImGui::TableNextColumn();
                    ImGui::TextDisabled("--");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(node->Type);
                    if (open)
                    {
                        for (int child_n = 0; child_n < node->ChildCount; child_n++)
                            DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
                        ImGui::TreePop();
                    }
                }
                else
                {
                    ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", node->Size);
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(node->Type);
                }
            }
        };
        static const MyTreeNode nodes[] =
        {
            { "Root",                         "Folder",       -1,       1, 3    }, // 0
            { "Music",                        "Folder",       -1,       4, 2    }, // 1
            { "Textures",                     "Folder",       -1,       6, 3    }, // 2
            { "desktop.ini",                  "System file",  1024,    -1,-1    }, // 3
            { "File1_a.wav",                  "Audio file",   123000,  -1,-1    }, // 4
            { "File1_b.wav",                  "Audio file",   456000,  -1,-1    }, // 5
            { "Image001.png",                 "Image file",   203128,  -1,-1    }, // 6
            { "Copy of Image001.png",         "Image file",   203256,  -1,-1    }, // 7
            { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
        };

        MyTreeNode::DisplayNode(&nodes[0], nodes);

        ImGui::EndTable();
    }
}
