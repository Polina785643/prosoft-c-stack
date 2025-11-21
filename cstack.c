#include "cstack.h"
#include <stdlib.h> // для функций работы с памятью: malloc, free, calloc, realloc
#include <string.h>  // для memcpy

// Структура элемента стека
struct stack_item {
    struct stack_item* prev; // указатель на предыдущий элемент в стеке
    unsigned int size;       // размер данных, хранящихся в этом элементе
    char data[0];           // гибкий массив для хранения данных пользователя
};

// Структура для записи о стеке в таблице
struct stack_entry {
    int reserved;           // флаг: 1 если запись занята, 0 если свободна
    struct stack_item* top; // указатель на вершину стека (последний добавленный элемент)
};

// Глобальная таблица стеков
struct stack_entries_table {
    unsigned int size;           // текущий размер таблицы (количество слотов)
    struct stack_entry* entries; // динамический массив записей о стеках
};

static struct stack_entries_table g_table = {0u, NULL}; // инициализация глобальной таблицы

// Вспомогательные функции для работы с таблицей стеков
static int expand_table_if_needed(void);
static int find_free_slot(void);

// Находит первый свободный слот в таблице стеков
// Возвращает индекс свободного слота или -1 если свободных слотов нет
static int find_free_slot(void) {
    for (unsigned int i = 0; i < g_table.size; ++i) {
        if (g_table.entries[i].reserved == 0) {
            return (int)i;
        }
    }
    return -1;
}

// Удваивает размер таблицы когда все слоты заняты
// Возвращает 1 при успешном расширении, 0 при ошибке
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

// Создает новый стек и возвращает его хэндлер
// Возвращает -1 в случае ошибки, неотрицательный хэндлер при успехе
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
    
    // Помечаем слот как занятый и инициализируем стек
    g_table.entries[slot].reserved = 1;
    g_table.entries[slot].top = NULL;
    
    return (hstack_t)slot;
}

// Удаляет стек и освобождает всю связанную с ним память
// Если хэндлер невалиден, функция ничего не делает
void stack_free(const hstack_t hstack) {
    // Проверяем на отрицательный хэндлер
    if (hstack < 0) {
        return;
    }
    
    // Проверяем валидность хэндлера
    if (stack_valid_handler(hstack) != 0) {
        return;
    }
    
    int index = (int)hstack;
    
    // Освобождаем все элементы стека
    struct stack_item* current = g_table.entries[index].top;
    while (current != NULL) {
        struct stack_item* prev = current->prev;
        free(current);
        current = prev;
    }
    
    // Освобождаем запись в таблице
    g_table.entries[index].reserved = 0;
    g_table.entries[index].top = NULL;
}

// Проверяет валидность хэндлера стека
// Возвращает 0 если стек существует и валиден, 1 если стек невалиден
int stack_valid_handler(const hstack_t hstack) {
    // Отрицательные хэндлеры невалидны (кроме -1 для ошибок)
    if (hstack < 0) {
        return 1;
    }
    
    int index = (int)hstack;
    
    // Проверяем границы таблицы
    if (g_table.entries == NULL || index >= (int)g_table.size) {
        return 1;
    }
    
    // Проверяем занят ли слот
    return g_table.entries[index].reserved == 0;
}

// Возвращает количество элементов в стеке
// Если стек невалиден, возвращает 0
unsigned int stack_size(const hstack_t hstack) {
    // Проверяем на отрицательный хэндлер
    if (hstack < 0) {
        return 0;
    }
    
    // Проверяем валидность хэндлера
    if (stack_valid_handler(hstack) != 0) {
        return 0;
    }
    
    int index = (int)hstack;
    struct stack_item* current = g_table.entries[index].top;
    unsigned int count = 0;
    
    // Подсчитываем элементы в стеке
    while (current != NULL) {
        count++;
        current = current->prev;
    }
    
    return count;
}

// Добавляет элемент в вершину стека
// Если хэндлер невалиден или данные некорректны, функция ничего не делает
void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size) {
    // Проверяем на отрицательный хэндлер
    if (hstack < 0) {
        return;
    }
    
    // Проверяем валидность параметров
    if (stack_valid_handler(hstack) != 0 || data_in == NULL || size == 0) {
        return;
    }
    
    int index = (int)hstack;
    
    // Создаем новый элемент стека с памятью под данные
    struct stack_item* new_item = (struct stack_item*)malloc(sizeof(struct stack_item) + size);
    if (new_item == NULL) {
        return;
    }
    
    // Заполняем данные нового элемента
    new_item->prev = g_table.entries[index].top;
    new_item->size = size;
    memcpy(new_item->data, data_in, size);
    
    // Обновляем вершину стека
    g_table.entries[index].top = new_item;
}

// Извлекает элемент из вершины стека и копирует данные в буфер
// Возвращает количество скопированных байтов
// Если стек пуст или параметры некорректны, возвращает 0
unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size) {
    // Проверяем на отрицательный хэндлер
    if (hstack < 0) {
        return 0;
    }
    
    // Проверяем валидность параметров ДО обращения к стеку
    // Это предотвращает извлечение данных при невалидных параметрах
    if (stack_valid_handler(hstack) != 0 || data_out == NULL || size == 0) {
        return 0;
    }
    
    int index = (int)hstack;
    struct stack_item* top = g_table.entries[index].top;
    
    // Проверяем, не пуст ли стек
    if (top == NULL) {
        return 0;
    }
    
    // Определяем, сколько данных можно безопасно скопировать
    unsigned int copy_size = (size < top->size) ? size : top->size;
    memcpy(data_out, top->data, copy_size);
    
    // Обновляем вершину стека и освобождаем извлеченный элемент
    g_table.entries[index].top = top->prev;
    free(top);
    
    return copy_size;
}

