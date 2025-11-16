#include "cstack.h"
#include <stdlib.h> //для функций работы с памятью 

#define UNUSED(VAR) (void)(VAR)

// Структура элемета  стека
struct stack_element {
    struct *stack_element prev; //указатель на предыдущий элемент 
    unsigned int size; //размер данных, хранящихся в текущем элементе
    char data[0];  // данные 
};

// Структура для записи о стеке в таблице
struct stack_entry {
    int reserved;       // 1 если запись занята, 0 если свободна
    struct node* stack; //указатель на последний добавленный элемент
};

// Глобальная таблица стеков
struct stack_entries_table {
    unsigned int size;           //текущий размер таблицы
    struct stack_entry* entries; 
};

static struct stack_entries_table g_table = {0u, NULL};

hstack_t stack_new(void)
{
    return -1;
}

void stack_free(const hstack_t hstack)
{
    UNUSED(hstack);
}

int stack_valid_handler(const hstack_t hstack)
{
    UNUSED(hstack);
    return 1;
}

unsigned int stack_size(const hstack_t hstack)
{
    UNUSED(hstack);
    return 0;
}

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    UNUSED(hstack);
    UNUSED(data_in);
    UNUSED(size);
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    UNUSED(hstack);
    UNUSED(data_out);
    UNUSED(size);
    return 0;
}

