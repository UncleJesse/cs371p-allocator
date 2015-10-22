// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;};

struct OurType {
    const char* s;
    OurType() : s("123456789"){}
    OurType(int) : s("123456789"){}
    bool operator==(const OurType&)const{return true;}
    bool operator!=(const OurType&)const{return true;}
};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100>>
            //Allocator<OurType, 100>>
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    std::cout << sizeof(value_type) << std::endl;
          allocator_type  x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}

TEST(TestAllocator2, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}


// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

//------------------------
// TestAllocator4
// ---------------------


template <typename A>
struct TestAllocator4 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;};

typedef testing::Types<
            Allocator<int,  12>,
            Allocator<double, 16> >
        my_types_3;

TYPED_TEST_CASE(TestAllocator4, my_types_3);

TYPED_TEST(TestAllocator4, test_1) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;


          allocator_type  x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TEST(TestAllocator4, bad_fit_int){
    Allocator<int, 20> x;
    ASSERT_EQ(x[0], 12);
    ASSERT_EQ(x[16], 12);
    x.allocate(2);
    ASSERT_EQ(x[0], -12);
    ASSERT_EQ(x[16], -12);
}

TEST(TestAllocator4, bad_fit_double){
    Allocator<double, 20> x;
    ASSERT_EQ(x[0], 12);
    ASSERT_EQ(x[16], 12);
    x.allocate(1);
    ASSERT_EQ(x[0], -12);
    ASSERT_EQ(x[16], -12);
}

TEST(TestAllocator4, good_fit_int){
    Allocator<int, 12> x;
    ASSERT_EQ(x[0], 4);
    ASSERT_EQ(x[8], 4);
    x.allocate(1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
}

TEST(TestAllocator4, good_fit_double){
    Allocator<double, 16> x;
    ASSERT_EQ(x[0], 8);
    ASSERT_EQ(x[12], 8);
    x.allocate(1);
    ASSERT_EQ(x[0], -8);
    ASSERT_EQ(x[12], -8);
}

TEST(TestAllocator4, deallocate_int){
    Allocator<int, 12> x;
    ASSERT_EQ(x[0], 4);
    ASSERT_EQ(x[8], 4);
    int * p = x.allocate(1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    x.deallocate(p, 4);
    ASSERT_EQ(x[0], 4);
    ASSERT_EQ(x[8], 4);
}

TEST(TestAllocator4, deallocate_double){
    Allocator<double, 16> x;
    ASSERT_EQ(x[0], 8);
    ASSERT_EQ(x[12], 8);
    double* p = x.allocate(1);
    ASSERT_EQ(x[0], -8);
    ASSERT_EQ(x[12], -8);
    x.deallocate(p, 8);
    ASSERT_EQ(x[0], 8);
    ASSERT_EQ(x[12], 8);
}

TEST(TestAllocator4, deallocate_right_int){
    Allocator<int, 28> x;
    ASSERT_EQ(x[0], 20);
    ASSERT_EQ(x[24], 20);
    int * p = x.allocate(1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], 8);
    ASSERT_EQ(x[24], 8);
    x.deallocate(p, 4);
    ASSERT_EQ(x[0], 20);
    ASSERT_EQ(x[24], 20);
}

TEST(TestAllocator4, deallocate_right_double){
    Allocator<double, 32> x;
    ASSERT_EQ(x[0], 24);
    ASSERT_EQ(x[28], 24);
    double * p = x.allocate(1);
    ASSERT_EQ(x[0], -8);
    ASSERT_EQ(x[12], -8);
    ASSERT_EQ(x[16], 8);
    ASSERT_EQ(x[28], 8);
    x.deallocate(p, 8);
    ASSERT_EQ(x[0], 24);
    ASSERT_EQ(x[28], 24);
}

TEST(TestAllocator4, deallocate_left_int){
    Allocator<int, 24> x;
    ASSERT_EQ(x[0], 16);
    ASSERT_EQ(x[20], 16);
    int * p = x.allocate(1);
    int * p2 = x.allocate(1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);
    x.deallocate(p, 4);
    ASSERT_EQ(x[0], 4);
    ASSERT_EQ(x[8], 4);
    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);
    x.deallocate(p2, 4);
    ASSERT_EQ(x[0], 16);
    ASSERT_EQ(x[20], 16);
}

TEST(TestAllocator4, deallocate_left_double){
    Allocator<double, 32> x;
    ASSERT_EQ(x[0], 24);
    ASSERT_EQ(x[28], 24);
    double * loc1 = x.allocate(1);
    double * loc2 = x.allocate(1);
    ASSERT_EQ(x[0], -8);
    ASSERT_EQ(x[12], -8);
    ASSERT_EQ(x[16], -8);
    ASSERT_EQ(x[28], -8);
    x.deallocate(loc1, 8);
    ASSERT_EQ(x[0], 8);
    ASSERT_EQ(x[12], 8);
    ASSERT_EQ(x[16], -8);
    ASSERT_EQ(x[28], -8);
    x.deallocate(loc2, 8);
    ASSERT_EQ(x[0], 24);
    ASSERT_EQ(x[28], 24);
}

TEST(TestAllocator4, use_case_test){
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
    int* p1 = x.allocate(1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    int* p2 = x.allocate(1);
    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);
    ASSERT_EQ(x[24], 68);
    ASSERT_EQ(x[96], 68);    
    x.deallocate(p2, 0);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], 80);
    ASSERT_EQ(x[96], 80);
    x.deallocate(p1, 0);
}

TEST(TestAllocator4, constructor_int_test){
    Allocator<int, 20> x;
    ASSERT_EQ(x[0], 12);
    ASSERT_EQ(x[16], 12);
}

TEST(TestAllocator4, constructor_double_test){
    Allocator<double, 24> x;
    ASSERT_EQ(x[0], 16);
    ASSERT_EQ(x[20], 16);
}

TEST(TestAllocator4, bad_constructor_int_test){
    try{
        Allocator<int, 15> x;
        ASSERT_FALSE(0);
    }catch(bad_alloc &e){
        ASSERT_TRUE(1);
    }
}

TEST(TestAllocator4, bad_constructor_double_test){
    try{
        Allocator<double, 15> x;
        ASSERT_FALSE(0);
    }catch(bad_alloc &e){
        ASSERT_TRUE(1);
    }
}
