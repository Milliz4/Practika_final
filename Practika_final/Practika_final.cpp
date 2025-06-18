#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

// Функция для преобразования русских букв
wchar_t transformChar(wchar_t c, int shift, bool encrypt) {
    if (c >= L'А' && c <= L'Я') {
        if (encrypt) return L'А' + (c - L'А' + shift) % 32;
        else return L'А' + (c - L'А' - shift + 32) % 32;
    }
    else if (c >= L'а' && c <= L'я') {
        if (encrypt) return L'а' + (c - L'а' + shift) % 32;
        else return L'а' + (c - L'а' - shift + 32) % 32;
    }
    return c; // Оставляем другие символы без изменений
}

// Функция обработки файла
void processFile(const wstring& inputFile, const wstring& outputFile, int shift, bool encrypt) {
    wifstream inFile(inputFile);
    inFile.imbue(locale("rus_rus.866"));

    if (!inFile) {
        wcerr << L"Ошибка открытия входного файла" << endl;
        return;
    }

    wofstream outFile(outputFile);
    outFile.imbue(locale("rus_rus.866"));

    wchar_t c;
    while (inFile.get(c)) {
        outFile.put(transformChar(c, shift, encrypt));
    }

    inFile.close();
    outFile.close();
}

int main(int argc, char* argv[]) {
    // Настройка локали для поддержки русского языка
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    // Проверка аргументов
    if (argc != 5) {
        wcout << L"Использование:\n"
            << L"Шифрование: Practika_final.exe входной_файл выходной_файл сдвиг encrypt\n"
            << L"Дешифровка: Practika_final.exe входной_файл выходной_файл сдвиг decrypt\n"
            << L"Пример: Practika_final.exe text.txt encrypted.txt 5 encrypt\n";
        return 1;
    }

    try {
        // Преобразование аргументов в wide-строки
        wstring inputFile(argv[1], argv[1] + strlen(argv[1]));
        wstring outputFile(argv[2], argv[2] + strlen(argv[2]));
        int shift = stoi(argv[3]);
        string mode = argv[4];

        if (mode != "encrypt" && mode != "decrypt") {
            throw runtime_error("Неверный режим работы");
        }

        processFile(inputFile, outputFile, shift, mode == "encrypt");
        wcout << L"Операция выполнена успешно!\n";
    }
    catch (const exception& e) {
        wcerr << L"Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}
