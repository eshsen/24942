#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t beep_count = 0;
volatile sig_atomic_t keep_running = 1;

// Обработчик для SIGINT (Ctrl+C)
void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\a", 1); 
    beep_count++;
    printf("\nBeep! (Ctrl+C pressed)\n");
}

// Обработчик для SIGQUIT (Ctrl+\)
void sigquit_handler(int sig) {
    printf("\nTotal beep count: %d\n", beep_count);
    printf("Program terminated by Ctrl+\\n");
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    
    printf("Program started. PID: %d\n", getpid());
    printf("Type text and press Enter to beep.\n");
    printf("Press Ctrl+C for instant beep.\n");
    printf("Press Ctrl+\\ to show statistics and exit.\n");
    
    while(1) {
        int c = getchar();
        
        if (c == EOF) {
            printf("\nBeep count: %d\n", beep_count);
            exit(0);
        }
        
        if (c == '\n') {
            write(STDOUT_FILENO, "\a", 1); 
            beep_count++;
            printf("Beep! (line completed)\n");
        }
        
    }
    
    return 0;
}