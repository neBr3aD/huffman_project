#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <iostream>
#include <fstream>
#include "my_string.hpp"
#include "hash_table.hpp"
#include "linkedList.hpp"
#include "huffman_node.hpp"

class HuffmanCoder {
private:
    HuffmanNode* root_;
    HashTable<MyString> codesTable_;
    void generateCodes(HuffmanNode* node, const MyString& currentCode);
    void clearTree(HuffmanNode* node);

public:
    HuffmanCoder();
    ~HuffmanCoder();
    void buildTree(const int frequencies[256]);
    void buildTree(const MyString& text);
    MyString getCode(unsigned char ch) const;
    void decodeFromFile(std::ifstream& inFile, std::ofstream& outFile);
    MyString encode(const MyString& text) const;
    MyString decode(const MyString& encodedText) const;
    void printCodes() const;
};
void HuffmanCoder::generateCodes(HuffmanNode* node, const MyString& currentCode) {
    if (!node) return;
    if (!node->left_ && !node->right_) {
        unsigned char uc = static_cast<unsigned char>(node->symbol_);
        char key[2] = { static_cast<char>(uc), '\0' };
        codesTable_.insert(key, currentCode);
        return;
    }
    generateCodes(node->left_, currentCode + "0");
    generateCodes(node->right_, currentCode + "1");
}
void HuffmanCoder::clearTree(HuffmanNode* node) {
    if (!node) return;
    clearTree(node->left_);
    clearTree(node->right_);
    delete node;
}
HuffmanCoder::HuffmanCoder() : root_(nullptr), codesTable_(256) {}
HuffmanCoder::~HuffmanCoder() {
    clearTree(root_);
}
void HuffmanCoder::buildTree(const int frequencies[256]) {
    clearTree(root_);
    root_ = nullptr;
    codesTable_.clear();
    DoubleOrderedList<NodeWrapper> list;
    std::size_t nextOrder = 0;
    for (int i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) {
            list.insert(NodeWrapper(new HuffmanNode(
                static_cast<char>(i), frequencies[i], nullptr, nullptr, nextOrder++
            )));
        }
    }
    if (list.size() == 0) return;
    if (list.size() == 1) {
        HuffmanNode* singleNode = list.head().node_;
        list.removeHead();
        root_ = new HuffmanNode('\0', singleNode->freq_, singleNode, nullptr);
        generateCodes(root_, "");
        return;
    }
    while (list.size() > 1) {
        HuffmanNode* firstNode = list.head().node_;
        list.removeHead();
        HuffmanNode* secondNode = list.head().node_;
        list.removeHead();
        HuffmanNode* parent = new HuffmanNode(
            '\0', firstNode->freq_ + secondNode->freq_, firstNode, secondNode, nextOrder++
        );
        list.insert(NodeWrapper(parent));
    }
    if (list.size() == 1) {
        root_ = list.head().node_;
        list.removeHead();
    }
    if (root_) {
        generateCodes(root_, "");
    }
}
void HuffmanCoder::buildTree(const MyString& text) {
    clearTree(root_);
    root_ = nullptr;
    codesTable_.clear();
    if (text.length() == 0) return;
    HashTable<int> freqTable(256);
    for (std::size_t i = 0; i < text.length(); ++i) {
        char key[2] = { text[i], '\0' };
        int currentFreq = 0;
        if (freqTable.find(key, currentFreq)) {
            freqTable.remove(key);
            freqTable.insert(key, currentFreq + 1);
        } else {
            freqTable.insert(key, 1);
        }
    }
    DoubleOrderedList<NodeWrapper> list;
    std::size_t nextOrder = 0;
    for (int i = 0; i < 256; ++i) {
        char key[2] = { static_cast<char>(i), '\0' };
        int freq = 0;
        if (freqTable.find(key, freq)) {
            list.insert(NodeWrapper(new HuffmanNode(
                static_cast<char>(i), freq, nullptr, nullptr, nextOrder++
            )));
        }
    }
    if (list.size() == 1) {
        HuffmanNode* singleNode = list.head().node_;
        list.removeHead();
        root_ = new HuffmanNode('\0', singleNode->freq_, singleNode, nullptr);
        generateCodes(root_, "");
        return;
    }
    while (list.size() > 1) {
        HuffmanNode* firstNode = list.head().node_;
        list.removeHead();
        HuffmanNode* secondNode = list.head().node_;
        list.removeHead();
        HuffmanNode* parent = new HuffmanNode(
            '\0', firstNode->freq_ + secondNode->freq_, firstNode, secondNode, nextOrder++
        );
        list.insert(NodeWrapper(parent));
    }
    if (list.size() == 1) {
        root_ = list.head().node_;
        list.removeHead();
    }
    if (root_) {
        generateCodes(root_, "");
    }
}
MyString HuffmanCoder::getCode(unsigned char ch) const {
    char key[2] = { static_cast<char>(ch), '\0' };
    MyString code;
    if (codesTable_.find(key, code)) {
        return code;
    }
    return MyString("");
}
void HuffmanCoder::decodeFromFile(std::ifstream& inFile, std::ofstream& outFile) {
    if (!root_) return;
    HuffmanNode* current = root_;
    unsigned char ch;
    while (inFile.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
        for (int i = 7; i >= 0; --i) {
            int bit = (ch >> i) & 1;
            if (bit == 0) {
                current = current->left_;
            } else {
                current = current->right_;
            }
            if (current && !current->left_ && !current->right_) {
                outFile.put(current->symbol_);
                current = root_;
            }
        }
    }
}
MyString HuffmanCoder::encode(const MyString& text) const {
    MyString encodedStr = "";
    for (std::size_t i = 0; i < text.length(); ++i) {
        unsigned char uc = static_cast<unsigned char>(text[i]);
        char key[2] = { static_cast<char>(uc), '\0' };
        MyString code;
        if (codesTable_.find(key, code)) {
            encodedStr = encodedStr + code;
        }
    }
    return encodedStr;
}
MyString HuffmanCoder::decode(const MyString& encodedText) const {
    MyString decodedStr = "";
    if (!root_) return decodedStr;
    HuffmanNode* current = root_;
    for (std::size_t i = 0; i < encodedText.length(); ++i) {
        if (encodedText[i] == '0') current = current->left_;
        else if (encodedText[i] == '1') current = current->right_;
        if (current && !current->left_ && !current->right_) {
            char sym[2] = { current->symbol_, '\0' };
            decodedStr = decodedStr + sym;
            current = root_;
        }
    }
    return decodedStr;
}
void HuffmanCoder::printCodes() const {
    std::cout << "\n--- ТАБЛИЦА КОДОВ ХАФФМАНА ---" << "\n";
    codesTable_.print();
    std::cout << "-----------------------------\n" << "\n";
}

#endif