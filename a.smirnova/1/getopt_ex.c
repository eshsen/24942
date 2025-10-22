#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_OPTIONS 50

typedef struct {
    char opt;
    char *arg;
} Option;

int main(int argc, char *argv[]) {
    int i;
    int opt;
    // Массив для хранения опций в порядке появления
    Option options[MAX_OPTIONS];
    int options_count = 0;

    // Обработка аргументов командной строки, собираем опции
    while ((opt = getopt(argc, argv, "isp u:: U:C:dvV:")) != -1) {
        if (options_count >= MAX_OPTIONS) {
            fprintf(stderr, "Превышен лимит опций\n");
            exit(EXIT_FAILURE);
        }
        options[options_count].opt = opt;
        options[options_count].arg = NULL;

        // Для опций с аргументом их запоминаем
        if (opt == 'U' || opt == 'C' || opt == 'V') {
            options[options_count].arg = strdup(optarg);
        }
        options_count++;
    }

    // Обработка оставшихся аргументов после опций (если есть)
    for (i = optind; i < argc; i++) {
        // Можно обработать как параметры или игнорировать
        // В данной задаче это не требуется
    }

    // Обработка опций в порядке их появления (слева направо)
    printf("Обработка опций в порядке появления:\n");
    for (i = 0; i < options_count; i++) {
        char opt_char = options[i].opt;
        switch (opt_char) {
            case 'i':
            {
                printf("Real UID: %d\n", getuid());
                printf("Effective UID: %d\n", geteuid());
                printf("Real GID: %d\n", getgid());
                printf("Effective GID: %d\n", getegid());
                break;
            }
            case 's':
            {
                pid_t pid = getpid();
                pid_t pgrp = getpgrp();
                printf("Process PID: %d, PGID: %d\n", pid, pgrp);
                // меняем группу процесса на текущий
                if (setpgid(0,0) == -1) {
                    perror("setpgid");
                } else {
                    printf("Группа процесса изменена.\n");
                }
                break;
            }
            case 'p':
            {
                printf("PID: %d, PPID: %d, PGID: %d\n", getpid(), getppid(), getpgid(0));
                break;
            }
            case 'u':
            {
                struct rlimit lim;
                if (getrlimit(RLIMIT_CORE, &lim) == -1) {
                    perror("getrlimit");
                } else {
                    printf("Core dump size (по умолчанию): %ld bytes\n", lim.rlim_cur);
                }
                break;
            }
            case 'U':
            {
                // Изменяет ulimit (RLIMIT_CORE)
                if (options[i].arg) {
                    long new_lim = strtol(options[i].arg, NULL, 10);
                    struct rlimit lim;
                    if (getrlimit(RLIMIT_CORE, &lim) == -1) {
                        perror("getrlimit");
                        break;
                    }
                    lim.rlim_cur = new_lim;
                    if (setrlimit(RLIMIT_CORE, &lim) == -1) {
                        perror("setrlimit");
                    } else {
                        printf("uLimit установлен в %ld\n", new_lim);
                    }
                }
                break;
            }
            case 'c':
            {
                // Размер core файла
                struct rlimit lim;
                if (getrlimit(RLIMIT_CORE, &lim) == -1) {
                    perror("getrlimit");
                } else {
                    printf("Max core file size: %ld bytes\n", lim.rlim_cur);
                }
                break;
            }
            case 'C':
            {
                // Установка размера core-файла
                if (options[i].arg) {
                    long new_size = strtol(options[i].arg, NULL, 10);
                    struct rlimit lim;
                    if (getrlimit(RLIMIT_CORE, &lim) == -1) {
                        perror("getrlimit");
                        break;
                    }
                    lim.rlim_cur = new_size;
                    if (setrlimit(RLIMIT_CORE, &lim) == -1) {
                        perror("setrlimit");
                    } else {
                        printf("Размер core-файла установлен в %ld\n", new_size);
                    }
                }
                break;
            }
            case 'd':
            {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) == NULL) {
                    perror("getcwd");
                } else {
                    printf("Текущая директория: %s\n", cwd);
                }
                break;
            }
            case 'v':
            {
                // Выводит все переменные среды
                extern char **environ;
                for (char **env = environ; *env != NULL; env++) {
                    printf("%s\n", *env);
                }
                break;
            }
            case 'V':
            {
                // Установка переменной среды
                if (options[i].arg) {
                    char *eq_pos = strchr(options[i].arg, '=');
                    if (eq_pos) {
                        size_t name_len = eq_pos - options[i].arg;
                        char var_name[name_len + 1];
                        strncpy(var_name, options[i].arg, name_len);
                        var_name[name_len] = '\0';
                        char *value = eq_pos + 1;
                        if (setenv(var_name, value, 1) == -1) {
                            perror("setenv");
                        } else {
                            printf("Добавлено/обновлено %s=%s\n", var_name, value);
                        }
                    } else {
                        fprintf(stderr, "Некорректный формат -V: %s\n", options[i].arg);
                    }
                }
                break;
            }
            default:
                fprintf(stderr, "Неизвестная опция -%c\n", opt_char);
        }
    }

    // Освобождение памяти
    for (i = 0; i < options_count; i++) {
        if (options[i].arg) free(options[i].arg);
    }

    return 0;
}