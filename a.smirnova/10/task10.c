#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char*argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <команда> [аргументы...]\n", argv[0]);
        exit(1);
    }

    printf("Родительский процесс: PID = %d\n", getpid());
    printf("Запускаю команду: %s", argv[1]);

    // выводим все аргументы для команды
    for (int i = 2; i < argc; i++) {
        printf(" %s", argv[i]);
    }
    printf("\n");

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        printf("Дочерний процесс: PID = %d\n", getpid());
        char **cmd_args = malloc((argc) * sizeof(char*));
        cmd_args[0] = argv[1]; // первый аргумент - имя команды
        // копируем остальные аргументы
        for (int i = 2; i < argc; i++) {
            cmd_args[i-1] = argv[i];
        }
        cmd_args[argc-1] = NULL; // последний аргумент NULL
        execvp(argv[1], cmd_args); // заменяем программу дочернего процесса на указанную команду 
        perror("execvp failed");
        free(cmd_args);
        exit(1);
    } else {
        // ждем завершения дочернего процесса
        printf("Родительский процесс: жду завершения дочернего процесса\n");
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Родительский процесс: дочерний процесс завершился нормально\n");
        } else if (WIFSIGNALED(status)) {
            printf("Родительский процесс: дочерний процесс завершился с ошибкой\n");
        } else {
            printf("Родительский процесс: дочерний процесс завершился ненормально\n");
        }
    }
    return 0;
}