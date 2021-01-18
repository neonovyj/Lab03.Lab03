// Copyright 2020 ivan <ikhonyak@gmail.com>

#include <gtest/gtest.h>
#include <SharedPtr.hpp>
#include <type_traits>

TEST(DefaultFeatures, CopyingConstructor) { //проверяем, правильно ли работает конструктор копирования
SharedPtr<int> ptr(new int(123));// создаем некий умный указатель
SharedPtr<int> ptr2(ptr); //создаем второй умный указатель, копируя первый (ptr2 из ptr)
EXPECT_EQ(ptr.get(), ptr2.get());//сравниваем на равество значения, которые они хранят
EXPECT_EQ(ptr.use_count(), ptr2.use_count());//сравниваем счетчики ссылок( должны быть равны)
EXPECT_EQ(ptr.use_count(), 2);//сравниваем счетчик ссылок с 2, так как всего два указателя
}
//НИЖЕ создали умный указатель, переместили его в другой
TEST(DefaultFeatures, MovingConstructor) {//конструктор перемещения
SharedPtr<int> ptr(new int(123));//создаем умный указетель из некого встроенного указателя

auto tmp = ptr.get();// сохраняем значение, которое он хранит во временную переменную tmp, для последующего сравнения
EXPECT_TRUE(std::is_move_constructible<SharedPtr<int>>::value);//функция is... вернет тру, если у класса определен оператор перемещения(ожидаем тру). is move...проверяет, может ли тип быть создан с помощью операции перемещения.
SharedPtr<int> ptr2(std::move(ptr));//создаем ptr2 как объект, в который мы поместили ptr(move),
// так как мы его переместили, ожидаем, что у него нет ресурсов(ничем не владеем, nullptr) сравниваем кол-во ссылок с нулем и соответсвенно счетчик ссылок равен нулю,
EXPECT_EQ(ptr.get(), nullptr);//Этот и ниже проверяют, обнулился ли первый указатель
EXPECT_EQ(ptr.use_count(), 0);
EXPECT_EQ(ptr2.use_count(), 1);//Этот и ниже проверяют, забрал ли второй ресурсы первого
EXPECT_EQ(ptr2.get(), tmp);
}

TEST(DefaultFeatures, Destructor) { //деструктор
int *ptr = new int(1234);//создаем встроенный указатель
SharedPtr<int> sharedPtr(ptr);//создаем умный указатель
{//скорбки создают область видимости (внутри делаем sharedPtr)
SharedPtr<int> sharedPtr1(sharedPtr);// его создаем как копию, которая уже есть
EXPECT_EQ(sharedPtr1.use_count(), 2);//счетчик ссылок дожен быть равен null, так как у нас два объекта sharedPtr, которые владеют ресурсом
EXPECT_EQ(sharedPtr1.use_count(), sharedPtr.use_count());
}
EXPECT_EQ(sharedPtr.use_count(), 1);//ожидаем, что sharedPtr1 уничтожится, так как он за областью видимости.(кол-во ссылок должно быть равно 1)
}

TEST(DefaultFeatures, CopyingOperator) {
SharedPtr<int> ptr1(new int(123));//создаем умный указатель
auto ptr2 = ptr1;//присваиваем копию птр1
EXPECT_EQ(ptr1.use_count(), ptr2.use_count());//сравниваем их на равенство(кол-во ссылок и указателей должны быть равны)
EXPECT_EQ(ptr1.get(), ptr2.get());
}

TEST(DefaultFeatures, MovingOperator) { //тест, который проверяет оператор присваивания-перемещения
EXPECT_TRUE(std::is_move_assignable<SharedPtr<int>>::value);//is move...проверяет, является ли тип присваиваемым при перемещении.
SharedPtr<int> ptr1(new int(123));//создаем птр1
auto ptr1_obj = ptr1.get();//сохраняем значение его указателей в значение счетчика ссылок
auto ptr1_use_count = ptr1.use_count();//сохранили его данные, которые хранились, чтобы сравнить с птр, который мы переместили

auto ptr2 = std::move(ptr1);//перемещаем его
EXPECT_FALSE(ptr1);//ожидаем, что птр1 становится пустым, он владеет nullptr, у него украли ресурсы
EXPECT_EQ(ptr1.use_count(), 0);

EXPECT_EQ(ptr2.get(), ptr1_obj);
EXPECT_EQ(ptr2.use_count(), ptr1_use_count);
}

TEST(DefaultFeatures, ResetTest1) {//проверяет правильность работы reset()
SharedPtr<int> sharedPtr(new int(1234));//создаем объект, который не нулевой
EXPECT_EQ(sharedPtr.use_count(), 1);
EXPECT_TRUE(sharedPtr);

sharedPtr.reset();// делаем ресет и ожидаем, что он станет нулевым
EXPECT_FALSE(sharedPtr);
EXPECT_EQ(sharedPtr.use_count(), 0);
}

TEST(DefaultFeatures, ResetTest2) {//проверяем reset(аргумент)
int *ptr = new int(123);// делаем два умных указателя, один- копия другого (счетчик ссылок равен два, два умных указателя владеют одним ресурсом)
int *ptr2 = new int(456);

SharedPtr<int> sharedPtr1(ptr);
SharedPtr<int> sharedPtr2(sharedPtr1);

EXPECT_EQ(sharedPtr1.get(), ptr);//проверям, являются ли они копиями друг друга
EXPECT_EQ(sharedPtr2.get(), ptr);
EXPECT_EQ(sharedPtr2.use_count(), 2);
EXPECT_EQ(sharedPtr1.use_count(), 2);

sharedPtr2.reset(ptr2); //второй умный указатель ресетим на ptr2
EXPECT_EQ(sharedPtr2.get(), ptr2);//ожидаем, что в этом случае sraredptr2 не копия первого
EXPECT_EQ(sharedPtr1.use_count(), 1);//ссылки подсчитаются таким образом, что каждый из умных указателей останется единственным владельцем своего ресурса
EXPECT_EQ(sharedPtr2.use_count(), 1);
}
