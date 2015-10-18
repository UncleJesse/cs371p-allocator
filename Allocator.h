// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <string>
#include "gtest/gtest_prod.h"

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
            
            return true;}

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestAllocator2, index);
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------
        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         */
        Allocator () {
            if (N < sizeof(T) + (2 * sizeof(int)))
            {
                throw std::bad_alloc();
            }
            (*this)[0] = N - (2 * sizeof(int));
            (*this)[N - sizeof(int)] = N - (2 * sizeof(int));
            assert(valid());}

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type n) {
            // can't allocate negative sized blocks, throw bad_alloc
            if (n < 0) {
                throw std::bad_alloc();
            }
            if (n == 0) {
                return nullptr;
            }
            int i = 0;
            bool not_done = true;
            while (i < N && not_done) {
                int s = (*this)[i];
                if (s < 0) {  //skips past allocated block
                    i += -s + (2 * sizeof(int));
                }
                else if (s < (n * sizeof(T))) {  //skips past blocks that are too small
                    i += s + (2 * sizeof(int));
                }
                else {  //acually allocates space
                    if (s - ( n * sizeof(T) ) < (2 * sizeof(int) + sizeof(T)))  //coalesces blocks that are too small
                    {
                        (*this)[i] = -s;
                        (*this)[i + s + sizeof(int)] = -s;
                    }
                    else if (s - n == 0)  //simply adapts all the space at the sentinel
                    {
                        (*this)[i] = -s;
                        (*this)[i + s + sizeof(int)] = -s;
                    }
                    else {  
                        (*this)[i] = -n * sizeof(T);
                        (*this)[i + n * sizeof(T) + sizeof(int)] = -n * sizeof(T);
                        (*this)[i + n * sizeof(T) + 2 * sizeof(int)] = s - n * sizeof(T) - (2 * sizeof(int));
                        (*this)[i + s + sizeof(int)] = s - n * sizeof(T) - (2 * sizeof(int));
                    }
                    
                    not_done = false;
                }
            }
            if (i == N)
            {
                throw std::bad_alloc();
            }

            assert(valid());
            return  reinterpret_cast<T*>(&a[i + sizeof(int)]);
        }         

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * <your documentation>
         */
        void deallocate (pointer p, size_type n) {
            int b = (int)((char*)p - a); //gets index of pointer in allocator

            if (b < sizeof(int) || b >= N - sizeof(int)){  //sees that pointer p is within a[N]

                throw std::invalid_argument("Pointer p is invalid");
            }

            int& sentinel_1 = (*this)[b - 4];
            int e = b + -sentinel_1;
            //checks that sentinel_2 is in a[N] and sentinel_1 is negative
            if (e > N || e < b || sentinel_1 >= 0) {  
                throw std::invalid_argument("Pointer p is invalid");
            }
            int& sentinel_2 = (*this)[e];
            std::cout << b << " " << e << std::endl;
            if (sentinel_1 >= 0 || sentinel_2 >= 0 || sentinel_1 != sentinel_2){
                throw std::invalid_argument("Pointer p is invalid");
            }

            /*if (b > sizeof(int) && (*this)[b - 2 * sizeof(int)] > 0) { //coalesces free block behind
                int q = b - 2 * sizeof(int);
                int& sentinel_3 = (*this)[q];
                int& sentinel_4 = (*this)[q - sentinel_3 - sizeof(int)];
                int v = -sentinel_2 + sentinel_4 + 2 * sizeof(int);
                sentinel_2 = v;
                sentinel_4 = v;
                sentinel_3 = 0;
                sentinel_1 = 0;
            }
            
            else if (e < (N - sizeof(int)) && (*this)[e + sizeof(int)] > 0) { //coalesces free block in front
                int& sentinel_3 = (*this)[e + sizeof(int)];
                int v = sentinel_3 + (-sentinel_1) + 2 * sizeof(int);
                sentinel_1 = v;
                int& sentinel_4 = (*this)[b + (-sentinel_1) + 2* sizeof(int) + sentinel_3];
                sentinel_4 = v;
                sentinel_2 = 0;
                sentinel_3 = 0;
            }
            else {
                sentinel_1 = -sentinel_1;
                sentinel_2 = -sentinel_2;
            }*/

            //std::cout << b << std::endl;
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * It takes the four bytes on top of the reference, turns it into an
         * int, and returns a contant alias to the value at the address
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
