#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui_bgfx/imgui.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

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

    printf("created new frame buffer %f %f - %d - native handle %p\n", vp->Size.x, vp->Size.y, handle.idx, native_handle);

    vp->RendererUserData = (void*)(uintptr_t)handle.idx;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// N . U . D  // Destroy swap chain, frame buffers etc. (called before Platform_DestroyWindow)

static void renderer_destroy_window(ImGuiViewport* vp) {
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)vp->PlatformUserData;

    if (!data->window_owned) {
        return;
    }

    printf("%s:%d\n", __FILE__, __LINE__);

    bgfx::FrameBufferHandle handle = { (uint16_t)(uintptr_t)vp->RendererUserData };

    bgfx::destroy(handle);

    // Flush destruction of swap chain before destroying window!
    bgfx::frame();
    bgfx::frame();
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

    imguiRenderDraws(vp->DrawData, 1);

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
    //glfwWindowHint(GLFW_DECORATED, false);
    //glfwWindowHint(GLFW_RESIZABLE, true);

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

    printf("first native handle %p\n", pd.nwh);

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
    io.ConfigViewportsNoDecoration = true;
    ImGui::GetStyle().WindowRounding = 0.0f;

#if defined(GLFW_EXPOSE_NATIVE_X11)
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplGlfw_InitForOther(window, true);
#endif

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

    {
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
    }

    //Protracker1Display* display = new Protracker1Display();
    //display->init();

    bgfx::setDebug(BGFX_DEBUG_TEXT);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetKeyCallback(window, key_callback);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int display_w, display_h;
        glfwGetWindowSize(window, &display_w, &display_h);

        bgfx::setViewRect(0, 0, 0, display_w, display_h);
        bgfx::touch(0);

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;

            //ImVec2 pos = ImVec2(0.0f, 0.0f);
            //ImVec2 size = ImVec2((float)display_w, (float)display_h);

            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));
            //ImGui::SetNextWindowSizeConstraints(pos, size);

            //ImGui::Begin("HippoPlayer", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
            ImGui::Begin("HippoPlayer Window", nullptr);
            ImGui::ProgressBar(0.5f);
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("Play")) {
                printf("play!\n");
            }
            ImGui::End();

            ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));


            ImGui::Begin("Playlist", nullptr, ImGuiWindowFlags_NoTitleBar);
            ImGui::ProgressBar(0.5f);
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::End();

            /*
               static float f = 0.0f;
               static int counter = 0;

               ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

               ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            //ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
             */
        }

        ImGui::Render();

        imguiEndFrame();

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            //GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            //glfwMakeContextCurrent(backup_current_context);
        }

        //display->render(s_width, s_height);
        //display->m_display->set_playing_row(0);

        bgfx::frame();

    }

    bgfx::shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#if 0

#include <bgfx/bgfx.h>
//#include "core/log.h"
//#include "core/file.h"
//#include "core/core.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))

#if defined(__clang__) || defined(__gcc__)
#define PD_NO_RETURN __attribute__((noreturn))
#else
#define PD_NO_RETURN
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* File_loadToMemory(const char* filename, size_t* size, size_t padAllocSize) {
    FILE* f = fopen(filename, "rb");
    void* data = 0;
    size_t s = 0, t = 0;

    *size = 0;

    if (!f)
        return 0;

    // TODO: Use fstat here?

    fseek(f, 0, SEEK_END);
    long ts = ftell(f);

    if (ts < 0)
        goto end;

    s = (size_t)ts;

    data = malloc(s + padAllocSize);

    if (!data)
        goto end;

    fseek(f, 0, SEEK_SET);

    t = fread(data, s, 1, f);
    (void)t;

    *size = s;

end:

    fclose(f);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bgfx::UniformHandle s_tex;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ProgramAttribs {
    bgfx::Attrib::Enum attrib;
    uint8_t num;
    bgfx::AttribType::Enum type;
    bool norm;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ProgramInfo {
    ProgramAttribs* attribs;
    const char* vsName;
    const char* fsName;
    bgfx::VertexDecl vertexDecl;
    bgfx::ProgramHandle handle;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const bgfx::Memory* loadShader(const char* filename) {
    size_t size;
    uint8_t* data = (uint8_t*)File_loadToMemory(filename, &size, 1);

    if (!data) {
        printf("Unable to load shader %s\n", filename);
        return 0;
    }

    const bgfx::Memory* mem = bgfx::makeRef(data, size + 1, NULL, NULL);

    // terminate strings
    data[size] = 0;

    return mem;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bgfx::ProgramHandle loadProgram(const char* vsName, const char* fsName) {
    bgfx::ProgramHandle ph = { bgfx::invalidHandle };

    const bgfx::Memory* vsShader = loadShader(vsName);
    const bgfx::Memory* fsShader = loadShader(fsName);

    if (!vsShader)
        return ph;

    if (!fsShader)
        return ph;

    bgfx::ShaderHandle vsHandle = bgfx::createShader(vsShader);
    bgfx::ShaderHandle fsHandle = bgfx::createShader(fsShader);

    if (!isValid(vsHandle)) {
        printf("Unable to load vsShader %s\n", vsName);
        return ph;
    }

    if (!isValid(fsHandle)) {
        printf("Unable to load fsShader %s\n", fsName);
        return ph;
    }

    ph = bgfx::createProgram(vsHandle, fsHandle, true);

    if (!isValid(ph))
        printf("Unable to create shader program for %s %s\n", vsName, fsName);

    return ph;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ProgramAttribs posTexColorAttribs[] =
{
    { bgfx::Attrib::Position, 2, bgfx::AttribType::Float, false },
    { bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, false },
    { bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true },
    { bgfx::Attrib::Count, 0, bgfx::AttribType::Uint8, false },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ProgramAttribs posColorAttribs[] =
{
    { bgfx::Attrib::Position, 2, bgfx::AttribType::Float, false },
    { bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true },
    { bgfx::Attrib::Count, 0, bgfx::AttribType::Uint8, false },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ProgramInfo s_programs[] =
{
    {
        (ProgramAttribs*)&posColorAttribs,
        OBJECT_DIR "/_generated/data/shaders/ui_pos_color/vs_pos_color.vs",
        OBJECT_DIR "/_generated/data/shaders/ui_pos_color/fs_pos_color.fs",
    },

    {
        (ProgramAttribs*)&posTexColorAttribs,
        OBJECT_DIR "/_generated/data/shaders/ui_pos_tex_r_color/vs_pos_tex_r_color.vs",
        OBJECT_DIR "/_generated/data/shaders/ui_pos_tex_r_color/fs_pos_tex_r_color.fs",
    },

    {
        (ProgramAttribs*)&posTexColorAttribs,
        OBJECT_DIR "/_generated/data/shaders/imgui/vs_imgui.vs",
        OBJECT_DIR "/_generated/data/shaders/imgui/fs_imgui.fs",
    },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum {
    Program_PosColor,
    Program_PosTexRColor,
    Program_PosTexColor,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UIRender_init() {
    s_tex = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);

    for (int i = 0; i < (int)sizeof_array(s_programs); ++i) {
        ProgramInfo* program = &s_programs[i];

        program->handle = loadProgram(program->vsName, program->fsName);

        if (!isValid(program->handle))
            return false;

        const ProgramAttribs* attribs = program->attribs;
        bgfx::VertexDecl& decl = program->vertexDecl;

        decl.begin();

        while (attribs->attrib != bgfx::Attrib::Count) {
            decl = decl.add(attribs->attrib, attribs->num, attribs->type, attribs->norm);
            attribs++;
        }

        decl.end();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UIRender_allocPosTexColorTb(bgfx::TransientVertexBuffer* buffer, uint32_t count) {
    assert(bgfx::checkAvailTransientVertexBuffer(count, s_programs[Program_PosTexColor].vertexDecl));
    bgfx::allocTransientVertexBuffer(buffer, count, s_programs[Program_PosTexColor].vertexDecl);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UIRender_allocPosColorTb(bgfx::TransientVertexBuffer* buffer, uint32_t count) {
    assert(bgfx::checkAvailTransientVertexBuffer(count, s_programs[Program_PosColor].vertexDecl));
    bgfx::allocTransientVertexBuffer(buffer, count, s_programs[Program_PosColor].vertexDecl);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UIRender_posTexColor(bgfx::TransientVertexBuffer* vertexBuffer, uint32_t offset, uint32_t count, bgfx::TextureHandle texHandle) {
    bgfx::setTexture(0, s_tex, texHandle);
    bgfx::setVertexBuffer(vertexBuffer, offset, count);
    //NOTE(marco): the program handle is now part of bgfx::submit
    //bgfx::setProgram(s_programs[Program_PosTexColor].handle);
    bgfx::submit(0, s_programs[Program_PosTexColor].handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UIRender_posIdxTexColor(bgfx::TransientVertexBuffer* vertexBuffer, bgfx::TransientIndexBuffer* indexBuffer, uint32_t vtxSize, uint32_t offset, uint32_t count, bgfx::TextureHandle texHandle) {
    bgfx::setTexture(0, s_tex, texHandle);
    bgfx::setVertexBuffer(vertexBuffer, 0, vtxSize);
    bgfx::setIndexBuffer(indexBuffer, offset, count);
    //NOTE(marco): the program handle is now part of bgfx::submit
    //bgfx::setProgram(s_programs[Program_PosTexColor].handle);
    bgfx::submit(0, s_programs[Program_PosTexColor].handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UIRender_posTexRColor(bgfx::TransientVertexBuffer* vertexBuffer, uint32_t offset, uint32_t count, bgfx::TextureHandle texHandle) {
    bgfx::setTexture(0, s_tex, texHandle);
    bgfx::setVertexBuffer(vertexBuffer, offset, count);
    //NOTE(marco): the program handle is now part of bgfx::submit
    //bgfx::setProgram(s_programs[Program_PosTexRColor].handle);
    bgfx::submit(0, s_programs[Program_PosTexRColor].handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UIRender_posColor(bgfx::TransientVertexBuffer* vertexBuffer, uint32_t offset, uint32_t count) {
    bgfx::setVertexBuffer(vertexBuffer, offset, count);
    //NOTE(marco): the program handle is now part of bgfx::submit
    //bgfx::setProgram(s_programs[Program_PosColor].handle);
    bgfx::submit(0, s_programs[Program_PosColor].handle);
}

bgfx::ProgramHandle UIRender_getProgramHandle(uint32_t programIndex) {
    return s_programs[2].handle;
}

#endif

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>


/*
#include "pattern_display/protracker_1_dispay.h"

// settings
const unsigned int SCR_WIDTH = 320 * 4;
const unsigned int SCR_HEIGHT = 200 * 4;

int current_row = 63;
bool repeated = true;

static int s_size[2] = { SCR_WIDTH, SCR_HEIGHT };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
// glfw: initialize and configure
// ------------------------------
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

// glfw window creation
// --------------------
GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Black Tracker", NULL, NULL);
if (window == NULL) {
printf("Failed to create GLFW window\n");
glfwTerminate();
return -1;
}
glfwMakeContextCurrent(window);
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

// glad: load all OpenGL function pointers
// ---------------------------------------
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
printf("Failed to create initialize GLAD\n");
return -1;
}

Protracker1Display* display = new Protracker1Display();
display->init();

// render loop
// -----------
while (!glfwWindowShouldClose(window)) {
// input
// -----
processInput(window);

// render
// ------
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);

display->render(s_size[0], s_size[1]);
display->m_display->set_playing_row(current_row);

// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
// -------------------------------------------------------------------------------
glfwSwapBuffers(window);
glfwPollEvents();
}

// ------------------------------------------------------------------
glfwTerminate();
return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && !repeated) {
        current_row++;
        current_row &= 63;
        repeated = true;
    }

    if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)) {
        repeated = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    s_size[0] = width;
    s_size[1] = height;

    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
*/

