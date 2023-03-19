#include <stdio.h>
#include <string.h>

void find_substring_indexes(char str[], char substr[]) {
    int len = strlen(str); // длина строки
    int sublen = strlen(substr); // длина подстроки
    int indexes[BUFSIZ], idx = 0; // массив индексов и индекс текущего вхождения
    for (int i = 0; i <= len - sublen; i++) { // перебираем все возможные подстроки
        if (strncmp(&str[i], substr, sublen) == 0) { // если текущая подстрока совпадает с заданной
            indexes[idx++] = i; // добавляем индекс в массив
        }
    }
    if (idx == 0) { // если подстрока не найдена
        printf("Substring not found.\n");
    } else {
        printf("Substring found at indexes: ");
        for (int i = 0; i < idx; i++) { // выводим индексы
            printf("%d ", indexes[i]);
        }
        printf("\n");
    }
}

int main() {
    char str[BUFSIZ], substr[BUFSIZ]; // строки;
    fgets(str, BUFSIZ, stdin); // вводим строку
    fgets(substr, BUFSIZ, stdin); // вводим подстроку
    // удаляем символ новой строки в конце строк
    str[strlen(str) - 1] = '\0';
    substr[strlen(substr) - 1] = '\0';
    find_substring_indexes(str, substr); // ищем подстроку и выводим индексы
    return 0;
}
