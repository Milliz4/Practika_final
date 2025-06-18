#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <clocale>
#include <locale>
#include <windows.h>

using namespace std;
const wstring FREQUENT_LETTERS = L"оеаинтсрвлкмдпуяыьгзбчйхжшюцщэфъё";

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
/*int crackShift(const wstring& filename) {
    wifstream inFile(filename);
    inFile.imbue(locale("ru_RU.UTF-8"));

    map<wchar_t, int> freq;
    wchar_t c;

    while (inFile.get(c)) {
        if (iswalpha(c)) {
            c = towlower(c);
            freq[c]++;
        }
    }

    if (freq.empty()) return 0;

    auto max_it = max_element(freq.begin(), freq.end(),
        [](auto& a, auto& b) { return a.second < b.second; });

    int shift = (max_it->first - FREQUENT_LETTERS[0] + 32) % 32;
    return shift;
}*/
int findBestShift(const wstring& filename) {
    wifstream inFile(filename);
    inFile.imbue(locale("rus_rus.866"));

    map<wchar_t, int> freq;
    wchar_t c;

    while (inFile.get(c)) {
        if (iswalpha(c)) {
            freq[towlower(c)]++;
        }
    }

    if (freq.empty()) return 0;

    // Самые частые буквы в русском (о, е, а)
    const wchar_t common_letters[3] = { L'о', L'е', L'а' };
    int best_shift = 0;
    int max_count = 0;

    // Проверяем все возможные сдвиги (0-31)
    for (int shift = 0; shift < 32; shift++) {
        int current_count = 0;
        for (auto& pair : freq) {
            wchar_t decrypted = L'а' + (pair.first - L'а' - shift + 32) % 32;
            for (wchar_t common : common_letters) {
                if (decrypted == common) {
                    current_count += pair.second;
                    break;
                }
            }
        }

        if (current_count > max_count) {
            max_count = current_count;
            best_shift = shift;
        }
    }

    return best_shift;
}


/*int main(int argc, char* argv[]) {
    // Настройка локали для поддержки русского языка
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    // Проверка аргументов
    //if (argc != 5) {
    if (argc<4){
        wcout << L"Использование:\n"
            << L"Шифрование: Practika_final.exe входной_файл выходной_файл сдвиг encrypt\n"
            << L"Дешифровка: Practika_final.exe входной_файл выходной_файл сдвиг decrypt\n"
            << L"Пример: Practika_final.exe text.txt encrypted.txt 5 encrypt\n"
            << L"Взлом: Practika_final.exe входной_файл выходной_файл crack\n";
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
}*/
int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    if (argc < 4) {
        wcout << L"Использование:\n"
            << L"Шифрование: program.exe входной_файл выходной_файл сдвиг encrypt\n"
            << L"Дешифровка: program.exe входной_файл выходной_файл сдвиг decrypt\n"
            << L"Автовзлом: program.exe входной_файл выходной_файл auto\n";
        return 1;
    }

    try {
        wstring inputFile(argv[1], argv[1] + strlen(argv[1]));
        wstring outputFile(argv[2], argv[2] + strlen(argv[2]));

        if (string(argv[3]) == "auto") {
            int found_shift = findBestShift(inputFile);
            wcout << L"Автоматически определенный сдвиг: " << found_shift << endl;
            processFile(inputFile, outputFile, found_shift, false);
            wcout << L"Текст успешно расшифрован!" << endl;
        }
        else {
            int shift = stoi(argv[3]);
            string mode = argv[4];
            processFile(inputFile, outputFile, shift, mode == "encrypt");
            wcout << L"Операция выполнена успешно!" << endl;
        }
    }
    catch (const exception& e) {
        wcerr << L"Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}
