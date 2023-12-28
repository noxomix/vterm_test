#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

static int fps_frames = 0;
static double fps_t, fps_t0, fps_fps;
static char fps_title_string[10];

void fps_counter(GLFWwindow *window) {
    fps_t = glfwGetTime();

    if((fps_t - fps_t0) > 1.0 || fps_frames == 0)
    {
        fps_fps = (double)fps_frames / (fps_t - fps_t0);
        sprintf(fps_title_string, "FPS: %.1f", fps_fps);
        glfwSetWindowTitle(window, fps_title_string);
        fps_t0 = fps_t;
        fps_frames = 0;
    }
    fps_frames ++;
}