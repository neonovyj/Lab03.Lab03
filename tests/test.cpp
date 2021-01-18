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

TEST(DefaultFeatures, MovingConstructor) {//конструктор перемещения
SharedPtr<int> ptr(new int(123));//создаем умный указетель из некого встроенного указателя

auto tmp = ptr.get();
EXPECT_TRUE(std::is_move_constructible<SharedPtr<int>>::value);
SharedPtr<int> ptr2(std::move(ptr));

EXPECT_EQ(ptr.get(), nullptr);
EXPECT_EQ(ptr.use_count(), 0);
EXPECT_EQ(ptr2.use_count(), 1);
EXPECT_EQ(ptr2.get(), tmp);
}

TEST(DefaultFeatures, Destructor) {
int *ptr = new int(1234);
SharedPtr<int> sharedPtr(ptr);
{
SharedPtr<int> sharedPtr1(sharedPtr);
EXPECT_EQ(sharedPtr1.use_count(), 2);
EXPECT_EQ(sharedPtr1.use_count(), sharedPtr.use_count());
}
EXPECT_EQ(sharedPtr.use_count(), 1);
}

TEST(DefaultFeatures, CopyingOperator) {
SharedPtr<int> ptr1(new int(123));
auto ptr2 = ptr1;
EXPECT_EQ(ptr1.use_count(), ptr2.use_count());
EXPECT_EQ(ptr1.get(), ptr2.get());
}

TEST(DefaultFeatures, MovingOperator) {
EXPECT_TRUE(std::is_move_assignable<SharedPtr<int>>::value);
SharedPtr<int> ptr1(new int(123));
auto ptr1_obj = ptr1.get();
auto ptr1_use_count = ptr1.use_count();

auto ptr2 = std::move(ptr1);
EXPECT_FALSE(ptr1);
EXPECT_EQ(ptr1.use_count(), 0);

EXPECT_EQ(ptr2.get(), ptr1_obj);
EXPECT_EQ(ptr2.use_count(), ptr1_use_count);
}

TEST(DefaultFeatures, ResetTest1) {
SharedPtr<int> sharedPtr(new int(1234));
EXPECT_EQ(sharedPtr.use_count(), 1);
EXPECT_TRUE(sharedPtr);

sharedPtr.reset();
EXPECT_FALSE(sharedPtr);
EXPECT_EQ(sharedPtr.use_count(), 0);
}

TEST(DefaultFeatures, ResetTest2) {
int *ptr = new int(123);
int *ptr2 = new int(456);

SharedPtr<int> sharedPtr1(ptr);
SharedPtr<int> sharedPtr2(sharedPtr1);

EXPECT_EQ(sharedPtr1.get(), ptr);
EXPECT_EQ(sharedPtr2.get(), ptr);
EXPECT_EQ(sharedPtr2.use_count(), 2);
EXPECT_EQ(sharedPtr1.use_count(), 2);

sharedPtr2.reset(ptr2);
EXPECT_EQ(sharedPtr2.get(), ptr2);
EXPECT_EQ(sharedPtr1.use_count(), 1);
EXPECT_EQ(sharedPtr2.use_count(), 1);
}
