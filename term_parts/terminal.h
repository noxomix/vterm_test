#include <vterm.h>
#include <stdint.h>

#define MAX_UTF8_BYTES 8
void uint32ToUTF8(uint32_t codepoint, char utf8[]) {
    if (codepoint <= 0x7F) {
        utf8[0] = (char)codepoint;
        utf8[1] = '\0';
    } else if (codepoint <= 0x7FF) {
        utf8[0] = (char)((codepoint >> 6) | 0xC0);
        utf8[1] = (char)((codepoint & 0x3F) | 0x80);
        utf8[2] = '\0';
    } else if (codepoint <= 0xFFFF) {
        utf8[0] = (char)((codepoint >> 12) | 0xE0);
        utf8[1] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        utf8[2] = (char)((codepoint & 0x3F) | 0x80);
        utf8[3] = '\0';
    } else if (codepoint <= 0x10FFFF) {
        utf8[0] = (char)((codepoint >> 18) | 0xF0);
        utf8[1] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
        utf8[2] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        utf8[3] = (char)((codepoint & 0x3F) | 0x80);
        utf8[4] = '\0';
    } else {
        printf("Ungültiger Codepoint\n");
        utf8[0] = '\0';
    }
}

VTerm *create_vterm(int rows, int cols) {
    return vterm_new(rows, cols);
}

void render_cell(Application *app, int x, int y) {
    VTermScreenCell cell;
    VTermPos pos = {.row = y, .col = x};
    vterm_screen_get_cell(app->screen, pos, &cell);
    uint32_t ch = cell.chars[0];
    if(ch == 0) {
        //printf("Error - render_cell\n");
        return;
    }

    vterm_state_convert_color_to_rgb(app->termstate, &cell.fg);
    vterm_state_convert_color_to_rgb(app->termstate, &cell.bg);

    unsigned char r = cell.fg.rgb.red * 255;
    unsigned char g = cell.fg.rgb.green * 255;
    unsigned char b = cell.fg.rgb.blue * 255;

    int xcord = x*app->font_width;
    int ycord = y*app->font_height;

    r = ~r - 255;
    g = ~g - 255;
    b = ~b - 255;

    if(cell.attrs.reverse) {
        glColor3f((float)r/255.0f, (float)g/255.0f, (float)b/255.0f);
        //glRecti(xcord+app->font_width/2, ycord-app->font_height/2, xcord-app->font_width/2, ycord+app->font_height/2);
        glRectf(x*app->font_width+app->font_width, y*app->font_height+(app->font_height/4), x*app->font_width, y*app->font_height+app->font_height+(app->font_height/4));
        r = ~r;
        g = ~g;
        b = ~b;
    }


    RFont_set_color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, 1.0f);
    /* Outdated part in favor to support more the 255 utf-8 chars :D
    char chr[1] = {(char)ch};
    if(chr[1] == 0x20) {
        chr[1] = (char)160;
    }*/

    char chr[5]; // Platz für ein UTF-8-Zeichen

    uint32ToUTF8(ch, chr);

    RFont_draw_text_spacing(&app->font, chr, (float)xcord, (float)ycord, app->font_height, 0);
}

void render_screen(Application *app) {
    for(int y = 0; y < app->rows; y++) {
        char c = 0;
        for(int x = 0; x < app->cols; x++) {
            render_cell(app, x, y);
            char chr[1] = {c + '0'};
            if(c < 9) {
                c++;
            } else {
                c = 0;
                glColor3f(0.5, 0.2,0.3);
                //glRectf(x*app->font_width+app->font_width, y*app->font_height, x*app->font_width, y*app->font_height+app->font_height);
            }
            //RFont_draw_text_spacing(&app->font, chr, (float)x*app->font_width, (float)y*app->font_height, app->font_height, 0);
        }
    }
}