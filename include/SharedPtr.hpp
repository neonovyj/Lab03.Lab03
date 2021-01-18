// Copyright 2020 ivan <ikhonyak@gmail.com>

#ifndef INCLUDE_SHAREDPTR_HPP_
#define INCLUDE_SHAREDPTR_HPP_
#include <atomic> //предоставляют возможность форсировать "передачу" изменений данных в другой поток
#include <utility>
//Атомарная операция — это операция, которую невозможно наблюдать в промежуточном состоянии, она либо выполнена либо нет. Атомарные операции могут состоять из нескольких операций.
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

  operator bool() const; //Нулевой ли указатель ил нет
  auto operator*() const -> T&;//Оператор разыменования
  auto operator->() const -> T*;//Похож на метод get

  auto get() -> T*;//возвращает указатель
  void reset();//сбрасывает наш указатель на нулевой. Проверяет, что делать с ресурсом, кот уже владеем, если мы последние владельцы ресурса, чистим его, если не последние, уменьшает счетчик ссылок и выходит из права владения, зануляет себя
  void reset(T* ptr);//то же самое, что  и reset, только не зануляет себя, а инициализирует себя ptr и счетчик инициализирует единицей
  void swap(SharedPtr& r);//Меняет местами ресурсы одного метода от одного объекта с ресурсами объекта, кот мы передаем
  auto use_count() const -> std::size_t;//счетчик ссылок, возвращает кол-во ссылок, кот ссылаюся на данный ресурс

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
    : object(r.object), counter(r.counter) { //копируем ресурс и адрес счетчика ссылок
  if (counter != nullptr) (*counter)++;
}
template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& r) : object(nullptr), counter(nullptr) {
  std::swap(object, r.object);
  std::swap(counter, r.counter);
}
template <typename T>
SharedPtr<T>::~SharedPtr() { //вначеле делаем две проверки:
  if (counter == nullptr) return; //является ли наш объект владельцем какого либо ресура (не равен ли counter nullptr)
  (*counter)--; //уменьшаем значения из counter на единицу
  if (*counter == 0) { //Если counter обратился в ноль
    delete counter;//удаляем все
    delete object;
  }
}

template <typename T>
auto SharedPtr<T>::operator=(const SharedPtr& r) -> SharedPtr& {//определяет, что делать с тем ресурсом, кот мы владеем.( уничтожить самому или отдать другим умным указателям)
  if (*this == r) return *this;//проверяем, не присваиваем ли мы себя себе. Тогда просто возвращаем ммылку на самого себя
  reset();
  counter = r.counter;//r - ссылка на объект из которого мы будем производить копирование
  object = r.object; //объект из которого вызывается присваивание = объект, который передается как r (аргумент)
  if (counter != nullptr) (*counter)++; //Если counter не nllptr, увеличиваем счетчик на единицу
  return *this; //возвращаем ссылку на самого себя, чтобы можно было сделать множественное присваивание
}

template <typename T>
auto SharedPtr<T>::operator=(SharedPtr&& r) -> SharedPtr& {// оператор присваивания перемещения
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
template <typename T> //Вызываем метод reset. Если counter - nullptr(ничем не владеем), просто выходим. Если чем то владеем, мы уменьшаем счетчик ссылок на 1, отказываясь от владения ресурсами, и смотрим, остались ли владельцы этих ресурсов. Если не остались, то чистим память от ресурса и счетчика и зануляем. А если еще есть владельцы(умные указатели, владеющие ресурсами), то себя зануляем, а их не трогаем
void SharedPtr<T>::reset() { //определяет, что делать с ресурсом, кот уже владеем, а потом обнуляет себя
  if (counter == nullptr) return; //смотрим, владеем ли мы чем либо, если они равны, то выходим просто
  --(*counter); //уже знаем, что counter не nllptr, там есть значение, то разыменовываем его и уменьшаем на единицу
  if (*counter == 0) { //сравниваем, если значение из counter 0, то
    delete counter; //
    delete object; //
  }
  counter = nullptr; //так как отказываемя от права владения ресурсом, обнуляем все
  object = nullptr;
}

template <typename T>
void SharedPtr<T>::reset(T* ptr) {
  reset();//вызываем reset без аргументов, кот сбросит наш указатель в нулевое состояние
  object = ptr;//инициализируем object ptrом(войдем в право владения нулевым объектом)
  counter = new std::atomic_uint(1);//Делаем новый счетчик. Инициализируем новый счетчик типа атомарного uinta единицей.
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
