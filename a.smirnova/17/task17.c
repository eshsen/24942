#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_LINE_LENGTH 40
#define MAX_TEXT_LENGTH 2000
#define BELL '\007'
#define ERASE 0x7F
#define KILL 0x15
#define CTRL_W 0x17
#define CTRL_D 0x04

struct termios original_termios;

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void setup_terminal(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(restore_terminal);
    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
}

struct editor {
    char text[MAX_TEXT_LENGTH];
    int pos;
    int len;
};

void redraw(struct editor *e) {
    printf("\033[2J\033[HInput text (CTRL-D at line start to exit):\n");
    
    int col = 0;
    for (int i = 0; i < e->len; i++) {
        if (col >= MAX_LINE_LENGTH) {
            printf("\n");
            col = 0;
        }
        
        putchar(e->text[i]);
        col++;
        
        if (e->text[i] == ' ' && col >= MAX_LINE_LENGTH - 10 && i + 1 < e->len && e->text[i + 1] != ' ') {
            printf("\n");
            col = 0;
        }
    }
    
    printf("\033[2;1H");
    fflush(stdout);
}

void erase_word(struct editor *e) {
    if (e->pos == 0) {
        putchar(BELL);
        return;
    }
    
    int new_pos = e->pos;
    while (new_pos > 0 && e->text[new_pos - 1] == ' ') new_pos--;
    while (new_pos > 0 && e->text[new_pos - 1] != ' ') new_pos--;
    
    memmove(e->text + new_pos, e->text + e->pos, e->len - e->pos + 1);
    e->len -= (e->pos - new_pos);
    e->pos = new_pos;
}

int main(void) {
    struct editor e = { .pos = 0, .len = 0 };
    char c;
    
    setup_terminal();
    printf("\033[2J\033[HInput text (CTRL-D at line start to exit):\n> ");
    fflush(stdout);
    
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == CTRL_D && e.pos == 0) {
            printf("\nExit.\n");
            break;
        }
        
        switch (c) {
            case ERASE: // Backspace
                if (e.pos > 0) {
                    memmove(e.text + e.pos - 1, e.text + e.pos, e.len - e.pos + 1);
                    e.pos--;
                    e.len--;
                } else {
                    putchar(BELL);
                }
                break;
                
            case KILL: // Ctrl+U
                memmove(e.text, e.text + e.pos, e.len - e.pos + 1);
                e.len -= e.pos;
                e.pos = 0;
                break;
                
            case CTRL_W: // Удалить слово
                erase_word(&e);
                break;
                
            default:
                if (c >= 32 && c <= 126) { // Печатаемые символы
                    if (e.len < MAX_TEXT_LENGTH - 1) {
                        if (e.pos < e.len) {
                            memmove(e.text + e.pos + 1, e.text + e.pos, e.len - e.pos);
                        }
                        e.text[e.pos++] = c;
                        e.text[++e.len] = '\0';
                    } else {
                        putchar(BELL);
                    }
                } else {
                    putchar(BELL);
                }
        }
        
        redraw(&e);
    }
    
    return 0;
}