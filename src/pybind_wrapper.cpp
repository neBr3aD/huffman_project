#include <pybind11/pybind11.h>
#include <fstream>
#include <string>
#include <stdexcept>
#include "../include/my_string.hpp"
#include "../include/huffman.hpp"

namespace py = pybind11;

const char MAGIC[4] = {'H', 'U', 'F', 'F'};

bool compressFile(const std::string& inFile, const std::string& outFile) {
    std::ifstream in(inFile, std::ios::binary | std::ios::ate);
    if (!in.is_open()) throw std::runtime_error("Не удалось открыть исходный файл");

    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    char* buffer = new char[size];
    if (!in.read(buffer, size)) {
        delete[] buffer;
        throw std::runtime_error("Ошибка чтения файла");
    }
    in.close();

    int freqs[256] = {0};
    for (std::streamsize i = 0; i < size; ++i) {
        freqs[static_cast<unsigned char>(buffer[i])]++;
    }

    HuffmanCoder coder;
    coder.buildTree(freqs);

    std::ofstream out(outFile, std::ios::binary);
    if (!out.is_open()) {
        delete[] buffer;
        throw std::runtime_error("Не удалось создать архив");
    }

    out.write(MAGIC, 4);
    int unique_chars = 0;
    for (int i = 0; i < 256; ++i) if (freqs[i] > 0) unique_chars++;

    out.write(reinterpret_cast<char*>(&unique_chars), sizeof(unique_chars));
    for (int i = 0; i < 256; ++i) {
        if (freqs[i] > 0) {
            unsigned char ch = i;
            out.write(reinterpret_cast<char*>(&ch), sizeof(ch));
            out.write(reinterpret_cast<char*>(&freqs[i]), sizeof(freqs[i]));
        }
    }

    std::size_t totalBits = 0;
    std::streampos bitsPos = out.tellp();
    out.write(reinterpret_cast<char*>(&totalBits), sizeof(totalBits));

    unsigned char current_byte = 0;
    int bit_count = 0;

    for (std::streamsize i = 0; i < size; ++i) {
        unsigned char ch = static_cast<unsigned char>(buffer[i]);
        MyString code = coder.getCode(ch);

        for (std::size_t j = 0; j < code.length(); ++j) {
            current_byte <<= 1;
            if (code[j] == '1') current_byte |= 1;
            bit_count++;
            totalBits++;

            if (bit_count == 8) {
                out.put(current_byte);
                current_byte = 0;
                bit_count = 0;
            }
        }
    }

    if (bit_count > 0) {
        current_byte <<= (8 - bit_count);
        out.put(current_byte);
    }

    out.seekp(bitsPos);
    out.write(reinterpret_cast<char*>(&totalBits), sizeof(totalBits));

    out.close();
    delete[] buffer;
    return true;
}

bool decompressFile(const std::string& inFile, const std::string& outFile) {
    std::ifstream in(inFile, std::ios::binary);
    if (!in.is_open()) throw std::runtime_error("Не удалось открыть архив");

    char magic[4];
    in.read(magic, 4);
    if (magic[0] != MAGIC[0] || magic[1] != MAGIC[1] ||
        magic[2] != MAGIC[2] || magic[3] != MAGIC[3]) {
        throw std::runtime_error("Неверный формат файла (ожидался .huff)");
    }

    int unique_chars = 0;
    in.read(reinterpret_cast<char*>(&unique_chars), sizeof(unique_chars));

    int freqs[256] = {0};
    for (int i = 0; i < unique_chars; ++i) {
        unsigned char ch;
        int f;
        in.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        in.read(reinterpret_cast<char*>(&f), sizeof(f));
        freqs[ch] = f;
    }

    HuffmanCoder coder;
    coder.buildTree(freqs);

    std::size_t totalBits = 0;
    in.read(reinterpret_cast<char*>(&totalBits), sizeof(totalBits));

    MyString bits;
    unsigned char current_byte;
    std::size_t bits_read = 0;

    while (in.read(reinterpret_cast<char*>(&current_byte), 1)) {
        for (int i = 7; i >= 0; --i) {
            if (bits_read >= totalBits) break;
            if ((current_byte >> i) & 1) bits += '1';
            else bits += '0';
            bits_read++;
        }
    }

    std::ofstream out(outFile, std::ios::binary);
    if (!out.is_open()) throw std::runtime_error("Не удалось создать распакованный файл");

    MyString decodedText = coder.decode(bits);
    for(size_t i = 0; i < decodedText.length(); ++i) {
        out.put(decodedText[i]);
    }

    return true;
}

PYBIND11_MODULE(huffman_archiver, m) {
    m.doc() = "C++ Huffman Archiver Module for Python";
    m.def("compress", &compressFile, "Сжимает текстовый файл в архив .huff");
    m.def("decompress", &decompressFile, "Распаковывает .huff архив");
}