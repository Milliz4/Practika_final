#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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

// Поиск позиции символа в алфавите
int findPos(unsigned char c1, unsigned char c2) {
    for (int i = 0; i < RUS_UTF8.size(); ++i) {
        if (RUS_UTF8[i].first == c1 && RUS_UTF8[i].second == c2)
            return i;
    }
    return -1;
}

// Функция шифрования
string encrypt(const string& text, int key) {
    string result;
    int size = RUS_UTF8.size();

    for (size_t i = 0; i < text.size(); ) {
        // Обрабатываем UTF-8 символы (русские буквы - 2 байта)
        if ((text[i] & 0xE0) == 0xC0 && i + 1 < text.size()) {
            int pos = findPos(text[i], text[i + 1]);
            if (pos != -1) {
                int newPos = (pos + key + size) % size;
                result += RUS_UTF8[newPos].second;
                i += 2;
                continue;
            }
        }
        // Все остальные символы оставляем как есть
        result += text[i];
        i++;
    }
    return result;
}

// Чтение файла как бинарного
string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    return string((istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>());
}

// Запись файла с принудительным UTF-8
void writeFile(const string& filename, const string& content) {
    ofstream file(filename, ios::binary);
    // Добавляем BOM для явного указания UTF-8
    file << "\xEF\xBB\xBF";
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage:\n"
            << "  cipher_force_utf8.exe input.txt output.txt -e -k 3\n"
            << "  cipher_force_utf8.exe encrypted.txt decrypted.txt -d -k 3\n";
        return 0;
    }

    string inputFile, outputFile;
    int key = 3;


    // Обработка аргументов
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-e") encryptMode = true;
        else if (arg == "-d") encryptMode = false;
        else if (arg == "-k" && i + 1 < argc) key = stoi(argv[++i]);
        else if (inputFile.empty()) inputFile = arg;
        else if (outputFile.empty()) outputFile = arg;
    }

    try {
        string text = readFile(inputFile);
        string result = encrypt(text, encryptMode ? key : -key);

        writeFile(outputFile, result);
        cout << "File successfully processed: " << outputFile << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}