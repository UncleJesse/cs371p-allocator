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

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);


TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
    }
}

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

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
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
// TestAllocator4
// --------------

TEST(TestAllocator4, all_1){
    Allocator<double, 48>x;
    ASSERT_EQ(x[0],40);
    ASSERT_EQ(x[44], 40);
}


TEST(TestAllocator4, all_2){
    Allocator<int, 12>x;
    ASSERT_EQ(x[0], 4);
    ASSERT_EQ(x[8], 4);
}


TEST(TestAllocator4, all_3){
    Allocator<double, 16>x;
    ASSERT_EQ(x[0],8);
    ASSERT_EQ(x[12], 8);
}

TEST(TestAllocator4, all_4){
    try{
        Allocator<int, 9> x;
        assert(false);
    }
    catch(std::bad_alloc& e){
        assert(true);
    }
}

// --------------
// TestValid
// --------------

TEST(TestValid, val_1){
    Allocator<int, 12>x;
    assert(x.valid());
}

TEST(TestValid, val_2){
    Allocator<int, 18>x;
    x.allocate(1);
    assert(x.valid());
}

TEST(TestValid, val_3){
    Allocator<int, 12>x;
    int* ptr = x.allocate(1);
    x.deallocate(ptr,1);
    assert(x.valid());
}

// ------------
// TestAllocate
// ------------
TEST(TestAllocate, allo_1){
    Allocator<double, 100> x;
    x.allocate(11);
    ASSERT_EQ(x[0], -92);
    ASSERT_EQ(x[96], -92);
}

TEST(TestAllocate, allo_2){
    Allocator<int, 100>x;
    x.allocate(5);
    x.allocate(5);
    x.allocate(5);
    ASSERT_EQ(x[0], -20);
    ASSERT_EQ(x[24], -20);
    ASSERT_EQ(x[28], -20);
    ASSERT_EQ(x[52], -20);
    ASSERT_EQ(x[56], -20);
    ASSERT_EQ(x[80], -20);
    ASSERT_EQ(x[84], 8);
    ASSERT_EQ(x[96], 8);
}

TEST(TestAllocate, allo_3){
    Allocator<int, 20>x;
    x.allocate(3);
    ASSERT_EQ(x[0],-12);
    ASSERT_EQ(x[16],-12);
}

TEST(TestAllocate, allo_4){
    try{
        Allocator<int, 100> x;
        x.allocate(100);
        assert(false);
    }
    catch(std::bad_alloc& e){
        assert(true);
    }
}

TEST(TestAllocate, allo_5) {
    try {
        Allocator<int, 100> x;
        x.allocate(0);
    }
    catch(std::bad_alloc& e) {
        assert(true);
    }
}

// --------------
// TestDeallocate
// --------------
// Perfect Deallocate
TEST(TestDeallocate, deall_1){
    Allocator<int, 100>x;
    int* ptr = x.allocate(23);
    x.deallocate(ptr,23);
    assert(x.valid());
}

// No Coalesce
TEST(TestDeallocate, deall_2){
    Allocator<int, 100>x;
    x.allocate(5);
    int* ptr2 = x.allocate(5);
    x.allocate(5);
    x.deallocate(ptr2,5);
    ASSERT_EQ(x[0], -20);
    ASSERT_EQ(x[24], -20);
    ASSERT_EQ(x[28], 20);
    ASSERT_EQ(x[52], 20);
    ASSERT_EQ(x[56], -20);
    ASSERT_EQ(x[80], -20);
    ASSERT_EQ(x[84], 8);
    ASSERT_EQ(x[96], 8);
    assert(x.valid());
}

// Coalesce Front
TEST(TestDeallocate, deall_3){
    Allocator<int, 100>x;
    int* ptr = x.allocate(5);
    int* ptr2 = x.allocate(5);
    x.allocate(5);
    x.deallocate(ptr2,5);
    x.deallocate(ptr,5);
    ASSERT_EQ(x[0], 48);
    ASSERT_EQ(x[52], 48);
    ASSERT_EQ(x[56], -20);
    ASSERT_EQ(x[80], -20);
    ASSERT_EQ(x[84], 8);
    ASSERT_EQ(x[96], 8);
    assert(x.valid());
}

// Coalesce Back
TEST(TestDeallocate, deall_4){
    Allocator<int, 100>x;
    int* ptr = x.allocate(5);
    int* ptr2 = x.allocate(5);
    x.allocate(5);
    x.deallocate(ptr,5);
    x.deallocate(ptr2,5);
    ASSERT_EQ(x[0], 48);
    ASSERT_EQ(x[52], 48);
    ASSERT_EQ(x[56], -20);
    ASSERT_EQ(x[80], -20);
    ASSERT_EQ(x[84], 8);
    ASSERT_EQ(x[96], 8);
    assert(x.valid());
}

// Coalesce Both
TEST(TestDeallocate, deall_5){
    Allocator<int, 100>x;
    int* ptr = x.allocate(5);
    int* ptr2 = x.allocate(5);
    int* ptr3 = x.allocate(5);
    x.deallocate(ptr,5);
    x.deallocate(ptr3,5);
    x.deallocate(ptr2,5);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
    assert(x.valid());
}

TEST(TestDeallocate, deall_6){
    Allocator<int, 100> x;

    int* b = x.allocate(3);
    if (b != nullptr) {
        int* e = b + 3;
        int* p = b;
        while (p != e) {
            x.construct(p, 2);
            ++p;
        }
    }
    try {
        x.deallocate(b + 1, 3);
        ASSERT_TRUE(false);
    }
    catch (std::invalid_argument& i) {
        ASSERT_TRUE(true);
    }

    ASSERT_TRUE(x.valid());
}
