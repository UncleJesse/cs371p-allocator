// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <cstring>   // strcmp
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
    typedef typename A::pointer         pointer;
};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
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
        x.deallocate(p, s);
    }
}

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
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestAllocator2, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

// ---------------------
// Allocator constructor
// ---------------------

// test constructor's placement of sentinels
TEST(TestAllocator2, constructor_1) {
    Allocator<int, 110> x;
    ASSERT_EQ(x[0], 102);
    ASSERT_EQ(x[106], 102);
}

TEST(TestAllocator2, constructor_2) {
    Allocator<int, 99> x;
    ASSERT_EQ(x[0], 91);
    ASSERT_EQ(x[95], 91);
}

// test whether constructor throws bad_alloc exception
TEST(TestAllocator2, constructor_3) {
    try {
        Allocator<int, 11> x;

        // I know this assertion's a little weird, but if Allocator throws
        //  bad_alloc then control will never reach it and the test will pass
        ASSERT_EQ(1, 0);
    }
    catch (std::bad_alloc& e) {
        // will only reach here if a bad_alloc exception is thrown
        ASSERT_EQ(1, 1);
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

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
};

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
        x.deallocate(p, s);
    }
}

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

template <typename A>
struct TestAllocator4 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
};

typedef testing::Types<Allocator<int, 100>>
        my_types_3;

TYPED_TEST_CASE(TestAllocator4, my_types_3);

// test if allocate finds first fit
TYPED_TEST(TestAllocator4, test_allocate_1) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s1 = 1;
    const difference_type s2 = 1;
    const difference_type s3 = 1;
    const difference_type s4 = 1;
    const difference_type s5 = 1;
    const difference_type s6 = 1;
    const difference_type s7 = 1;
    const difference_type s8 = 1;
    const value_type      v  = 2;
    const pointer         p1 = x.allocate(s1);
    const pointer         p2 = x.allocate(s2);
    const pointer         p3 = x.allocate(s3);
    const pointer         p4 = x.allocate(s4);
    const pointer         p5 = x.allocate(s5);

    x.deallocate(p2, s2);
    x.deallocate(p3, s3);
    x.deallocate(p5, s5);

    const pointer         p6 = x.allocate(s6);
    const pointer         p7 = x.allocate(s7);
    const pointer         p8 = x.allocate(s8);

    ASSERT_EQ(p6, p2);
    ASSERT_EQ(p7, p3);
    ASSERT_EQ(p8, p5);
}

// test if allocate throws bad_alloc exception if out of space
TYPED_TEST(TestAllocator4, test_allocate_2) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s1 = 1;
    const value_type      v  = 2;
    const pointer         p1 = x.allocate(s1);
    const pointer         p2 = x.allocate(s1);
    const pointer         p3 = x.allocate(s1);
    const pointer         p4 = x.allocate(s1);
    const pointer         p5 = x.allocate(s1);
    const pointer         p6 = x.allocate(s1);
    const pointer         p7 = x.allocate(s1);
    const pointer         p8 = x.allocate(s1);

    try {
        const pointer     p10 = x.allocate(s1);
        ASSERT_EQ(1, 0);
    }
    catch (std::bad_alloc& e) {
        ASSERT_EQ(1, 1);
    }
}


// check both!
TYPED_TEST(TestAllocator4, test_allocate_3) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s1 = 1;
    const value_type      v  = 2;
    const pointer         p1 = x.allocate(s1);
    const pointer         p2 = x.allocate(s1);
    const pointer         p3 = x.allocate(s1);
    const pointer         p4 = x.allocate(s1);
    const pointer         p5 = x.allocate(s1);
    const pointer         p6 = x.allocate(s1);
    const pointer         p7 = x.allocate(s1);
    const pointer         p8 = x.allocate(s1);

    x.deallocate(p2, s1);
    x.deallocate(p3, s1);
    x.deallocate(p5, s1);
    x.deallocate(p8, s1);

    const pointer         p9  = x.allocate(s1);
    const pointer         p10 = x.allocate(s1);
    const pointer         p11 = x.allocate(s1);
    const pointer         p12 = x.allocate(s1);

    ASSERT_EQ(p2, p9);
    ASSERT_EQ(p3, p10);
    ASSERT_EQ(p5, p11);
    ASSERT_EQ(p8, p12);

    try {
        const pointer     p13 = x.allocate(s1);
        ASSERT_EQ(1, 0);
    }
    catch (std::bad_alloc& e) {
        ASSERT_EQ(1, 1);
    }
}

// test coalescence of deallocated block before a newly-deallocated block
TEST(TestAllocator4, test_deallocate_1) {
    typedef          Allocator<int, 100>                  allocator_type;
    typedef typename Allocator<int, 100>::difference_type difference_type;
    typedef typename Allocator<int, 100>::pointer         pointer;

          allocator_type  x;
    const difference_type s1 = 1;

    x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[12], 80);

    const pointer         p2 = x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[24], 68);

    const pointer         p3 = x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);

    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);

    ASSERT_EQ(x[24], -4);
    ASSERT_EQ(x[32], -4);

    ASSERT_EQ(x[36], -4);
    ASSERT_EQ(x[44], -4);

    ASSERT_EQ(x[48], -4);
    ASSERT_EQ(x[56], -4);

    ASSERT_EQ(x[60], -4);
    ASSERT_EQ(x[68], -4);

    ASSERT_EQ(x[72], -4);
    ASSERT_EQ(x[80], -4);

    ASSERT_EQ(x[84], -8);
    ASSERT_EQ(x[96], -8);


    x.deallocate(p2, s1);
    ASSERT_EQ(x[12], 4);

    x.deallocate(p3, s1);
    ASSERT_EQ(x[12], 16);
}

// test coalescence of deallocated block after a newly-deallocated block
TEST(TestAllocator4, test_deallocate_2) {
    typedef          Allocator<int, 100>                  allocator_type;
    typedef typename Allocator<int, 100>::difference_type difference_type;
    typedef typename Allocator<int, 100>::pointer         pointer;

          allocator_type  x;
    const difference_type s1 = 1;

    x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], 80);
    ASSERT_EQ(x[96], 80);

    const pointer         p2 = x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);

    ASSERT_EQ(x[24], 68);
    ASSERT_EQ(x[96], 68);

    const pointer         p3 = x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);
    x.allocate(s1);

    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);

    ASSERT_EQ(x[24], -4);
    ASSERT_EQ(x[32], -4);

    ASSERT_EQ(x[36], -4);
    ASSERT_EQ(x[44], -4);

    ASSERT_EQ(x[48], -4);
    ASSERT_EQ(x[56], -4);

    ASSERT_EQ(x[60], -4);
    ASSERT_EQ(x[68], -4);

    ASSERT_EQ(x[72], -4);
    ASSERT_EQ(x[80], -4);

    ASSERT_EQ(x[84], -8);
    ASSERT_EQ(x[96], -8);


    x.deallocate(p3, s1);
    ASSERT_EQ(x[24], 4);
    ASSERT_EQ(x[32], 4);

    x.deallocate(p2, s1);
    ASSERT_EQ(x[12], 16);
    ASSERT_EQ(x[32], 16);
}

// test coalescence of deallocated blocks before and after a newly-deallocated block
TEST(TestAllocator4, test_deallocate_3) {
    typedef          Allocator<int, 100>                  allocator_type;
    typedef typename Allocator<int, 100>::difference_type difference_type;
    typedef typename Allocator<int, 100>::pointer         pointer;

          allocator_type  x;
    const difference_type s1 = 1;

    const pointer         p1 = x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], 80);
    ASSERT_EQ(x[96], 80);

    const pointer         p2 = x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);

    ASSERT_EQ(x[24], 68);
    ASSERT_EQ(x[96], 68);

    const pointer         p3 = x.allocate(s1);
    const pointer         p4 = x.allocate(s1);
    const pointer         p5 = x.allocate(s1);
    const pointer         p6 = x.allocate(s1);
    const pointer         p7 = x.allocate(s1);
    const pointer         p8 = x.allocate(s1);

    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);

    ASSERT_EQ(x[24], -4);
    ASSERT_EQ(x[32], -4);

    ASSERT_EQ(x[36], -4);
    ASSERT_EQ(x[44], -4);

    ASSERT_EQ(x[48], -4);
    ASSERT_EQ(x[56], -4);

    ASSERT_EQ(x[60], -4);
    ASSERT_EQ(x[68], -4);

    ASSERT_EQ(x[72], -4);
    ASSERT_EQ(x[80], -4);

    ASSERT_EQ(x[84], -8);
    ASSERT_EQ(x[96], -8);


    x.deallocate(p3, s1);
    ASSERT_EQ(x[24], 4);
    ASSERT_EQ(x[32], 4);

    ASSERT_EQ(x[36], -4);
    ASSERT_EQ(x[44], -4);


    x.deallocate(p1, s1);
    ASSERT_EQ(x[0], 4);
    ASSERT_EQ(x[8], 4);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);

    ASSERT_EQ(x[24], 4);
    ASSERT_EQ(x[32], 4);

    ASSERT_EQ(x[36], -4);
    ASSERT_EQ(x[44], -4);


    x.deallocate(p2, s1);
    ASSERT_EQ(x[0], 28);
    ASSERT_EQ(x[32], 28);

    ASSERT_EQ(x[36], -4);
    ASSERT_EQ(x[44], -4);


    x.deallocate(p8, s1);
    x.deallocate(p6, s1);
    ASSERT_EQ(x[0], 28);
    ASSERT_EQ(x[32], 28);

    ASSERT_EQ(x[60], 4);
    ASSERT_EQ(x[68], 4);

    ASSERT_EQ(x[72], -4);
    ASSERT_EQ(x[80], -4);

    ASSERT_EQ(x[84], 8);
    ASSERT_EQ(x[96], 8);


    x.deallocate(p7, s1);
    ASSERT_EQ(x[0], 28);
    ASSERT_EQ(x[32], 28);

    ASSERT_EQ(x[60], 32);
    ASSERT_EQ(x[96], 32);


    x.deallocate(p5, s1);
    ASSERT_EQ(x[0], 28);
    ASSERT_EQ(x[32], 28);

    ASSERT_EQ(x[36], -4);
    ASSERT_EQ(x[44], -4);

    ASSERT_EQ(x[48], 44);
    ASSERT_EQ(x[96], 44);


    x.deallocate(p4, s1);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

// test if deallocate throws invalid_argument
TEST(TestAllocator4, test_deallocate_4) {
    typedef          Allocator<int, 100>                  allocator_type;
    typedef typename Allocator<int, 100>::difference_type difference_type;
    typedef typename Allocator<int, 100>::pointer         pointer;

          allocator_type  x;
    const difference_type s1 = 1;

    x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], 80);
    ASSERT_EQ(x[96], 80);

    x.allocate(s1);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);

    ASSERT_EQ(x[24], 68);
    ASSERT_EQ(x[96], 68);

    const pointer         p3 = reinterpret_cast<const pointer>(&x[25]);
    try {
        x.deallocate(p3, s1);
        // if this assertion runs, the error was not caught and the test should fail
        ASSERT_EQ(1, 0);
    }
    catch (std::invalid_argument& e) {
        ASSERT_TRUE(strcmp(e.what(), "invalid pointer sent to deallocate") == 0);
    }

    const pointer         p4 = reinterpret_cast<const pointer>(&x[24]);
    try {
        x.deallocate(p4, s1);
        // if this assertion runs, the error was not caught and the test should fail
        ASSERT_EQ(1, 0);
    }
    catch (std::invalid_argument& e) {
        ASSERT_TRUE(strcmp(e.what(), "invalid pointer sent to deallocate") == 0);
    }
}
