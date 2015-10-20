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
 * Tests equality operators 
 */

TEST(TestAllocator2, equal_1) {
    Allocator<int, 100> x;
    Allocator<int, 100> y;
    ASSERT_EQ(x, y);
}

TEST(TestAllocator2, equal_2) {
    Allocator<int, 16> x;
    Allocator<int, 16> y;
    ASSERT_EQ(x, y);
}

TEST(TestAllocator2, equal_3) {
    Allocator<char, 9> x;
    Allocator<char, 9> y;
    ASSERT_EQ(x, y);
}

TEST(TestAllocator2, not_equal_1) {
    Allocator<int, 100> x;
    Allocator<int, 100> y;
    ASSERT_FALSE(x != y);
}

TEST(TestAllocator2, not_equal_2) {
    Allocator<int, 16> x;
    Allocator<int, 16> y;
    ASSERT_FALSE(x != y);
}

TEST(TestAllocator2, not_equal_3) {
    Allocator<char, 9> x;
    Allocator<char, 9> y;
    ASSERT_FALSE(x != y);
}

/**
 * Tests the construct and destroy functions 
 */

TEST(TestAllocator2, creation_and_destruction_1) {
    Allocator<int, 16> x;
    int* p = x.allocate(1);
    x.construct(p, 7);
    ASSERT_EQ(p[0], 7);
    x.destroy(p);
}

TEST(TestAllocator2, creation_and_destruction_2) {
    Allocator<double, 100> x;
    double* p = x.allocate(1);
    x.construct(p, 7);
    ASSERT_EQ(p[0], 7);
    x.destroy(p);
}

TEST(TestAllocator2, creation_and_destruction_3) {
    Allocator<char, 9> x;
    char* p = x.allocate(1);
    x.construct(p, 7);
    ASSERT_EQ(p[0], 7);
    x.destroy(p);
}
    
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
    const Allocator<int, 100>& y = x;
    int* p = x.allocate(23);
    ASSERT_EQ (p, &y[4]);
    ASSERT_EQ (y[0], -92); //uses the pointer to check sentinel
    ASSERT_EQ (y[96], -92);
}

TEST(TestAllocator2, allocate_2) {
    Allocator<int, 12> x;
    const Allocator<int, 12>& y = x;
    int* p = x.allocate(1);
    ASSERT_EQ (p, &y[4]);
    ASSERT_EQ (y[0], -4);
    ASSERT_EQ (y[8], -4);
}

TEST(TestAllocator2, allocate_3) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    int* p = x.allocate(3);
    ASSERT_EQ (p, &y[4]);
    ASSERT_EQ (y[0], -12);
    ASSERT_EQ (y[16], -12);
    ASSERT_EQ (y[20], 72);
    ASSERT_EQ (y[96], 72);
    }

TEST(TestAllocator2, allocate_4) {
    Allocator<int, 60> x;
    const Allocator<int, 60>& y = x;
    x.allocate(3);
    int* k = x.allocate(3);
    x.allocate(3);
    x.deallocate(k, 3);
    int* b = x.allocate(3);
    ASSERT_EQ (b, &y[24]);
    ASSERT_EQ (b, k);
    ASSERT_EQ (y[20], -12);
    ASSERT_EQ (y[36], -12);
    }

TEST(TestAllocator2, allocate_coalesce) {
    //shows that allocate coalesces blocks that are too small to be free
    Allocator<int, 16> x;
    const Allocator<int, 16>& y = x;
    int* p = x.allocate(1);
    ASSERT_EQ (p, &y[4]);
    ASSERT_EQ (y[12], -8);
    ASSERT_EQ (y[0], -8);
}

TEST(TestAllocator2, allocate_multiple) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    x.allocate(4);
    x.allocate(2);   
    x.allocate(5);
    int* p = x.allocate(5); //this should coalesce remaining space
    ASSERT_EQ (p, &y[72]);
    ASSERT_EQ (y[0], -16);
    ASSERT_EQ (y[20], -16);
    ASSERT_EQ (y[24], -8);
    ASSERT_EQ (y[36], -8);
    ASSERT_EQ (y[40], -20);
    ASSERT_EQ (y[64], -20);
    ASSERT_EQ (y[68], -24);
    ASSERT_EQ (y[96], -24);
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

TEST(TestAllocator2, allocate_first_fit_1) { //allocates at deallocated address instead of previously unallocated space
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    x.allocate(3);
    int* k = x.allocate(3);
    x.allocate(3);
    x.deallocate(k, 3);
    int* b = x.allocate(3);
    ASSERT_EQ (b, &y[24]);
    ASSERT_EQ (b, k);
    ASSERT_EQ (y[0], -12);
    ASSERT_EQ (y[16], -12);
    ASSERT_EQ (y[20], -12);
    ASSERT_EQ (y[36], -12);
    ASSERT_EQ (y[40], -12);
    ASSERT_EQ (y[56], -12);
    ASSERT_EQ (y[60], 32);
    ASSERT_EQ (y[96], 32);
}

TEST(TestAllocator2, allocate_first_fit_2) { //allocates at deallocated address with correct fit
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    x.allocate(3);
    int* k = x.allocate(2);
    x.allocate(3);
    int* q = x.allocate(3);
    x.allocate(3);
    x.deallocate(k, 2);
    x.deallocate(q, 3);
    int* b = x.allocate(3);
    ASSERT_EQ (b, &y[60]);
    ASSERT_EQ (y[0], -12);
    ASSERT_EQ (y[16], -12);
    ASSERT_EQ (y[20], 8);
    ASSERT_EQ (y[32], 8);
    ASSERT_EQ (y[36], -12);
    ASSERT_EQ (y[52], -12);
    ASSERT_EQ (y[56], -12);
    ASSERT_EQ (y[72], -12);
    ASSERT_EQ (y[76], -16);
    ASSERT_EQ (y[96], -16);
    }

/**
 * Tests the deallocate function 
 */

TEST(TestAllocator2, deallocate_1) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    int* p = x.allocate(23);
    x.deallocate(p, 1);
    ASSERT_EQ(y[0], 92);
}

TEST(TestAllocator2, deallocate_2) {
    Allocator<int, 12> x;
    const Allocator<int, 12>& y = x;
    int* p = x.allocate(1);
    x.deallocate(p, 1);
    ASSERT_EQ(y[0], 4);
}

TEST(TestAllocator2, deallocate_coalesce_front_1) {
    Allocator<int, 68> x;
    const Allocator<int, 68>& y = x;
    int* p = x.allocate(5);
    int* k = x.allocate(8);
    x.deallocate(k, 8);
    x.deallocate(p, 5);
    ASSERT_EQ(y[0], 60);
    ASSERT_EQ(y[24], 0);
    ASSERT_EQ(y[28], 0);
    ASSERT_EQ(y[64], 60);
}

TEST(TestAllocator2, deallocate_coalesce_front_2) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    x.allocate(4);
    int* p = x.allocate(2);   
    int* k = x.allocate(5);
    x.allocate(5); //this should coalesce remaining space
    x.deallocate(k, 1);
    x.deallocate(p, 1);
    ASSERT_EQ (y[0], -16);
    ASSERT_EQ (y[20], -16);
    ASSERT_EQ (y[24], 36);
    ASSERT_EQ (y[36], 0);
    ASSERT_EQ (y[40], 0);
    ASSERT_EQ (y[64], 36);
    ASSERT_EQ (y[68], -24);
    ASSERT_EQ (y[96], -24);
}

TEST(TestAllocator2, deallocate_coalesce_behind_1) {
    Allocator<int, 36> x;
    const Allocator<int, 36>& y = x;
    int* p = x.allocate(2);
    int* k = x.allocate(3);
    x.deallocate(p, 2);
    x.deallocate(k, 3);
    ASSERT_EQ(y[0], 28);
    ASSERT_EQ(y[12], 0);
    ASSERT_EQ(y[16], 0);
    ASSERT_EQ(y[32], 28);
}

TEST(TestAllocator2, deallocate_coalesce_behind_2) {
    Allocator<char, 100> x;
    const Allocator<char, 100>& y = x;
    x.allocate(16);
    char* p = x.allocate(8);   
    char* k = x.allocate(20);
    x.allocate(20); //this should coalesce remaining space
    x.deallocate(p, 1);
    x.deallocate(k, 1);
    ASSERT_EQ (y[0], -16);
    ASSERT_EQ (y[20], -16);
    ASSERT_EQ (y[24], 36);
    ASSERT_EQ (y[36], 0);
    ASSERT_EQ (y[40], 0);
    ASSERT_EQ (y[64], 36);
    ASSERT_EQ (y[68], -24);
    ASSERT_EQ (y[96], -24);
}

TEST(TestAllocator2, deallocate_coalesce_both_sides) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    int* p = x.allocate(2);
    int* k = x.allocate(3);
    int* l = x.allocate(1);
    x.deallocate(p, 2);
    x.deallocate(l, 1);
    x.deallocate(k, 3);
    ASSERT_EQ(y[0], 92);
    ASSERT_EQ(y[12], 0);
    ASSERT_EQ(y[16], 0);
    ASSERT_EQ(y[32], 0);
    ASSERT_EQ(y[36], 0);
    ASSERT_EQ(y[44], 0);
    ASSERT_EQ(y[96], 92);
}

TEST(TestAllocator2, deallocate_invalid_1) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    x.allocate(4);
    x.allocate(2);   
    int* p = x.allocate(5);
    x.allocate(5);
    ++p;
    try {
        x.deallocate(p, 5);
        ASSERT_TRUE(false);
    }
    catch(const std::invalid_argument& e){ 
        ASSERT_EQ(strcmp(e.what(), "Pointer p is invalid"), 0); 
    }
    ASSERT_EQ (p, &y[48]);
    ASSERT_EQ (y[0], -16);
    ASSERT_EQ (y[20], -16);
    ASSERT_EQ (y[24], -8);
    ASSERT_EQ (y[36], -8);
    ASSERT_EQ (y[40], -20);
    ASSERT_EQ (y[64], -20);
    ASSERT_EQ (y[68], -24);
    ASSERT_EQ (y[96], -24);
}

TEST(TestAllocator2, deallocate_invalid_2) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    x.allocate(4);
    x.allocate(2);   
    int* p = x.allocate(5);
    x.allocate(5);
    x.deallocate(p, 5);
    try {
        x.deallocate(p, 5);
        ASSERT_TRUE(false);
    }
    catch(const std::invalid_argument& e){ 
        ASSERT_EQ(strcmp(e.what(), "Pointer p is invalid"), 0); 
    }
    ASSERT_EQ (p, &y[44]);
    ASSERT_EQ (y[0], -16);
    ASSERT_EQ (y[20], -16);
    ASSERT_EQ (y[24], -8);
    ASSERT_EQ (y[36], -8);
    ASSERT_EQ (y[40], 20);
    ASSERT_EQ (y[64], 20);
    ASSERT_EQ (y[68], -24);
    ASSERT_EQ (y[96], -24);
}

TEST(TestAllocator2, deallocate_invalid_3) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    x.allocate(4);
    x.allocate(2);   
    int* p = x.allocate(5);
    x.allocate(5);
    try {
        x.deallocate(p - 1, 5);
        ASSERT_TRUE(false);
    }
    catch(const std::invalid_argument& e){ 
        ASSERT_EQ(strcmp(e.what(), "Pointer p is invalid"), 0); 
    }
    ASSERT_EQ (p, &y[44]);
    ASSERT_EQ (y[0], -16);
    ASSERT_EQ (y[20], -16);
    ASSERT_EQ (y[24], -8);
    ASSERT_EQ (y[36], -8);
    ASSERT_EQ (y[40], -20);
    ASSERT_EQ (y[64], -20);
    ASSERT_EQ (y[68], -24);
    ASSERT_EQ (y[96], -24);
}

TEST(TestAllocator2, deallocate_invalid_4) {
    Allocator<int, 100> x;
    const Allocator<int, 100>& y = x;
    int* p = x.allocate(4);
    x.allocate(2);   
    x.allocate(5);
    x.allocate(5);
    try {
        x.deallocate(p - 1, 5);
        ASSERT_TRUE(false);
    }
    catch(const std::invalid_argument& e){ 
        ASSERT_EQ(strcmp(e.what(), "Pointer p is out of bounds"), 0); 
    }
    ASSERT_EQ (p, &y[4]);
    ASSERT_EQ (y[0], -16);
    ASSERT_EQ (y[20], -16);
    ASSERT_EQ (y[24], -8);
    ASSERT_EQ (y[36], -8);
    ASSERT_EQ (y[40], -20);
    ASSERT_EQ (y[64], -20);
    ASSERT_EQ (y[68], -24);
    ASSERT_EQ (y[96], -24);
}

/**
 * Tests the valid function by making an invalid array and catching the exceptions upon the destroy function calling valid
 */

TEST(TestAllocator2, valid_1) {
    try{
        Allocator<int, 12> x;
        int* p = x.allocate(1);
        p[-1] = 0; 
        x.destroy(p);
        ASSERT_TRUE(false);
    }
    catch(const std::logic_error& e){ 
        ASSERT_EQ(strcmp(e.what(), "Invalid sentinal value"), 0); 
    }
}

TEST(TestAllocator2, valid_2) {
    try{
        Allocator<int, 12> x;
        int* p = x.allocate(1);
        p[1] = 1; 
        x.destroy(p);
        ASSERT_TRUE(false);
    }
    catch(const std::logic_error& e){ 
        ASSERT_EQ(strcmp(e.what(), "Sentinels don't match"), 0); 
    }
}

TEST(TestAllocator2, valid_3) {
    try{
        Allocator<double, 16> x;
        double* q = x.allocate(1);
        int* p = reinterpret_cast<int*>(&q[0]);
        x.deallocate(q, 1);
        p[-1] = 4; 
        p[1] = 4;
        x.destroy(q);
        ASSERT_TRUE(false);
    }
    catch(const std::logic_error& e){ 
        ASSERT_EQ(strcmp(e.what(), "Block too small"), 0); 
    }
}

TEST(TestAllocator2, valid_4) {
    try{
        Allocator<int, 12> x;
        int* p = x.allocate(1);
        x.deallocate(p, 1);
        p[1] = 1; 
        x.destroy(p);
        ASSERT_TRUE(false);
    }
    catch(const std::logic_error& e){ 
        ASSERT_EQ(strcmp(e.what(), "Sentinels not matching"), 0); 
    }
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
            Allocator<char,    100>,
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
