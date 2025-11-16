#include "cstack.h"
#include <stdlib.h> //для функций работы с памятью 
#include <string.h>  // для memcpy

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

// Вспомогательные функции для работы с таблицей стеков
static int expand_table_if_needed(void);
static int find_free_slot(void);

//Функция для нахождения первого свободного слота в таблице стеков
static int find_free_slot(void) {
    for (unsigned int i = 0; i < g_table.size; ++i) {
        if (g_table.entries[i].reserved == 0) {
            return (int)i;  // Нашли свободный слот
        }
    }
    return -1;  // Все слоты заняты
}

//Удваивает размер таблицы когда все слоты заняты
static int expand_table_if_needed(void) {
    unsigned int new_size = g_table.size * 2;
    struct stack_entry* new_entries = (struct stack_entry*)realloc(
        g_table.entries, new_size * sizeof(struct stack_entry));
    
    if (new_entries == NULL) {
        return 0;
    }
    
    // Инициализируем новую часть таблицы
    for (unsigned int i = g_table.size; i < new_size; ++i) {
        new_entries[i].reserved = 0;
        new_entries[i].top = NULL;
    }
    
    g_table.entries = new_entries;
    g_table.size = new_size;
    return 1;
}


//Cоздание нового стека
hstack_t stack_new(void) {
    // Инициализируем таблицу при первом вызове
    if (g_table.entries == NULL) {
        g_table.size = 10;
        g_table.entries = (struct stack_entry*)calloc(g_table.size, sizeof(struct stack_entry));
        if (g_table.entries == NULL) {
            return -1;
        }
    }
    
    // Находим свободный слот
    int slot = find_free_slot();
    if (slot == -1) {
        // Пытаемся расширить таблицу
        if (!expand_table_if_needed()) {
            return -1;
        }
        slot = find_free_slot();
        if (slot == -1) {
            return -1;
        }
    }
    
    // Помечаем слот как занятый
    g_table.entries[slot].reserved = 1;
    g_table.entries[slot].top = NULL;
    
    return (hstack_t)slot;
}

void stack_free(const hstack_t hstack) {
    if (!stack_valid_handler(hstack)) {
        return;
    }
    
    int index = (int)hstack;
    
    // Освобождаем все элементы стека
    struct stack_item* current = g_table.entries[index].top;
    while (current != NULL) {
        struct stack_item* prev = current->previous;
        free(current);
        current = prev;
    }
    
    // Освобождаем запись в таблице
    g_table.entries[index].reserved = 0;
    g_table.entries[index].top = NULL;
}


//Проверка хэндлера
int stack_valid_handler(const hstack_t hstack) {
    if (hstack < 0) {
        return 0;
    }
    
    int index = (int)hstack;
    
    if (g_table.entries == NULL || index >= (int)g_table.size) {
        return 0;
    }
    
    return g_table.entries[index].reserved == 1;
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

