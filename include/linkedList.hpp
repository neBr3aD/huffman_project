#ifndef LINKED_LIST
#define LINKED_LIST

#include <iostream>
#include <utility>
#include <stdexcept>

template <typename T>
class DoubleOrderedList{
private:
    struct Node
    {
        T data_;
        Node* next_;
        Node* prev_;
        Node(const T& data, Node* next = nullptr, Node* prev = nullptr): data_(data), next_(next), prev_(prev){}
        Node(T&& data, Node* next = nullptr, Node* prev = nullptr): data_(std::move(data)), next_(next), prev_(prev){}
    };
    std::size_t size_;
    Node* tail_;
    Node* head_;
    void swap(DoubleOrderedList& other) noexcept;
    template <typename U>
    void insert_internal(U&& value);
public:
    DoubleOrderedList(): size_(0), tail_(nullptr), head_(nullptr){}
    DoubleOrderedList(const T& value);
    DoubleOrderedList(const DoubleOrderedList& other);
    DoubleOrderedList(DoubleOrderedList&& other) noexcept;
    ~DoubleOrderedList();
    DoubleOrderedList& operator=(const DoubleOrderedList& other);
    DoubleOrderedList& operator=(DoubleOrderedList&& other) noexcept;
    void clear();
    void insert(const T& value) {
        insert_internal(value);
    }
    void insert(T&& value) {
        insert_internal(std::move(value));
    }
    bool search(const T& key) const;
    T& head();
    T& tail();
    void removeHead();
    std::size_t size() const { return size_; }
    bool isEmpty() const { return size_ == 0; }
    void print() const;
    void removeKey(const T& keyBegin, const T& keyEnd);
    bool operator==(const DoubleOrderedList& other) const;
    void merge(DoubleOrderedList& other);
    void remove(const DoubleOrderedList& other);
};
template <typename T>
void DoubleOrderedList<T>::swap(DoubleOrderedList& other) noexcept {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(size_, other.size_);
}
template <typename T>
template <typename U>
void DoubleOrderedList<T>::insert_internal(U&& value) {
    if (!head_) {
        head_ = tail_ = new Node(std::forward<U>(value));
        size_++;
        return;
    }
    Node* current = head_;
    while (current && current->data_ < value) {
        current = current->next_;
    }
    if (current && current->data_ == value) {
        return;
    }
    Node* newNode = new Node(std::forward<U>(value), current, current ? current->prev_ : tail_);
    if (current) {
        if (current->prev_) {
            current->prev_->next_ = newNode;
        } else {
            head_ = newNode;
        }
        current->prev_ = newNode;
    } else {
        tail_->next_ = newNode;
        tail_ = newNode;
    }
    size_++;
}
template <typename T>
void DoubleOrderedList<T>::clear(){
    auto current = head_;
    while (current) {
        auto next = current->next_;
        delete current;
        current = next;
    }
    head_ = tail_ = nullptr;
    size_ = 0;
}
template <typename T>
DoubleOrderedList<T>::DoubleOrderedList(const T& value): size_(0), tail_(nullptr), head_(nullptr) {
    insert(value);
}
template <typename T>
DoubleOrderedList<T>::DoubleOrderedList(const DoubleOrderedList& other): size_(0), tail_(nullptr), head_(nullptr){
    auto current = other.head_;
    while (current)
    {
        insert(current->data_);
        current = current->next_;
    }
}
template <typename T>
DoubleOrderedList<T>::DoubleOrderedList(DoubleOrderedList&& other) noexcept: size_(other.size_), tail_(other.tail_), head_(other.head_){
    other.head_ = other.tail_ = nullptr;
    other.size_ = 0;
}
template <typename T>
DoubleOrderedList<T>::~DoubleOrderedList(){
    clear();
}
template <typename T>
DoubleOrderedList<T>& DoubleOrderedList<T>::operator=(const DoubleOrderedList& other){
    if(this != &other){
        DoubleOrderedList temp(other);
        this->swap(temp);
    }
    return *this;
}
template <typename T>
DoubleOrderedList<T>& DoubleOrderedList<T>::operator=(DoubleOrderedList&& other) noexcept {
    if (this !=&other){
        clear();
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}
template <typename T>
bool DoubleOrderedList<T>::search(const T& key) const {
    auto current = head_;
    while (current) {
        if (current->data_ == key) return true;
        if (current->data_ > key) return false;
        current = current->next_;
    }
    return false;
}
template <typename T>
T& DoubleOrderedList<T>::head(){
    if(!head_){
        throw std::runtime_error("List is empty");
    }
    return head_->data_;
}
template <typename T>
T& DoubleOrderedList<T>::tail(){
    if(!tail_){
        throw std::runtime_error("List is empty");
    }
    return tail_->data_;
}
template <typename T>
void DoubleOrderedList<T>::removeHead() {
    if (!head_) return;
    auto temp = head_;
    head_ = head_->next_;
    if (head_) {
        head_->prev_ = nullptr;
    } else {
        tail_ = nullptr;
    }
    delete temp;
    size_--;
}
template <typename T>
void DoubleOrderedList<T>::print() const {
    auto current = head_;
    while (current) {
        std::cerr << current->data_ << " ";
        current = current->next_;
    }
    std::cerr << "\n";
}
template <typename T>
void DoubleOrderedList<T>::removeKey(const T& keyBegin, const T& keyEnd){
    if (keyBegin > keyEnd || !head_) return;
    auto current = head_;
    while (current && current->data_ < keyBegin){
        current = current->next_;
    }
    while (current && current->data_ <= keyEnd){
        auto toDelete = current;
        current = current->next_;
        if (toDelete->prev_){
            toDelete->prev_->next_ = toDelete->next_;
        } else{
            head_ = toDelete->next_;
        }
        if(toDelete->next_){
            toDelete->next_->prev_ = toDelete->prev_;
        }else{
            tail_ = toDelete->prev_;
        }
        delete toDelete;
        size_--;
    }
}
template <typename T>
void DoubleOrderedList<T>::merge(DoubleOrderedList& other){
    if (this == &other || !other.head_) return;
    auto currentThis = head_;
    auto currentOther = other.head_;
    while (currentOther)
    {
        auto nextOther = currentOther->next_;
        while (currentThis && currentThis->data_ <currentOther->data_)
        {
            currentThis = currentThis->next_;
        }
        if (currentThis && currentThis->data_ == currentOther->data_){
            delete currentOther;
        }else{
            currentOther->next_ = currentThis;
            if (currentThis)
            {
                currentOther->prev_ = currentThis->prev_;
                if(currentThis->prev_) {
                    currentThis->prev_->next_ = currentOther;
                } else{
                    head_ = currentOther;
                }
                currentThis->prev_ = currentOther;
            }else{
                currentOther->prev_ = tail_;
                if (tail_) {
                    tail_->next_ = currentOther;
                }else{
                    head_ = currentOther;
                }
                tail_ = currentOther;
            }
            size_++;
        }
        currentOther = nextOther;
    }
    other.head_ = other.tail_ = nullptr;
    other.size_ = 0;
}
template <typename T>
void DoubleOrderedList<T>::remove(const DoubleOrderedList& other) {
    auto currentThis = head_;
    auto currentOther = other.head_;
    while (currentThis && currentOther) {
        if (currentThis->data_ < currentOther->data_) {
            currentThis = currentThis->next_;
        } else if (currentThis->data_ > currentOther->data_) {
            currentOther = currentOther->next_;
        } else {
            auto toDelete = currentThis;
            currentThis = currentThis->next_;
            if (toDelete->prev_) toDelete->prev_->next_ = toDelete->next_;
            else head_ = toDelete->next_;
            if (toDelete->next_) toDelete->next_->prev_ = toDelete->prev_;
            else tail_ = toDelete->prev_;
            delete toDelete;
            size_--;
            currentOther = currentOther->next_;
        }
    }
}
template <typename T>
bool DoubleOrderedList<T>::operator==(const DoubleOrderedList& other) const {
    if (size_ != other.size_) return false;
    auto currentThis = head_;
    auto currentOther = other.head_;
    while (currentOther && currentThis)
    {
        if (currentThis->data_ != currentOther->data_) return false;
        currentThis = currentThis->next_;
        currentOther = currentOther->next_;
    }
    return true;
}
#endif