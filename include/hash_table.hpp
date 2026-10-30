#ifndef HASH_TABLE
#define HASH_TABLE

#include <iostream>
#include <stdexcept>
#include <cstring>

template <typename T>
class HashTable {
private:
    struct Node{
        char* key_;
        T value_;
        Node* next_;

        Node(const char* key, const T& value, Node* next = nullptr) : value_(value), next_(next) {
        key_ = new char[strlen(key) + 1];
        strcpy(key_, key);
        }
        ~Node(){
            delete[] key_;
        }
    };
    std::size_t size_;
    Node** table_;
    std::size_t number_;
    std::size_t collision_;
    std::size_t hashFunction(const char* key) const;
public:
    HashTable();
    HashTable(std::size_t size);
    HashTable(const HashTable& other) = delete;
    HashTable(HashTable&& other) noexcept = delete;
    HashTable& operator=(const HashTable& other) = delete;
    HashTable& operator=(HashTable&& other) noexcept = delete;
    ~HashTable();

    bool insert(const char* key, const T& value);
    bool remove(const char* key);
    bool find(const char* key, T& value) const;
    void print() const;
    void clear();
    std::size_t getSize() const {
        return size_;
    }
    std::size_t getCollisions() const{
        return collision_;
    }
    std::size_t getBucketLength(std::size_t index) const;
};
template <typename T>
HashTable<T>::HashTable() : size_(11), number_(0), collision_(0) {
    table_ = new Node*[size_];
    for (std::size_t i = 0; i < size_; ++i) {
        table_[i] = nullptr;
    }
}

template <typename T>
HashTable<T>::HashTable(std::size_t size){
    size_ = size;
    number_ = 0;
    collision_ = 0;
    table_ = new Node*[size_];
    for (std::size_t i = 0; i<size_; ++i){
        table_[i] = nullptr;
    }
}

template <typename T>
HashTable<T>::~HashTable(){
    for (std::size_t i = 0; i<size_; ++i){
        Node* current = table_[i];
        while (current!=nullptr)
        {
            Node* to_delete = current;
            current = current->next_;
            delete to_delete;
        }
    }
    delete[] table_;
}
template <typename T>
size_t HashTable<T>::hashFunction(const char* key) const {
    unsigned long hash = 5381;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(key);
    while (*p != '\0') {
        hash = ((hash << 5) + hash) + *p;
        p++;
    }
    return hash % size_;
}
template <typename T>
bool HashTable<T>::insert(const char* key, const T& value){
    std::size_t index = hashFunction(key);
    Node* current = table_[index];
    while (current != nullptr) {
        if (strcmp(current->key_, key) == 0) {
            throw std::runtime_error("Duplicate key");
        }
        current = current->next_;
    }
    if (table_[index] != nullptr) {
        collision_++;
        std::cout << "COLLISION at bucket " << index << "!\n";
        std::cout << "  Existing chain: ";
        Node* walk = table_[index];
        while (walk != nullptr) {
            std::cout << "[" << walk->key_ << ":" << walk->value_ << "]";
            if (walk->next_ != nullptr) std::cout << " → ";
            walk = walk->next_;
        }
        std::cout << "\n";
        std::cout << "  Adding: [" << key << ":" << value << "] to the beginning\n";
        std::cout << "  New chain length: " << (getBucketLength(index) + 1) << "\n\n";
    }
    Node* newNode = new Node(key, value, table_[index]);
    table_[index] = newNode;
    number_++;
    return true;
}

template <typename T>
bool HashTable<T>::remove(const char* key){
    std::size_t index = hashFunction(key);
    Node* current = table_[index];
    Node* prev = nullptr;

    while (current!=nullptr)
    {
        if(strcmp(current->key_, key) == 0){
            if(prev == nullptr){
                table_[index] = current->next_;
            }else{
                prev->next_ = current->next_;
            }
            delete current;
            number_--;
            return true;
        }
        prev = current;
        current = current->next_;
    }
    return false;
}

template <typename T>
bool HashTable<T>::find(const char* key, T& value) const {
    std::size_t index = hashFunction(key);
    Node* current = table_[index];
    while (current!=nullptr)
    {
        if(strcmp(current->key_, key) == 0){
            value = current->value_;
            return true;
        }
        current = current->next_;
    }
    return false;
}

template <typename T>
void HashTable<T>::print() const {
    for (size_t i = 0; i < size_; ++i) {
        std::cout << "Bucket " << i << ": ";
        Node* current = table_[i];
        if (current == nullptr) {
            std::cout << "empty";
        }
        while (current != nullptr) {
            std::cout << "[" << current->key_ << ": " << current->value_ << "] -> ";
            current = current->next_;
        }
        std::cout << "nullptr" << "\n";
    }
}
template <typename T>
std::size_t HashTable<T>::getBucketLength(std::size_t index) const {
    std::size_t len = 0;
    Node* current = table_[index];
    while (current) {
        len++;
        current = current->next_;
    }
    return len;
}
template <typename T>
void HashTable<T>::clear() {
    for (std::size_t i = 0; i < size_; ++i) {
        Node* current = table_[i];
        while (current != nullptr) {
            Node* toDelete = current;
            current = current->next_;
            delete toDelete;
        }
        table_[i] = nullptr;
    }
    number_ = 0;
    collision_ = 0;
}

#endif