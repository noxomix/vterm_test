#define GLAD_GL_IMPLEMENTATION
#include "gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
//#include "render.h"
#define RFONT_IMPLEMENTATION
#include "RFont.h"
#include <vterm.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pty.h>
#include <unistd.h>

//custom
#include "term_parts/fps_counter.h"
#include "term_parts/application.h"
#include "term_parts/pty_manager.h"
#include "term_parts/terminal.h"


int damage(VTermRect rect, void *user) {
    //printf("damage: [%d, %d, %d, %d]\n", rect.start_col,
//			rect.start_row, rect.end_col, rect.end_row);
    return 1;
}
int moverect(VTermRect dest, VTermRect src, void *user) {
    return 1;
}
int movecursor(VTermPos pos, VTermPos oldpos, int visible, void *user) {
    return 1;
}
int settermprop(VTermProp prop, VTermValue *val, void *user) {
    return 1;
}
/*int bell(void *user) {
    TERM_State *state = (TERM_State*)user;
    state->bell.active = true;
    state->bell.ticks = state->ticks;
    return 1;
}*/

int sb_pushline(int cols, const VTermScreenCell *cells, void *user) {
    printf("SB Pushline\n");
    return 1;
}
int sb_popline(int cols, VTermScreenCell *cells, void *user) {
    return 1;
}

VTermScreenCallbacks callbacks = {
        .movecursor = movecursor,
        .sb_pushline = sb_pushline,
        .damage = damage
};

// Die Funktion, die aufgerufen wird, wenn ein Tastenereignis auftritt
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
    GLFWwindow* window;
    // Initialisiere die GLFW-Bibliothek
    if (!glfwInit()) {
        fprintf(stderr, "Fehler beim Initialisieren von GLFW\n");
        return -1;
    }


    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    window = glfwCreateWindow(900, 600, "Mein erstes GLFW-Fenster", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Fehler beim Erstellen des Fensters\n");
        glfwTerminate();
        return -1;
    }

    // Setze den Tastenrückruf
    //glfwSetKeyCallback(window, key_callback);

    // Mache das neu erstellte Fenster zum aktuellen Kontext
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    Application app;
    glfwGetWindowSize(window, &app.width, &app.height);

    //RFont init
    RFont_init(app.width, app.height);
    app.font = *RFont_font_init("NotoSansMono-Regular.ttf");
    //app.font = *RFont_font_init("JetBrainsMono-Regular.ttf");
    //RFont_font* japanese = RFont_font_init("DroidSansJapanese.ttf");

    app.font_height = 24;
    //draw text as test
    app.font_width = (int) RFont_text_width_len(&app.font, "\\u00A0", 1, app.font_height, 0);
    printf("%i\n", app.font_width);

    app.cols =  app.width / app.font_width;
    app.rows =  app.height / app.font_height;
    printf( " Row spacing %i\n", app.height % app.font_height);

    if(create_pty_fork(&app) < 0) {
        printf("error\n");
        exit(0);
    }

    app.vterm = create_vterm(app.rows, app.cols);
    app.screen = vterm_obtain_screen(app.vterm);
    vterm_screen_reset(app.screen, 1);
    app.termstate = vterm_obtain_state(app.vterm);
    vterm_screen_set_callbacks(app.screen, &callbacks, &app);

    struct winsize ws = {0};
    ws.ws_col = app.cols;
    ws.ws_row = app.rows;
    ioctl(app.childfd, TIOCSWINSZ, &ws);


    // Schleife, um das Fenster offen zu halten, bis es geschlossen wird
    glViewport(0, 0, app.width, app.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Hintergrundfarbe setzen (schwarz)
    glOrtho(0, app.width, app.height, 0, 1,-1);


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        TERM_HandleChildEvents(&app);
        render_screen(&app);

        fps_counter(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Aufräumen
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
