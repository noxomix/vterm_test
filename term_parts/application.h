#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//#define RFONT_IMPLEMENTATION
//#include "../RFont.h"
#include <vterm.h>

typedef struct {
    int width;
    int height;
    int rows;
    int cols;
    int font_height;
    int font_width;
    RFont_font font;

    GLFWwindow *window;
    RFont_font *font_regular;
    RFont_font *font_bold;

    VTerm *vterm;
    VTermScreen *screen;
    VTermState *termstate;

    pid_t child;
    int childfd;
} Application;

static void TERM_HandleChildEvents(Application *state) {
    fd_set rfds;
    struct timeval tv = {0};

    FD_ZERO(&rfds);
    FD_SET(state->childfd, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 50000;

    if(select(state->childfd+1, &rfds, NULL, NULL, &tv) > 0) {
        char line[256];
        int n;
        if((n = read(state->childfd, line, sizeof(line))) > 0) {
            vterm_input_write(state->vterm, line, n);
            vterm_screen_flush_damage(state->screen);
        }
    }
}