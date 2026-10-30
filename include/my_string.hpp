#ifndef MY_STRING
#define MY_STRING

#include <iostream>
#include <stdexcept>

class MyString {
    char* data_;
    std::size_t length_;
    std::size_t capacity_;
    void allocateAndCopy(const char* str, std::size_t len);
    void swap(MyString& other) noexcept;
    void resizeIfNeeded(std::size_t additional_length);

public:
    MyString();
    MyString(const char* str);
    MyString(const MyString& other);
    MyString(MyString&& other) noexcept;
    ~MyString();
    std::size_t length() const;
    bool empty() const;
    void clear();
    MyString& operator=(const MyString& other);
    MyString& operator=(MyString&& other) noexcept;
    MyString& operator=(const char* str);
    char& operator[](std::size_t index);
    const char& operator[](std::size_t index) const;
    friend std::ostream& operator<<(std::ostream& os, const MyString& str);
    MyString operator+(const MyString& other) const;
    MyString& operator+=(const MyString& other);
    MyString& operator+=(const char* str);
    bool operator<(const MyString& other) const;
    bool operator>(const MyString& other) const;
    bool operator==(const MyString& other) const;
    bool operator!=(const MyString& other) const;
    MyString& operator+=(char ch);
    MyString trimAndRemoveDuplicateSpaces() const;
};
void MyString::allocateAndCopy(const char* str, std::size_t len) {
    capacity_ = len + 1;
    data_ = new char[capacity_];
    length_ = len;
    for (std::size_t i = 0; i < len; ++i) {
        data_[i] = str[i];
    }
    data_[len] = '\0';
}
void MyString::swap(MyString& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(length_, other.length_);
    std::swap(capacity_, other.capacity_);
}
void MyString::resizeIfNeeded(std::size_t additional_length) {
    std::size_t new_length = length_ + additional_length;
    if (new_length + 1 > capacity_) {
        std::size_t new_capacity = std::max(capacity_ * 2, new_length + 1);
        char* new_data = new char[new_capacity];
        for (std::size_t i = 0; i < length_; ++i) {
            new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
}
MyString::MyString() : data_(new char[1]), length_(0), capacity_(1) {
    data_[0] = '\0';
}
MyString::MyString(const char* str) {
    if (!str) {
        data_ = new char[1];
        data_[0] = '\0';
        length_ = 0;
        capacity_ = 1;
    } else {
        std::size_t len = 0;
        while (str[len] != '\0') {
            ++len;
        }
        allocateAndCopy(str, len);
    }
}
MyString::MyString(const MyString& other) {
    allocateAndCopy(other.data_, other.length_);
}
MyString::MyString(MyString&& other) noexcept
    : data_(other.data_), length_(other.length_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.length_ = 0;
    other.capacity_ = 0;
}
MyString::~MyString() {
    delete[] data_;
}
std::size_t MyString::length() const {
    return length_;
}
bool MyString::empty() const {
    return length_ == 0;
}
void MyString::clear() {
    delete[] data_;
    data_ = new char[1];
    data_[0] = '\0';
    length_ = 0;
    capacity_ = 1;
}
MyString& MyString::operator=(const MyString& other) {
    if (this != &other) {
        MyString temp(other);
        swap(temp);
    }
    return *this;
}
MyString& MyString::operator=(MyString&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        length_ = other.length_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.length_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}
MyString& MyString::operator=(const char* str) {
    if (!str) {
        MyString temp;
        swap(temp);
    } else {
        MyString temp(str);
        swap(temp);
    }
    return *this;
}
char& MyString::operator[](std::size_t index) {
    return data_[index];
}
const char& MyString::operator[](std::size_t index) const {
    return data_[index];
}
std::ostream& operator<<(std::ostream& os, const MyString& str) {
    os << str.data_;
    return os;
}
MyString MyString::operator+(const MyString& other) const {
    MyString result;
    result.length_ = length_ + other.length_;
    result.capacity_ = result.length_ + 1;
    result.data_ = new char[result.capacity_];
    for (std::size_t i = 0; i < length_; ++i) {
        result.data_[i] = data_[i];
    }
    for (std::size_t i = 0; i < other.length_; ++i) {
        result.data_[length_ + i] = other.data_[i];
    }
    result.data_[result.length_] = '\0';
    return result;
}
MyString& MyString::operator+=(const MyString& other) {
    resizeIfNeeded(other.length_);
    for (std::size_t i = 0; i < other.length_; ++i) {
        data_[length_ + i] = other.data_[i];
    }
    length_ += other.length_;
    data_[length_] = '\0';
    return *this;
}
MyString& MyString::operator+=(const char* str) {
    if (!str) return *this;
    std::size_t other_len = 0;
    while (str[other_len] != '\0') {
        ++other_len;
    }
    resizeIfNeeded(other_len);
    for (std::size_t i = 0; i < other_len; ++i) {
        data_[length_ + i] = str[i];
    }
    length_ += other_len;
    data_[length_] = '\0';
    return *this;
}
bool MyString::operator<(const MyString& other) const {
    std::size_t min_len = std::min(length_, other.length_);
    for (std::size_t i = 0; i < min_len; ++i) {
        if (data_[i] != other.data_[i]) {
            return data_[i] < other.data_[i];
        }
    }
    return length_ < other.length_;
}
bool MyString::operator>(const MyString& other) const {
    return other < *this;
}
bool MyString::operator==(const MyString& other) const {
    if (length_ != other.length_) return false;
    for (std::size_t i = 0; i < length_; ++i) {
        if (data_[i] != other.data_[i]) {
            return false;
        }
    }
    return true;
}
bool MyString::operator!=(const MyString& other) const {
    return !(*this == other);
}
MyString& MyString::operator+=(char ch) {
    resizeIfNeeded(1);
    data_[length_] = ch;
    length_++;
    data_[length_] = '\0';
    return *this;
}
MyString MyString::trimAndRemoveDuplicateSpaces() const {
    if (length_ == 0) {
        return MyString();
    }
    std::size_t start = 0;
    while (start < length_ && data_[start] == ' ') {
        start++;
    }
    std::size_t end = length_ - 1;
    while (end > start && data_[end] == ' ') {
        end--;
    }
    if (start > end) {
        return MyString();
    }
    MyString result;
    bool lastWasSpace = false;
    for (std::size_t i = start; i <= end; i++) {
        if (data_[i] == ' ') {
            if (!lastWasSpace) {
                result += ' ';
                lastWasSpace = true;
            }
        } else {
            result += data_[i];
            lastWasSpace = false;
        }
    }
    return result;
}
#endif