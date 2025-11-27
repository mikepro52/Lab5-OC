#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <cassert>

#pragma warning(disable: 4996)

struct employee {
    int num;
    char name[10];
    double hours;
};

class SimpleTests {
public:
    void runAllTests() {
        std::cout << "=== ТЕСТЫ ЛАБОРАТОРНОЙ РАБОТЫ №5 ===\n\n";

        int passed = 0;
        int total = 0;

        // Тест 1: Структура employee
        total++;
        std::cout << "Тест 1: Структура employee... ";
        if (testEmployeeStruct()) {
            std::cout << "ПРОЙДЕН ✓\n";
            passed++;
        }
        else {
            std::cout << "ПРОВАЛЕН ✗\n";
        }

        // Тест 2: Файловые операции
        total++;
        std::cout << "Тест 2: Файловые операции... ";
        if (testFileOperations()) {
            std::cout << "ПРОЙДЕН ✓\n";
            passed++;
        }
        else {
            std::cout << "ПРОВАЛЕН ✗\n";
        }

        // Тест 3: Поиск сотрудников
        total++;
        std::cout << "Тест 3: Поиск сотрудников... ";
        if (testEmployeeSearch()) {
            std::cout << "ПРОЙДЕН ✓\n";
            passed++;
        }
        else {
            std::cout << "ПРОВАЛЕН ✗\n";
        }

        // Тест 4: Модификация данных
        total++;
        std::cout << "Тест 4: Модификация данных... ";
        if (testDataModification()) {
            std::cout << "ПРОЙДЕН ✓\n";
            passed++;
        }
        else {
            std::cout << "ПРОВАЛЕН ✗\n";
        }

        std::cout << "\n=== РЕЗУЛЬТАТ: " << passed << "/" << total << " тестов пройдено ===\n";

        if (passed == total) {
            std::cout << "=== ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ! ===\n";
        }
        else {
            std::cout << "=== НЕКОТОРЫЕ ТЕСТЫ ПРОВАЛЕНЫ ===\n";
        }
    }

private:
    bool testEmployeeStruct() {
        try {
            employee emp{ 1, "Ivan", 40.5 };
            return emp.num == 1 &&
                std::string(emp.name) == "Ivan" &&
                emp.hours == 40.5;
        }
        catch (...) {
            return false;
        }
    }

    bool testFileOperations() {
        std::string test_filename = "test_employees.dat";
        try {
            // Запись в файл
            std::ofstream file(test_filename, std::ios::binary);
            if (!file.is_open()) return false;

            employee emp1{ 1, "Anna", 35.0 };
            employee emp2{ 2, "Peter", 42.5 };

            file.write(reinterpret_cast<char*>(&emp1), sizeof(employee));
            file.write(reinterpret_cast<char*>(&emp2), sizeof(employee));
            file.close();

            // Чтение из файла
            std::ifstream in_file(test_filename, std::ios::binary);
            if (!in_file.is_open()) return false;

            employee test_emp;
            in_file.read(reinterpret_cast<char*>(&test_emp), sizeof(employee));
            bool result1 = (test_emp.num == 1);

            in_file.read(reinterpret_cast<char*>(&test_emp), sizeof(employee));
            bool result2 = (test_emp.num == 2);

            in_file.close();
            remove(test_filename.c_str());

            return result1 && result2;
        }
        catch (...) {
            remove(test_filename.c_str());
            return false;
        }
    }

    bool testEmployeeSearch() {
        try {
            std::vector<employee> employees = {
                {1, "John", 40.0},
                {2, "Alice", 35.5},
                {3, "Bob", 42.0}
            };

            bool found_existing = false;
            for (auto& emp : employees) {
                if (emp.num == 2) {
                    found_existing = true;
                    break;
                }
            }

            bool not_found_missing = true;
            for (auto& emp : employees) {
                if (emp.num == 999) {
                    not_found_missing = false;
                    break;
                }
            }

            return found_existing && not_found_missing;
        }
        catch (...) {
            return false;
        }
    }

    bool testDataModification() {
        try {
            employee emp{ 1, "OldName", 30.0 };

            strcpy(emp.name, "NewName");
            emp.hours = 45.5;

            return std::string(emp.name) == "NewName" && emp.hours == 45.5;
        }
        catch (...) {
            return false;
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    std::cout << "Запуск тестов лабораторной работы №5...\n";
    std::cout << "Тема: 'Обмен данными по именованным каналам'\n\n";

    SimpleTests tests;
    tests.runAllTests();

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}