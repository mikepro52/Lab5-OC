#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

#pragma warning(disable: 4996)

struct employee {
    int num;
    char name[10];
    double hours;
};

const std::wstring PIPE_NAME = L"\\\\.\\pipe\\employee_pipe";
const int BUFFER_SIZE = 1024;

class EmployeeServer {
private:
    std::string filename;
    std::vector<employee> employees;
    HANDLE hPipe;

public:
    EmployeeServer() : hPipe(INVALID_HANDLE_VALUE) {}

    bool createFile() {
        std::cout << "Введите имя файла: ";
        std::cin >> filename;

        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Ошибка создания файла" << std::endl;
            return false;
        }

        int count;
        std::cout << "Введите количество сотрудников: ";
        std::cin >> count;

        employees.resize(count);
        for (int i = 0; i < count; ++i) {
            std::cout << "Сотрудник " << i + 1 << ":\n";
            std::cout << "  Номер: ";
            std::cin >> employees[i].num;
            std::cout << "  Имя: ";
            std::cin >> employees[i].name;
            std::cout << "  Часы: ";
            std::cin >> employees[i].hours;

            file.write(reinterpret_cast<char*>(&employees[i]), sizeof(employee));
        }

        file.close();
        return true;
    }

    void displayFile() {
        std::cout << "\nСодержимое файла:\n";
        std::ifstream file(filename, std::ios::binary);
        employee emp;

        while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
            std::cout << "Номер: " << emp.num
                << ", Имя: " << emp.name
                << ", Часы: " << emp.hours << std::endl;
        }
        file.close();
    }

    bool createPipe() {
        hPipe = CreateNamedPipe(
            PIPE_NAME.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            BUFFER_SIZE,
            BUFFER_SIZE,
            0,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Ошибка создания канала" << std::endl;
            return false;
        }

        std::cout << "Ожидание подключения клиента..." << std::endl;
        return ConnectNamedPipe(hPipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
    }

    void loadEmployees() {
        employees.clear();
        std::ifstream file(filename, std::ios::binary);
        employee emp;

        while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
            employees.push_back(emp);
        }
        file.close();
    }

    void saveEmployees() {
        std::ofstream file(filename, std::ios::binary);
        for (const auto& emp : employees) {
            file.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
        }
        file.close();
    }

    employee* findEmployee(int id) {
        for (auto& emp : employees) {
            if (emp.num == id) {
                return &emp;
            }
        }
        return nullptr;
    }

    void handleClient() {
        char buffer[BUFFER_SIZE];
        DWORD bytesRead;

        while (true) {
            if (ReadFile(hPipe, buffer, BUFFER_SIZE, &bytesRead, NULL)) {
                std::string request(buffer, bytesRead);

                if (request == "READ") {
                    int id;
                    ReadFile(hPipe, &id, sizeof(id), &bytesRead, NULL);

                    employee* emp = findEmployee(id);
                    WriteFile(hPipe, emp, sizeof(employee), &bytesRead, NULL);

                }
                else if (request == "MODIFY") {
                    int id;
                    ReadFile(hPipe, &id, sizeof(id), &bytesRead, NULL);

                    employee* emp = findEmployee(id);
                    WriteFile(hPipe, emp, sizeof(employee), &bytesRead, NULL);

                    ReadFile(hPipe, emp, sizeof(employee), &bytesRead, NULL);
                    saveEmployees();

                }
                else if (request == "EXIT") {
                    break;
                }
            }
        }
    }

    void run() {
        if (!createFile()) return;
        displayFile();

        if (!createPipe()) return;

        int clientCount;
        std::cout << "Введите количество клиентов: ";
        std::cin >> clientCount;

        for (int i = 0; i < clientCount; ++i) {
            loadEmployees();
            handleClient();
        }

        std::cout << "\nМодифицированный файл:\n";
        displayFile();

        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);

        std::cout << "Нажмите любую клавишу для завершения...";
        std::cin.ignore();
        std::cin.get();
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    EmployeeServer server;
    server.run();
    return 0;
}