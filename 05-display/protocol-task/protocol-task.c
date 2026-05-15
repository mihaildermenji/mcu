#include "protocol-task.h"
#include <stdio.h>
#include <string.h>

static api_t* api = {0};
static int commands_count = 0;

void protocol_task_init(api_t* device_api) {
    api = device_api;
    commands_count = 0;

    if (api != NULL) {
        while (api[commands_count].command_name != NULL) {
            commands_count++;
        }
    }
}

void protocol_task_handle(char* command_string){
    if (!command_string) return;

        const char* command_name = command_string;
        const char* command_args = NULL;

        char* space_symbol = strchr(command_string, ' ');
        /* функция char *strchr(const char *str, int c) принимает указатель на строку и c символ,
         принимает формально int, но переводит в char. Функция ищит первое вхождение указаного символа и возращает указатель на него. Если симвл не найден, то возрощает NULL
         Теперь если в строке есть пробел, то space_symbol указывает на него, если нет то он NULL*/
                if (space_symbol) // Если пробел найден, то
        {
            *space_symbol = '\0';
            command_args = space_symbol + 1; // меняет указатель с пробела на следующий элемент
        }
        else
        {
            command_args = "";
        }
        printf("Command: '%s', Arguments: '%s'\n", command_name, command_args);
        int found= 0;

        for (int i=0; api[i].command_name != NULL; i++){
            if (!strcmp(command_name, api[i].command_name)){ // Проверяет две строки на совпадение, при совпадении возращает 0
                api[i].command_callback(command_args);
                found = 1;
                break;
            }
        }
        if (found == 0) {
        printf("Error: unknown command '%s'\n", command_name);
    }
}

