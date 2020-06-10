"Copyright [2019] <Copyright owner>"
#ifndef INCLUDE_SHAREDPTR_HPP_
#define INCLUDE_SHAREDPTR_HPP_
#include <atomic>
#include <iostream>
#include <utility>

class Control_Block {          //класс, содержащий методы для подсчета ссылок
    std::int counter;  //объявление самого счетчика
public:
    Control_Block() :counter(0) {}
    //присваивание счетчику значения 0 через конструктор
    void add() {
        // метод инкрементирования счетчика
        counter++;
    }
    void del() { // метод декриментирования счетчика
        counter--;
    }
    bool empty() {
        // метод проверки того факта, что в счетчике хранится 1 ссылка
        if (counter == 1) return true;
        return false;
    }
    int value_counter() { // метод отображения значения счетчика(кол-во ссылок)
        return counter;
    }
    ~Control_Block() {} // деструктор, ничего не несет в себе
};

template <typename T>
class SharedPtr {
public:
    T* ptr;
    Control_Block* control_block; //указатель на объект класса Control_Block
    SharedPtr() :ptr(nullptr), control_block(nullptr) {}
    //если конструктор подается без данных,
    // то ptr и control_block - пустые указатели
    SharedPtr(const SharedPtr&);
    //конструктор принимающий на вход ссылку на константный
    // объект класса SharedPtr
    SharedPtr<T>& operator = (const SharedPtr<T>&); //перегрузка оператора =
    explicit SharedPtr(const T);
    // консткуктор, принимающий на вход параметр шаблона
    SharedPtr(SharedPtr&& r);

    void reset();
    void reset(T*);
    void swap(SharedPtr&);
    T* get() const;
    size_t use_count() const;
    T& operator*() const;
    T* operator->() const;
    operator bool() const;
    ~SharedPtr();
};

template <typename T> // Конструктор копирования
SharedPtr<T>::SharedPtr(const SharedPtr& shared_ptr) { //
    ptr = shared_ptr.get();
    control_block = shared_ptr.control_block;
    control_block->add();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator = (const SharedPtr<T>& shared_ptr) {
    control_block->del();
    if (control_block->empty()) {
        delete ptr;
        delete control_block;
        ptr = shared_ptr.get();
        control_block = shared_ptr.control_block;
        control_block->add();
        return *this;
    }
    ptr = shared_ptr.get();
    control_block = shared_ptr.control_block;
    control_block->add();
    return *this;
}
template<typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& r){
    *this = std::move(r);
} // конструктор перемещения

template <typename T>
SharedPtr<T>::SharedPtr(const T value) {
    ptr = new T(value);
    control_block = new Control_Block();
    control_block->add();
} 

template <typename T>
void SharedPtr<T>::reset() {
    if (control_block->empty()) {
        delete ptr;
        delete control_block;
        ptr = nullptr;
        control_block = nullptr;
    } else {
        ptr = nullptr;
        control_block = nullptr;
    }
}

template <typename T>
void SharedPtr<T>::reset(T* curr_ptr) {
    if (control_block->empty()) {
        delete ptr;
        delete control_block;
        ptr = curr_ptr;
        control_block = new Control_Block();
        control_block->add();
    } else {
        ptr = curr_ptr;
        control_block = new Control_Block();
        control_block->add();
    }
}

template <typename T> // описание функции swap
void SharedPtr<T>::swap(SharedPtr& shared_ptr) {
    std::swap(ptr, shared_ptr.ptr);
    std::swap(control_block, shared_ptr.control_block);
}

template <typename T>
// описание константной функции get(), которая возвращает
// значение указателя ptr, если он не равен 0
T* SharedPtr<T>::get() const {
    if (ptr == nullptr) return 0;
    return ptr;
}

template <typename T>
T& SharedPtr<T>::operator*() const {
    return *ptr;
}

template <typename T>
T* SharedPtr<T>::operator->() const {
    return ptr;
}

template <typename T>
size_t SharedPtr<T>::use_count() const {
    return control_block->value_counter();
}

template <typename T>
SharedPtr<T>::operator bool() const {
    if (ptr == nullptr) return false;
    return true;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (control_block != nullptr) {
        if (!control_block->empty()) {
            control_block->del();
        } else {
            delete ptr;
            delete control_block;
            ptr = nullptr;
            control_block = nullptr; }
    }
}
#endif // INCLUDE_SHAREDPTR_HPP_
