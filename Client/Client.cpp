#include <iostream>
#include <string>
#include <windows.h>

#pragma warning(disable: 4996)

struct employee {
    int num;
    char name[10];
    double hours;
};

const std::wstring PIPE_NAME = L"\\\\.\\pipe\\employee_pipe";

class EmployeeClient {
private:
    HANDLE hPipe;

public:
    EmployeeClient() : hPipe(INVALID_HANDLE_VALUE) {}

    bool connectToServer() {
        hPipe = CreateFile(
            PIPE_NAME.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        return hPipe != INVALID_HANDLE_VALUE;
    }

    void readRecord() {
        int id;
        std::cout << "Введите ID сотрудника для чтения: ";
        std::cin >> id;

        std::string request = "READ";
        DWORD bytesWritten;
        WriteFile(hPipe, request.c_str(), request.size(), &bytesWritten, NULL);
        WriteFile(hPipe, &id, sizeof(id), &bytesWritten, NULL);

        employee emp;
        DWORD bytesRead;
        ReadFile(hPipe, &emp, sizeof(emp), &bytesRead, NULL);

        if (bytesRead == sizeof(emp)) {
            std::cout << "Найден сотрудник:\n";
            std::cout << "Номер: " << emp.num << std::endl;
            std::cout << "Имя: " << emp.name << std::endl;
            std::cout << "Часы: " << emp.hours << std::endl;
        }
        else {
            std::cout << "Сотрудник не найден" << std::endl;
        }

        std::cout << "Нажмите Enter для продолжения...";
        std::cin.ignore();
        std::cin.get();
    }

    void modifyRecord() {
        int id;
        std::cout << "Введите ID сотрудника для модификации: ";
        std::cin >> id;

        std::string request = "MODIFY";
        DWORD bytesWritten;
        WriteFile(hPipe, request.c_str(), request.size(), &bytesWritten, NULL);
        WriteFile(hPipe, &id, sizeof(id), &bytesWritten, NULL);

        employee emp;
        DWORD bytesRead;
        ReadFile(hPipe, &emp, sizeof(emp), &bytesRead, NULL);

        if (bytesRead == sizeof(emp)) {
            std::cout << "Текущие данные:\n";
            std::cout << "Номер: " << emp.num << std::endl;
            std::cout << "Имя: " << emp.name << std::endl;
            std::cout << "Часы: " << emp.hours << std::endl;

            std::cout << "Введите новые данные:\n";
            std::cout << "Новое имя: ";
            std::cin >> emp.name;
            std::cout << "Новое количество часов: ";
            std::cin >> emp.hours;

            WriteFile(hPipe, &emp, sizeof(emp), &bytesWritten, NULL);
            std::cout << "Данные обновлены" << std::endl;
        }
        else {
            std::cout << "Сотрудник не найден" << std::endl;
        }

        std::cout << "Нажмите Enter для продолжения...";
        std::cin.ignore();
        std::cin.get();
    }

    void run() {
        if (!connectToServer()) {
            std::cerr << "Не удалось подключиться к серверу" << std::endl;
            return;
        }

        while (true) {
            std::cout << "\nВыберите операцию:\n";
            std::cout << "1 - Модификация записи\n";
            std::cout << "2 - Чтение записи\n";
            std::cout << "3 - Выход\n";
            std::cout << "Ваш выбор: ";

            int choice;
            std::cin >> choice;

            switch (choice) {
            case 1:
                modifyRecord();
                break;
            case 2:
                readRecord();
                break;
            case 3: {
                std::string request = "EXIT";
                DWORD bytesWritten;
                WriteFile(hPipe, request.c_str(), request.size(), &bytesWritten, NULL);
                CloseHandle(hPipe);
                return;
            }
            default:
                std::cout << "Неверный выбор" << std::endl;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    EmployeeClient client;
    client.run();
    return 0;
}