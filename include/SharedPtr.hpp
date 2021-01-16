// Copyright 2020 ivan <ikhonyak@gmail.com>

#ifndef INCLUDE_SHAREDPTR_HPP_
#define INCLUDE_SHAREDPTR_HPP_
#include <atomic>
#include <utility>

template <typename T>
class SharedPtr { //Класс, управляеющий определнным ресурсом, считающий ссылки. Сам чистит память при необходимости
 public:
  SharedPtr();//Дефолтный конструктор.Конструктор, создающий пустой объект(нет ресурса, кот он владеет и нет счетчика ссылок, тк нет русурса)
  explicit SharedPtr(T* ptr);//explicit отключает неявное преобразование типов. Конструктор, кот мы передаем сырой указатель во владение, начинает распоряжаться ресурсом. Инициализируется ресурс этим ptr. Счетчик создается новый и инициализируется 1, тк мы единственные владельцы этого ptr
  SharedPtr(const SharedPtr& r);//Конструктор копирования.Создает копию объекта из уже существущего. Увеличивает счетчик ссылок. Тк мы копируем из какого то объекта, то кол-во ссылок должно увеличиться, тк добавился новый объект, управляющий этим ресурсом.
  SharedPtr(SharedPtr&& r);//Конструктор перемещения. Перемещает русурсы.
  ~SharedPtr();//Деструктор. Проверяет, является SharedPtr последним владельцем ресурса, если так, то чистит память, если нет, то уменьшает счетчик ссылок
  auto operator=(const SharedPtr& r) -> SharedPtr&;//Оператор присваивания-копирования. Реализовывает функционал, похожий на конструктор копирования, но при этом управляет ресурсом, которым он уже владеет. Есть умный указатель, уже владещий ресурсом, присваиваем ему значение нового указателя. Оператор копирования должен решить, что сделать с тем ресуром, которым уже владеем(очищать память от него или нет) , а потом скопировать в себя тот объект, который нам передается(справа от знака присваивания)
  auto operator=(SharedPtr&& r) -> SharedPtr&; //Оператор перемещения. Сначала решаем, что делать с объектом, кот мы уже владеем, потом перемещаем ресурсы из объекта, который находится справа от знака присваивания.

  operator bool() const;
  auto operator*() const -> T&;
  auto operator->() const -> T*;

  auto get() -> T*;
  void reset();
  void reset(T* ptr);
  void swap(SharedPtr& r);
  auto use_count() const -> std::size_t;

 private:
  T* object;
  std::atomic_uint* counter;
};
template <typename T>
SharedPtr<T>::SharedPtr() : object(nullptr), counter(nullptr) {}

template <typename T>
SharedPtr<T>::SharedPtr(T* ptr)
    : object(ptr), counter(new std::atomic_uint(1)) {
  if (ptr == nullptr) {
    delete counter;
    counter = nullptr;
  }
}
template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& r)
    : object(r.object), counter(r.counter) {
  if (counter != nullptr) (*counter)++;
}
template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& r) : object(nullptr), counter(nullptr) {
  std::swap(object, r.object);
  std::swap(counter, r.counter);
}
template <typename T>
SharedPtr<T>::~SharedPtr() {
  if (counter == nullptr) return;
  (*counter)--;
  if (*counter == 0) {
    delete counter;
    delete object;
  }
}

template <typename T>
auto SharedPtr<T>::operator=(const SharedPtr& r) -> SharedPtr& {
  if (*this == r) return *this;
  reset();
  counter = r.counter;
  object = r.object;
  if (counter != nullptr) (*counter)++;
  return *this;
}

template <typename T>
auto SharedPtr<T>::operator=(SharedPtr&& r) -> SharedPtr& {
  if (*this == r) return *this;
  reset();
  counter = r.counter;
  object = r.object;
  r.counter = nullptr;
  r.object = nullptr;
  return *this;
}

template <typename T>
SharedPtr<T>::operator bool() const {
  return object != nullptr;
}
template <typename T>
auto SharedPtr<T>::operator*() const -> T& {
  return *object;
}
template <typename T>
auto SharedPtr<T>::operator->() const -> T* {
  return object;
}
template <typename T>
auto SharedPtr<T>::get() -> T* {
  return object;
}
template <typename T>
void SharedPtr<T>::reset() {
  if (counter == nullptr) return;
  --(*counter);
  if (*counter == 0) {
    delete counter;
    delete object;
  }
  counter = nullptr;
  object = nullptr;
}

template <typename T>
void SharedPtr<T>::reset(T* ptr) {
  reset();
  object = ptr;
  counter = new std::atomic_uint(1);
}

template <typename T>
void SharedPtr<T>::swap(SharedPtr& r) {
  std::swap(object, r.object);
  std::swap(counter, r.counter);
}
template <typename T>
auto SharedPtr<T>::use_count() const -> std::size_t {
  if (counter == nullptr) return 0;
  return *counter;
}
#endif  // INCLUDE_HEADER_HPP_
