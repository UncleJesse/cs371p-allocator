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
         * takes the absolute value of the sentinel, goes forward by that value, checks that the next spot is a sentinel and
         * that it is equal to the former sentinel
         */
        bool valid () const {
            int i = 0;
            while (i < N){
                int s = (*this)[i];
                if (s == 0){ // invalid sentinel value
                    throw std::logic_error("Invalid sentinal value");
                    return false;
                }
                else if (s < 0) {  //skips past allocated block
                    int r = (*this)[i - s + sizeof(int)];
                    if (s != r){
                        throw std::logic_error("Sentinels don't match");
                        return false; // make sure that both sentinels match
                    }
                    i += -s + (2 * sizeof(int)); // increment i past second sentinel, to the sentinel of the next block
                }
                //checks unallocated block to make sure that there is enough 
                //space for two sentinels and at least 1 T
                else if (s > 0) {  
                    if (s < sizeof(T)){ // if block is too small, return false, block is invalid
                        throw std::logic_error("Block too small");
                        return false;
                    }
                    else{
                        int q = (*this)[i + s + sizeof(int)];
                        if(s != q){
                            throw std::logic_error("Sentinels not matching");
                            return false; // both sentinals for this block should match
                        }
                        i += s + (2 * sizeof(int)); // increment i to next block
                    }
                }
                // i should not be greater than N, once we've accounted for all blocks, i should be equal to N
                if (i > N){ 
                    throw std::logic_error("Out of bounds");
                    return false;
                }
            }
            return true;
        }

        /**
         * O(1) in space
         * O(1) in time
         * Unused in the tests we made. Takes the four bytes after index in a and returns an int
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
            //sentinels equal amount of space between them
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
            if (n == 0) { //doesn't allocate space and returns null
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
                    else {  //allocates space and changes sentinels to match the new space remaining
                        (*this)[i] = -n * sizeof(T);
                        (*this)[i + n * sizeof(T) + sizeof(int)] = -n * sizeof(T);
                        (*this)[i + n * sizeof(T) + 2 * sizeof(int)] = s - n * sizeof(T) - (2 * sizeof(int));
                        (*this)[i + s + sizeof(int)] = s - n * sizeof(T) - (2 * sizeof(int));
                    }
                    
                    not_done = false;
                }
            }
            if (i == N)  //this means there wasn't enough room for allocation
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
         * Upon receiving the pointer, the function checks the sentinels to see that they're valid.
         * Then it deallocates the block and coalesces free space on either side
         */
        void deallocate (pointer p, size_type n) {
            int b = (int)((char*)p - a); //gets index of pointer in allocator

            if (b < sizeof(int) || b >= N - sizeof(int)){  //sees that pointer p is within a[N]

                throw std::invalid_argument("Pointer p is out of bounds");
            }

            int& sentinel_1 = (*this)[b - 4];
            int e = b + -sentinel_1;
            //checks that sentinel_2 is within a[N] and sentinel_1 is negative
            if (e > N || e < b || sentinel_1 >= 0) {  
                throw std::invalid_argument("Pointer p is invalid");
            }
            int& sentinel_2 = (*this)[e];

            if (sentinel_1 >= 0 || sentinel_2 >= 0 || sentinel_1 != sentinel_2){ 
                throw std::invalid_argument("Pointer p is invalid");
            }

            if (b > sizeof(int)  && (*this)[b - 2 * sizeof(int)] > 0 && e < (N - sizeof(int)) && (*this)[e + sizeof(int)] > 0) { //coalesces free blocks on both sides
                int oe = b - 2 * sizeof(int);
                int ob = e + sizeof(int);
                int& sentinel_l = (*this)[oe];
                int& sentinel_r = (*this)[ob];
                int v = sentinel_l + sentinel_r + -sentinel_1 + 4 * sizeof(int);
                (*this)[oe - sentinel_l - sizeof(int)] = v;
                (*this)[ob + sentinel_r + sizeof(int)] = v;
                sentinel_l = 0;
                sentinel_r = 0;
                sentinel_1 = 0;
                sentinel_2 = 0;
            }

            else if (b > sizeof(int)  && (*this)[b - 2 * sizeof(int)] > 0) { //coalesces free block behind
                int oe = b - 2 * sizeof(int);
                int& sentinel_3 = (*this)[oe];
                int& sentinel_4 = (*this)[oe - sentinel_3 - sizeof(int)];
                int v = -sentinel_1 + sentinel_3 + 2 * sizeof(int);
                sentinel_4 = v;
                sentinel_2 = v;
                sentinel_3 = 0;
                sentinel_1 = 0;
            }

            else if (e < (N - sizeof(int)) && (*this)[e + sizeof(int)] > 0) //coalesces free block in front
            {
                int ob = e + sizeof(int);
                int& sentinel_3 = (*this)[ob];
                int& sentinel_4 = (*this)[ob + sizeof(int) + sentinel_3];
                int v = -sentinel_1 + sentinel_3 + 2 * sizeof(int);
                sentinel_1 = v;
                sentinel_4 = v;
                sentinel_3 = 0;
                sentinel_2 = 0;
            }

            else {
                sentinel_1 = -sentinel_1;
                sentinel_2 = -sentinel_2;
            } 

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
