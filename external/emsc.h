#pragma once
/* common emscripten platform helper functions */
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>


#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

static const char* _emsc_canvas_name = 0;
static int _emsc_sample_count = 0;
static double _emsc_width = 0;
static double _emsc_height = 0;
static GLint _emsc_framebuffer = 0;

enum {
    EMSC_NONE = 0,
    EMSC_ANTIALIAS = (1<<1)
};

/* track CSS element size changes and update the WebGL canvas size */
static EM_BOOL _emsc_size_changed(int event_type, const EmscriptenUiEvent* ui_event, void* user_data) {
    (void)event_type;
    (void)ui_event;
    (void)user_data;
    emscripten_get_element_css_size(_emsc_canvas_name, &_emsc_width, &_emsc_height);
    emscripten_set_canvas_element_size(_emsc_canvas_name, _emsc_width, _emsc_height);
    return true;
}

/* initialize WebGL context and canvas */
void emsc_init(const char* canvas_name, int flags, int init_width = 0, int init_height = 0) {
    _emsc_canvas_name = canvas_name;
    if (init_width == 0 && init_height == 0)
    {
        emscripten_get_element_css_size(canvas_name, &_emsc_width, &_emsc_height);
    }
    else
    {
        _emsc_width = init_width;
        _emsc_height = init_height;
    }
    emscripten_set_canvas_element_size(canvas_name, _emsc_width, _emsc_height);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false, _emsc_size_changed);
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.antialias = flags & EMSC_ANTIALIAS;
    attrs.majorVersion = 2;
    _emsc_sample_count = (flags & EMSC_ANTIALIAS) ? 4 : 1;
    ctx = emscripten_webgl_create_context(canvas_name, &attrs);
    emscripten_webgl_make_context_current(ctx);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_emsc_framebuffer);
}

int emsc_width(void) {
    return (int) _emsc_width;
}

int emsc_height(void) {
    return (int) _emsc_height;
}

sg_environment emsc_environment(void) {
    return (sg_environment){
        .defaults = {
            .color_format = SG_PIXELFORMAT_RGBA8,
            .depth_format = SG_PIXELFORMAT_DEPTH_STENCIL,
            .sample_count = _emsc_sample_count,
        }
    };
}

sg_swapchain emsc_swapchain(void) {
    return (sg_swapchain) {
        .width = (int)_emsc_width,
        .height = (int)_emsc_height,
        .sample_count = _emsc_sample_count,
        .color_format = SG_PIXELFORMAT_RGBA8,
        .depth_format = SG_PIXELFORMAT_DEPTH_STENCIL,
        .gl = {
            .framebuffer = (uint32_t)_emsc_framebuffer,
        }
    };
}