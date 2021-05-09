#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui_bgfx/imgui.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "vis_playlist.h"
#include "vis_main_window.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 400

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* glfwNativeWindowHandle(GLFWwindow* _window) {
#   if BX_PLATFORM_LINUX || BX_PLATFORM_BSD

    return (void*)(uintptr_t)glfwGetX11Window(_window);
#   elif BX_PLATFORM_OSX
    return glfwGetCocoaWindow(_window);
#   elif BX_PLATFORM_WINDOWS
    return glfwGetWin32Window(_window);
#   endif // BX_PLATFORM_
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int s_width = WINDOW_WIDTH;
int s_height = WINDOW_HEIGHT;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    s_width = width;
    s_height = height;

    bgfx::reset(width, height);
    bgfx::setViewRect(0, 0, 0, width, height);
}

struct ImGuiViewportDataGlfw
{
    GLFWwindow* window;
    bool        window_owned;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// . . U . .  // Create swap chain, frame buffers etc. (called after Platform_CreateWindow)

static void renderer_create_window(ImGuiViewport* vp) {
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)vp->PlatformUserData;

    if (!data->window_owned) {
        return;
    }

    void* native_handle = glfwNativeWindowHandle(data->window);

    bgfx::FrameBufferHandle handle = bgfx::createFrameBuffer(
        native_handle,
        uint16_t(vp->Size.x),
        uint16_t(vp->Size.y));

    vp->RendererUserData = (void*)(uintptr_t)handle.idx;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// N . U . D  // Destroy swap chain, frame buffers etc. (called before Platform_DestroyWindow)

static void renderer_destroy_window(ImGuiViewport* vp) {
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)vp->PlatformUserData;

    if (!data->window_owned) {
        return;
    }

    bgfx::FrameBufferHandle handle = { (uint16_t)(uintptr_t)vp->RendererUserData };

    bgfx::destroy(handle);

    // Flush destruction of swap chain before destroying window!
    bgfx::frame();
    bgfx::frame();

    vp->RendererUserData = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// . . U . .  // Resize swap chain, frame buffers etc. (called after Platform_SetWindowSize)

static void renderer_set_window_size(ImGuiViewport* vp, ImVec2 size) {
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)vp->PlatformUserData;

    //printf("%s:%d\n", __FILE__, __LINE__);

    if (!data->window_owned) {
        return;
    }

    //printf("%s:%d\n", __FILE__, __LINE__);

    renderer_destroy_window(vp);
    renderer_create_window(vp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// . . . R .  // (Optional) Clear framebuffer, setup render target, then render the viewport->DrawData. 'render_arg' is the value passed to RenderPlatformWindowsDefault().

static void renderer_render_window(ImGuiViewport* vp, void* render_arg) {
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)vp->PlatformUserData;

    //printf("%s:%d\n", __FILE__, __LINE__);

    if (!data->window_owned) {
        return;
    }

    int display_w, display_h;
    glfwGetWindowSize(data->window, &display_w, &display_h);

    //printf("%s:%d\n", __FILE__, __LINE__);

    bgfx::FrameBufferHandle handle = { (uint16_t)(uintptr_t)vp->RendererUserData };

    //printf("render to new frame buffer %f %f - %d\n", vp->Size.x, vp->Size.y, handle.idx);

    bgfx::setViewFrameBuffer(1, handle);
    bgfx::setViewRect(1, 0, 0, uint16_t(display_w), uint16_t(display_h));

    bgfx::setViewClear(1
        , BGFX_CLEAR_COLOR
        , 0xff00ffff
        , 1.0f
        , 0
        );

    // Set render states.
    bgfx::setState(BGFX_STATE_DEFAULT);

    imguiRenderDraws(vp->DrawData, 1, display_w, display_h);

    bgfx::touch(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
   static void renderer_swap_buffers(ImGuiViewport* vp, void* render_arg) {

   }
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

#if !defined(GLFW_EXPOSE_NATIVE_X11)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
    glfwWindowHint(GLFW_FLOATING, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "HippoPlayer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    bgfx::PlatformData pd;
#if defined(GLFW_EXPOSE_NATIVE_X11)
    pd.ndt = glfwGetX11Display();
#endif
    pd.nwh = glfwNativeWindowHandle(window);
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;

    bgfx::setPlatformData(pd);

    bgfx::Init bgfxInit;
    //bgfxInit.type = bgfx::RendererType::OpenGL;
    bgfxInit.type = bgfx::RendererType::Count;
    bgfxInit.resolution.width = WINDOW_WIDTH;
    bgfxInit.resolution.height = WINDOW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfxInit.platformData = pd;

    if (!bgfx::init(bgfxInit)) {
        printf("failed to init bgfx\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }

    imguiCreate();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigViewportsNoDecoration = true;
    ImGui::GetStyle().WindowRounding = 0.0f;

    ImGui_ImplGlfw_InitForOther(window, true);

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_CreateWindow = renderer_create_window;
    platform_io.Renderer_DestroyWindow = renderer_destroy_window;
    platform_io.Renderer_SetWindowSize = renderer_set_window_size;
    platform_io.Renderer_RenderWindow = renderer_render_window;
    platform_io.Renderer_SwapBuffers = nullptr;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
    colors[ImGuiCol_Border]                 = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_Button]                 = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
    colors[ImGuiCol_Header]                 = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

    style->ChildRounding = 4.0f;
    style->FrameBorderSize = 1.0f;
    style->FrameRounding = 2.0f;
    style->GrabMinSize = 7.0f;
    style->PopupRounding = 2.0f;
    style->ScrollbarRounding = 12.0f;
    style->ScrollbarSize = 13.0f;
    style->TabBorderSize = 1.0f;
    style->TabRounding = 0.0f;
    style->WindowRounding = 0.0f;

    //Protracker1Display* display = new Protracker1Display();
    //display->init();

    bgfx::setDebug(BGFX_DEBUG_TEXT);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, ImGui::ColorConvertFloat4ToU32(colors[ImGuiCol_WindowBg]), 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetKeyCallback(window, key_callback);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

    int old_width = 0;
    int old_height = 0;

    ImGui_ImplGlfw_NewFrame();
    create_main_window();

    //glfwGetWindowSize(window, &old_width, &old_height);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int display_w, display_h;
        glfwGetWindowSize(window, &display_w, &display_h);

        if ((old_width != display_w) || (old_height != display_h)) {
            bgfx::reset(display_w, display_h);
            old_width = display_w;
            old_height = display_h;
        }

        bgfx::setViewRect(0, 0, 0, display_w, display_h);
        bgfx::touch(0);

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* main_view = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(main_view->Pos);
        ImGui::SetNextWindowSize(main_view->Size);

        ImGui::Begin("Player", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
        show_main_window();
        ImGui::End();

        // this is all really hacky :)
        float y =  main_view->Pos.y + main_view->Size.y + 24.0f;

        ImGui::SetNextWindowPos(ImVec2(main_view->Pos.x, y));
        ImGui::Begin("Playlist", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
        show_playlist();
        ImGui::End();

        ImGui::Render();

        imguiEndFrame();

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        bgfx::frame();
    }

    bgfx::shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

