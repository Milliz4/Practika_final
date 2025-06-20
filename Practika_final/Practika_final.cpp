#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <windows.h>

using namespace std;

const vector<pair<unsigned char, unsigned char>> RUS_UTF8 = {
    {0xD0, 0x90}, {0xD0, 0x91}, {0xD0, 0x92}, {0xD0, 0x93}, {0xD0, 0x94},
    {0xD0, 0x95}, {0xD0, 0x81}, {0xD0, 0x96}, {0xD0, 0x97}, {0xD0, 0x98},
    {0xD0, 0x99}, {0xD0, 0x9A}, {0xD0, 0x9B}, {0xD0, 0x9C}, {0xD0, 0x9D},
    {0xD0, 0x9E}, {0xD0, 0x9F}, {0xD0, 0xA0}, {0xD0, 0xA1}, {0xD0, 0xA2},
    {0xD0, 0xA3}, {0xD0, 0xA4}, {0xD0, 0xA5}, {0xD0, 0xA6}, {0xD0, 0xA7},
    {0xD0, 0xA8}, {0xD0, 0xA9}, {0xD0, 0xAA}, {0xD0, 0xAB}, {0xD0, 0xAC},
    {0xD0, 0xAD}, {0xD0, 0xAE}, {0xD0, 0xAF}, {0xD0, 0xB0}, {0xD0, 0xB1},
    {0xD0, 0xB2}, {0xD0, 0xB3}, {0xD0, 0xB4}, {0xD0, 0xB5}, {0xD1, 0x91},
    {0xD0, 0xB6}, {0xD0, 0xB7}, {0xD0, 0xB8}, {0xD0, 0xB9}, {0xD0, 0xBA},
    {0xD0, 0xBB}, {0xD0, 0xBC}, {0xD0, 0xBD}, {0xD0, 0xBE}, {0xD0, 0xBF},
    {0xD1, 0x80}, {0xD1, 0x81}, {0xD1, 0x82}, {0xD1, 0x83}, {0xD1, 0x84},
    {0xD1, 0x85}, {0xD1, 0x86}, {0xD1, 0x87}, {0xD1, 0x88}, {0xD1, 0x89},
    {0xD1, 0x8A}, {0xD1, 0x8B}, {0xD1, 0x8C}, {0xD1, 0x8D}, {0xD1, 0x8E},
    {0xD1, 0x8F}
};

const vector<int> FREQUENT_LETTERS = { 12, 5, 0, 8, 13, 18, 19, 17, 2, 10, 9, 14, 3, 15, 20, 32, 21, 7, 23, 1, 4, 22, 11, 24, 6, 25, 31, 26, 27, 30, 28, 29, 16 };

// Статистика частотности букв в русском языке (в %)
const map<int, double> LETTER_FREQUENCY = {
    {0, 8.01},  // А
    {1, 1.59},  // Б
    {2, 4.54},  // В
    {3, 1.70},  // Г
    {4, 2.98},  // Д
    {5, 8.45},  // Е
    {6, 0.04},  // Ё
    {7, 1.65},  // Ж
    {8, 7.35},  // З
    {9, 1.21},  // И
    {10, 0.64}, // Й
    {11, 3.49}, // К
    {12, 4.40}, // Л
    {13, 3.21}, // М
    {14, 6.70}, // Н
    {15, 10.97},// О
    {16, 2.81}, // П
    {17, 4.73}, // Р
    {18, 5.47}, // С
    {19, 6.26}, // Т
    {20, 2.62}, // У
    {21, 0.97}, // Ф
    {22, 0.48}, // Х
    {23, 1.44}, // Ц
    {24, 0.73}, // Ч
    {25, 0.36}, // Ш
    {26, 0.35}, // Щ
    {27, 1.90}, // Ъ
    {28, 1.74}, // Ы
    {29, 0.32}, // Ь
    {30, 0.66}, // Э
    {31, 0.10}, // Ю
    {32, 1.98}  // Я
};

int findPos(unsigned char c1, unsigned char c2) { // ищем позицию символа
    for (int i = 0; i < RUS_UTF8.size(); ++i) { //перебор алфавита
        if (RUS_UTF8[i].first == c1 && RUS_UTF8[i].second == c2) // если 1 и 2 байт сопадают возвращают позицию
            return i;
    }
    return 1;
}

string transformText(const string& text, int key) {
    string result;
    int size = RUS_UTF8.size();

    for (size_t i = 0; i < text.size(); ) {// перебор посимвольно алфавита
        if ((text[i] & 0xE0) == 0xC0 && i + 1 < text.size()) {// Проверяем, является ли текущий символ началом русской буквы в UTF-8
            int pos = findPos(text[i], text[i + 1]);

            if (pos != -1) {//если русская буква
                int newPos = (pos + key + size) % size;// новая позиция
                result += RUS_UTF8[newPos].first;// Первый байт UTF-8
                result += RUS_UTF8[newPos].second;// Второй байт UTF-8
                i += 2;//пропускаем 2 байта (переход к следующему символу)
                continue;
            }
        }
        result += text[i];// если не русская, то оставляем как есть
        i++;
    }
    return result;
}

double calculateFrequencyScore(const map<int, int>& freqMap, int totalLetters) {
    double score = 0.0;
    for (const auto& pair : freqMap) {
        int pos = pair.first;// Позиция буквы в алфавите
        int count = pair.second;// Количество встреч буквы
        double actualFreq = (count * 100.0) / totalLetters;// Вычисляем фактическую частоту буквы в тексте (%)
        double expectedFreq = LETTER_FREQUENCY.at(pos);// Получаем ожидаемую частоту из статистики
        score += min(actualFreq, expectedFreq); // минимальное ожидаемой и фактическрй
    }
    return score;
}

int crackCipher(const string& encryptedText) {
    map<int, int> freqMap;
    int totalLetters = 0;

    // Считаем частоту каждой буквы
    for (size_t i = 0; i < encryptedText.size(); ) {
        if ((encryptedText[i] & 0xE0) == 0xC0 && i + 1 < encryptedText.size()) {
            int pos = findPos(encryptedText[i], encryptedText[i + 1]);
            if (pos != -1) {
                freqMap[pos]++; // Увеличиваем счётчик для этой буквы
                totalLetters++; // Общее количество русских букв
                i += 2; continue; // Следующий символ 
                continue;
            }
        }
        i++;
    }

    if (totalLetters < 10) {
        cerr << "Слишком мало русских букв для анализа (нужно минимум 10, найдено " << totalLetters << ")\n";
        return 0;
    }

    // Находим ключ с наилучшим соответствием частот
    int bestKey = 0;
    double bestScore = 0.0;

    for (int key = 0; key < 33; ++key) {
        map<int, int> decryptedFreq;

        // Для каждой буквы вычисляем её позицию после дешифровки
        for (const auto& pair : freqMap) {
            int decryptedPos = (pair.first - key + 33) % 33;// +33 убираем отрицательность
            decryptedFreq[decryptedPos] = pair.second;
        }
        // насколько распределение похоже на русский язык
        double score = calculateFrequencyScore(decryptedFreq, totalLetters);

        if (score > bestScore) {
            bestScore = score;
            bestKey = key;
        }
    }

    //ключ дает осмысленный текст
    string testText = transformText(encryptedText, -bestKey);
    int spaceCount = count(testText.begin(), testText.end(), ' ');// проверка кол-ва пробелов
    if (spaceCount < testText.size() / 20) { // Минимум 5% пробелов
        cerr << "Найденный ключ не дает осмысленного текста\n";
        return 0;
    }

    return bestKey;
}

string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    return string((istreambuf_iterator<char>(file)),//эффективное чтение файла целиком
        istreambuf_iterator<char>());//Возвращает строку со всем содержимым файла
}

void writeFile(const string& filename, const string& content) {
    ofstream file(filename, ios::binary);  // Открываем файл для записи в бинарном режиме
    file << "\xEF\xBB\xBF" << content;    // Записываем BOM (метку порядка байтов) и содержимое
}
void showHelp() {
    // Оформление рамки
    const string border = "============================================================";
    const string empty_line = "|                                                      |";

    // Выводим красивую справку
    cout << border << endl;
    cout << empty_line << endl;
    cout << "|                  СПРАВКА ПО ПРОГРАММЕ                  |" << endl;
    cout << empty_line << endl;
    cout << "|  Программа для шифрования/дешифрования текста методом  |" << endl;
    cout << "|                шифра Цезаря на русском языке           |" << endl;
    cout << empty_line << endl;
    cout << "|  Использование:                                        |" << endl;
    cout << "|    Шифрование:   program.exe input.txt output.txt -e -k 3 |" << endl;
    cout << "|    Дешифровка:   program.exe encrypted.txt decrypted.txt -d -k 3 |" << endl;
    cout << "|    Взлом шифра:  program.exe encrypted.txt decrypted.txt -c |" << endl;
    cout << empty_line << endl;
    cout << "|  Параметры:                                            |" << endl;
    cout << "|    -e          Режим шифрования                        |" << endl;
    cout << "|    -d          Режим дешифрования                      |" << endl;
    cout << "|    -c          Режим взлома шифра                      |" << endl;
    cout << "|    -k N        Указание ключа (сдвига)                 |" << endl;
    cout << "|    Help        Вывод этой справки                      |" << endl;
    cout << empty_line << endl;
    cout << "|  Примеры:                                              |" << endl;
    cout << "|    program.exe text.txt encrypted.txt -e -k 5          |" << endl;
    cout << "|    program.exe encrypted.txt decrypted.txt -d -k 5     |" << endl;
    cout << "|    program.exe encrypted.txt decrypted.txt -c          |" << endl;
    cout << empty_line << endl;
    cout << border << endl;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Проверка на запрос справки
    if (argc == 2 && string(argv[1]) == "Help") {
        showHelp();
        return 0;
    }

    if (argc < 2) {
        cout << "Для вывода справки введите: program.exe Help" << endl;
        cout << "Использование:\n"
            << "  Шифрование: program.exe input.txt output.txt -e -k 3\n"
            << "  Дешифровка: program.exe encrypted.txt decrypted.txt -d -k 3\n"
            << "  Взлом шифра: program.exe encrypted.txt decrypted.txt -c\n";
        return 0;
    }

    string inputFile, outputFile;
    int key = 3;
    string mode = "-e";

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-e" || arg == "-d" || arg == "-c") mode = arg;
        else if (arg == "-k" && i + 1 < argc) key = stoi(argv[++i]);
        else if (inputFile.empty()) inputFile = arg;
        else if (outputFile.empty()) outputFile = arg;
    }

        string text = readFile(inputFile);
        string result;

        if (mode == "-c") {
            int foundKey = crackCipher(text);
            if (foundKey == 0) {
                cerr << "Не удалось точно определить ключ. Попробуйте следующие варианты:\n";
                for (int k = 1; k <= 3; ++k) {
                    cout << "Ключ " << k << ": " << transformText(text, -k).substr(0, 50) << "...\n";
                }
                return 1;
            }
            cout << "Найденный ключ: " << foundKey << endl;
            result = transformText(text, -foundKey);
        }
        else {
            int effectiveKey = (mode == "-e") ? key : -key;
            result = transformText(text, effectiveKey);
        }

        writeFile(outputFile, result);
        cout << "Файл успешно обработан: " << outputFile << endl;

    return 0;
}