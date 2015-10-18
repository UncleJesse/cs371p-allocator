// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <cstring>   // strcmp
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
    typedef typename A::pointer    pointer;
};

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
        x.deallocate(p, s);
    }
}

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
                x.destroy(p);}
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
    ASSERT_EQ(x[0], 92);
}

TEST(TestAllocator2, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
}
    
// --------------
// The remaining of TestAllocator2 tests 
// my Allocator class's functions
// --------------

/**
 * Tests the default constructor 
 */
TEST(TestAllocator2, default_constructor) {
    const Allocator<int, 200> x;
    //checks sentinels
    ASSERT_EQ(x[0], 192);
    ASSERT_EQ(x[196], 192);
}

TEST(TestAllocator2, default_constructor_smallest) {
    const Allocator<int, 12> x;
    ASSERT_EQ(x[0], 4);
    ASSERT_EQ(x[8], 4);
}

TEST(TestAllocator2, default_constructor_odd) {
    const Allocator<int, 13> x;
    ASSERT_EQ(x[0], 5);
    ASSERT_EQ(x[9], 5);
}

TEST(TestAllocator2, default_constructor_bad_alloc) {
    try {
    const Allocator<int, 11> x; //should not get to the next line
    ASSERT_TRUE(false);}
    catch(const std::bad_alloc& e){ 
        ASSERT_EQ(strcmp(e.what(), "std::bad_alloc"), 0); 
    }
}

/**
 * Tests the allocate function 
 */

TEST(TestAllocator2, allocate_1) {
    Allocator<int, 100> x;
    Allocator<int, 100>& y = x;
    int* p = x.allocate(23);
    ASSERT_EQ (p, &y[0])
    ASSERT_EQ (p[-1], -92); //uses the pointer to check sentinel
    ASSERT_EQ (p[23], -92);
}

TEST(TestAllocator2, allocate_2) {
    Allocator<int, 12> x;
    int* p = x.allocate(1);
    ASSERT_EQ (p[1], -4);
    ASSERT_EQ (p[-1], -4);
}

TEST(TestAllocator2, allocate_3) {
    Allocator<int, 100> x;
    int* p = x.allocate(3);
    ASSERT_EQ (p[-1], -12);
    ASSERT_EQ (p[3], -12);

    }

TEST(TestAllocator2, allocate_coalesce) {
    //shows that allocate coalesces blocks that are too small to be free
    Allocator<int, 16> x;
    int* p = x.allocate(1);
    ASSERT_EQ (p[2], -8);
    ASSERT_EQ (p[-1], -8);
}

TEST(TestAllocator2, allocate_multiple) {
    Allocator<int, 100> x;
    int* p = x.allocate(4);
    x.allocate(2);   
    x.allocate(5);
    x.allocate(5);
    ASSERT_EQ (p[-1], -16);
    ASSERT_EQ (p[4], -16);
    ASSERT_EQ (p[5], -8);
    ASSERT_EQ (p[8], -8);
    ASSERT_EQ (p[9], -20);
    ASSERT_EQ (p[15], -20);
    ASSERT_EQ (p[16], -24);
    ASSERT_EQ (p[23], -24);
}



TEST(TestAllocator2, allocate_bad_alloc_1) {
    try {
    Allocator<int, 100> x;
    x.allocate(24);
    ASSERT_TRUE(false);}
    catch(const std::bad_alloc& e){ 
        ASSERT_EQ(strcmp(e.what(), "std::bad_alloc"), 0); 
    }
}

TEST(TestAllocator2, allocate_bad_alloc_2) {
    try {
    Allocator<int, 100> x;
    x.allocate(-1);
    ASSERT_TRUE(false);}
    catch(const std::bad_alloc& e){ 
        ASSERT_EQ(strcmp(e.what(), "std::bad_alloc"), 0); 
    }
}

TEST(TestAllocator2, allocate_bad_alloc_3) {
    try {
    Allocator<int, 100> x;
    x.allocate(4);
    x.allocate(2);   
    x.allocate(5);
    x.allocate(7);
    ASSERT_TRUE(false);}
    catch(const std::bad_alloc& e){ 
        ASSERT_EQ(strcmp(e.what(), "std::bad_alloc"), 0); 
    }
}

TEST(TestAllocator2, deallocate_1) {
    Allocator<int, 100> x;
    int* p = x.allocate(23);
    x.deallocate(p, 1);
    ASSERT_EQ(p[-1], 92);
    }

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
