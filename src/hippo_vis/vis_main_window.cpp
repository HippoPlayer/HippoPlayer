#include "imgui/imgui.h"
#include "imgui_bgfx/imgui.h"

ImFont* title_font = nullptr;
ImFont* author_font = nullptr;

void create_main_window() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    author_font = io.Fonts->AddFontFromFileTTF("data/fonts/Alegreya-Medium.ttf", 28.0f);
    title_font = io.Fonts->AddFontFromFileTTF("data/fonts/Alegreya-Bold.ttf", 32.0f);
    imguiRebuildFontAtlas();
}

void show_main_window() {
    ImGui::BeginChild("test", ImVec2(0, 0), true);
    ImGui::PushFont(title_font);
    ImGui::Text("Stardust Memories");
    ImGui::PopFont();
    ImGui::PushFont(author_font);
    ImGui::Text("by Jester");
    ImGui::PopFont();
    ImGui::EndChild();
}
