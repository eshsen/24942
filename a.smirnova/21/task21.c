#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t keep_running = 1;

void sigint_handler(int sig) {
    signal_count++;
    write(STDOUT_FILENO, "\a", 1);
}

void sigquit_handler(int sig) {
    char buffer[100];
    int len = snprintf(buffer, sizeof(buffer), 
                      "\nПрограмма завершена. Сигнал SIGINT получен %d раз(а).\n", 
                      signal_count);
    write(STDOUT_FILENO, buffer, len);
    keep_running = 0;
}

int main() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(1);
    }
    
    if (signal(SIGQUIT, sigquit_handler) == SIG_ERR) {
        perror("signal SIGQUIT");
        exit(1);
    }
    
    printf("Программа запущена. PID: %d\n", getpid());
    printf("Используйте Ctrl-C для звукового сигнала\n");
    printf("Используйте Ctrl-\\ для вывода статистики и завершения\n");
    printf("Ожидание сигналов...\n");
    
    while (keep_running) {
        pause();
    }
    
    return 0;
}