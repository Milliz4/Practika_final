#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <windows.h>

using namespace std;

// Русский алфавит в UTF-8 (первая часть двухбайтовых последовательностей)
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

// Частотность русских букв (от наиболее частых к менее частым)
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

int findPos(unsigned char c1, unsigned char c2) {
    for (int i = 0; i < RUS_UTF8.size(); ++i) {
        if (RUS_UTF8[i].first == c1 && RUS_UTF8[i].second == c2)
            return i;
    }
    return -1;
}

string transformText(const string& text, int key) {
    string result;
    int size = RUS_UTF8.size();

    for (size_t i = 0; i < text.size(); ) {
        if ((text[i] & 0xE0) == 0xC0 && i + 1 < text.size()) {
            int pos = findPos(text[i], text[i + 1]);
            if (pos != -1) {
                int newPos = (pos + key + size) % size;
                result += RUS_UTF8[newPos].first;
                result += RUS_UTF8[newPos].second;
                i += 2;
                continue;
            }
        }
        result += text[i];
        i++;
    }
    return result;
}

double calculateFrequencyScore(const map<int, int>& freqMap, int totalLetters) {
    double score = 0.0;
    for (const auto& pair : freqMap) {
        int pos = pair.first;
        int count = pair.second;
        double actualFreq = (count * 100.0) / totalLetters;
        double expectedFreq = LETTER_FREQUENCY.at(pos);
        score += min(actualFreq, expectedFreq); // Чем ближе частоты, тем выше score
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
                freqMap[pos]++;
                totalLetters++;
                i += 2;
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

        // Считаем частоты для текущего ключа
        for (const auto& pair : freqMap) {
            int decryptedPos = (pair.first - key + 33) % 33;
            decryptedFreq[decryptedPos] = pair.second;
        }

        double score = calculateFrequencyScore(decryptedFreq, totalLetters);

        if (score > bestScore) {
            bestScore = score;
            bestKey = key;
        }
    }

    // Проверяем, что найденный ключ дает осмысленный текст
    string testText = transformText(encryptedText, -bestKey);
    int spaceCount = count(testText.begin(), testText.end(), ' ');
    if (spaceCount < testText.size() / 20) { // Минимум 5% пробелов
        cerr << "Найденный ключ не дает осмысленного текста\n";
        return 0;
    }

    return bestKey;
}

string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    return string((istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>());
}

void writeFile(const string& filename, const string& content) {
    ofstream file(filename, ios::binary);
    file << "\xEF\xBB\xBF" << content;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    if (argc < 2) {
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

    try {
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
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}