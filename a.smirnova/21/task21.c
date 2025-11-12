#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t beep_count = 0;
volatile sig_atomic_t need_exit = 0;

// Обработчик для SIGINT (Ctrl+C)
void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\a", 1); 
    beep_count++;
    printf("\nBeep! (Ctrl+C pressed)\n");
    printf("> ");
    fflush(stdout);
}

// Обработчик для SIGQUIT (Ctrl+\)
void sigquit_handler(int sig) {
    printf("\nTotal beep count: %d\n", beep_count);
    printf("Program terminated by Ctrl+\\\n");
    need_exit = 1;
}

int main() {
    // Устанавливаем обработчики сигналов
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    
    printf("Program started. PID: %d\n", getpid());
    printf("Type text and press Enter to beep.\n");
    printf("Press Ctrl+C for instant beep.\n");
    printf("Press Ctrl+\\ to show statistics and exit.\n");
    printf("> ");
    
    while(!need_exit) {
        int c = getchar();
        
        if (need_exit) {
            break;
        }
        
        if (c == EOF) {
            // Если getchar был прерван сигналом, продолжаем
            clearerr(stdin);
            continue;
        }
        
        if (c == '\n') {
            write(STDOUT_FILENO, "\a", 1); 
            beep_count++;
            printf("Beep! (line completed)\n");
            printf("> ");
        }
    }
    
    return 0;
}