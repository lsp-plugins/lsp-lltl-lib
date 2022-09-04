/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 7 апр. 2020 г.
 *
 * lsp-lltl-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-lltl-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-lltl-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/lltl/darray.h>
#include <lsp-plug.in/test-fw/utest.h>

namespace
{
    static ssize_t test_int_cmp(const int *a, const int *b)
    {
        return *a - *b;
    }

    static ssize_t test_int_cmp2(const void *a, const void *b, size_t size)
    {
        const int *_a = static_cast<const int *>(a);
        const int *_b = static_cast<const int *>(b);
        return *_b - *_a;
    }
}

UTEST_BEGIN("lltl", darray)

    typedef struct large_struct_t
    {
        int data[0x1234];
    } large_struct_t;

    void dump(lltl::darray<int> &x)
    {
        for (int i=0, n=x.size(); i<n; ++i)
            printf("%d ", *x.get(i));
        printf("\n");
    }

    void check_darray(lltl::darray<int> &x, const int *numbers, int n)
    {
        int *pv;

        UTEST_ASSERT(x.size() == size_t(n));
        UTEST_ASSERT(x.capacity() >= size_t(n));
        for (int i=0; i<n; ++i)
        {
            UTEST_ASSERT_MSG(pv = x.get(i), "Failed get at index %d", i);
            UTEST_ASSERT_MSG(*pv == numbers[i], "Failed at index %d: got %d, expected %d", i, *pv, numbers[i]);
        }
        UTEST_ASSERT(x.get(n) == NULL);
    }

    void test_single()
    {
        printf("Testing single operations on darray...\n");

        lltl::darray<int> x;
        int *pv;

        // Check initial state
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);

        // Add items
        UTEST_ASSERT(pv = x.add());
        *pv = 1;
        dump(x);

        UTEST_ASSERT(pv = x.append());
        *pv = 2;
        dump(x);

        UTEST_ASSERT(!(pv = x.insert(10)));
        UTEST_ASSERT(pv = x.insert(1));
        *pv = 3;
        dump(x);

        UTEST_ASSERT(pv = x.insert(0));
        *pv = 4;
        dump(x);

        UTEST_ASSERT(pv = x.push());
        *pv = 5;
        dump(x);

        UTEST_ASSERT(pv = x.unshift());
        *pv = 6;
        dump(x);

        UTEST_ASSERT(pv = x.prepend());
        *pv = 7;
        dump(x);

        UTEST_ASSERT(pv = x.set(1, 10));
        UTEST_ASSERT(*pv == 10);
        dump(x);

        // Check items
        static const int numbers[] = { 7, 10, 4, 1, 3, 2, 5 };
        check_darray(x, numbers, sizeof(numbers)/sizeof(int));

        // Remove items
        UTEST_ASSERT(pv = x.pop());
        UTEST_ASSERT(*pv == 5);
        dump(x);

        UTEST_ASSERT(pv = x.last());
        UTEST_ASSERT(*pv == 2);
        dump(x);

        UTEST_ASSERT(x.shift());
        UTEST_ASSERT(pv = x.first());
        UTEST_ASSERT(*pv == 10);
        dump(x);

        UTEST_ASSERT(!x.remove(5));
        UTEST_ASSERT(pv = x.get(2));
        UTEST_ASSERT(*pv == 1);
        dump(x);

        UTEST_ASSERT(x.remove(2));
        UTEST_ASSERT(pv = x.get(2));
        UTEST_ASSERT(*pv == 3);
        dump(x);

        UTEST_ASSERT(!x.premove(NULL));
        UTEST_ASSERT(x.get(x.size()) == NULL);
        UTEST_ASSERT(!x.premove(x.uget(x.size())));
        UTEST_ASSERT(x.premove(pv));
        UTEST_ASSERT(pv = x.get(2));
        UTEST_ASSERT(*pv == 2);
        dump(x);

        // Check size
        UTEST_ASSERT(x.size() == 3);
        UTEST_ASSERT(x.capacity() >= 3);
        UTEST_ASSERT(pv = x.first());
        UTEST_ASSERT(*pv == 10);
        UTEST_ASSERT(pv = x.last());
        UTEST_ASSERT(*pv == 2);
    }

    void test_single_with_copy()
    {
        printf("Testing single operations on darray with copying...\n");

        lltl::darray<int> x;
        int v, *pv;

        // Check initial state
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);

        // Add items
        v = 0;
        UTEST_ASSERT(x.add(&v));
        dump(x);
        UTEST_ASSERT(x.add(1));
        dump(x);
        UTEST_ASSERT(x.add(2));
        dump(x);
        UTEST_ASSERT(x.add(3));
        dump(x);
        UTEST_ASSERT(x.size() == 4);
        UTEST_ASSERT(x.capacity() >= 4);

        // Insert items
        UTEST_ASSERT(!x.insert(100, 100));
        UTEST_ASSERT(!x.insert(100, &v));
        UTEST_ASSERT(x.size() == 4);
        UTEST_ASSERT(x.capacity() >= 4);
        v = 4;
        UTEST_ASSERT(x.insert(4, &v));
        dump(x);
        UTEST_ASSERT(x.insert(5, 5));
        dump(x);
        v = 6;
        UTEST_ASSERT(x.insert(0, &v));
        dump(x);
        UTEST_ASSERT(x.insert(1, 7));
        dump(x);
        UTEST_ASSERT(x.size() == 8);
        UTEST_ASSERT(x.capacity() >= 8);

        // Append items
        v = 8;
        UTEST_ASSERT(x.append(&v));
        UTEST_ASSERT(x.append(9));
        UTEST_ASSERT(x.size() == 10);
        UTEST_ASSERT(x.capacity() >= 10);

        // Push items
        v = 11;
        UTEST_ASSERT(x.push(&v));
        dump(x);
        UTEST_ASSERT(x.push(12));
        dump(x);
        UTEST_ASSERT(x.size() == 12);
        UTEST_ASSERT(x.capacity() >= 12);

        // Unshift items
        v = 13;
        UTEST_ASSERT(x.unshift(&v));
        dump(x);
        UTEST_ASSERT(x.unshift(14));
        dump(x);
        UTEST_ASSERT(x.size() == 14);
        UTEST_ASSERT(x.capacity() >= 14);

        // prepend items
        v = 15;
        UTEST_ASSERT(x.prepend(&v));
        dump(x);
        UTEST_ASSERT(x.prepend(16));
        dump(x);
        UTEST_ASSERT(x.size() == 16);
        UTEST_ASSERT(x.capacity() >= 16);

        // Check items
        static const int numbers[] = { 16, 15, 14, 13, 6, 7, 0, 1, 2, 3, 4, 5, 8, 9, 11, 12 };
        check_darray(x, numbers, sizeof(numbers)/sizeof(int));

        // Pop element
        pv = NULL;
        UTEST_ASSERT(pv = x.pop(&v));
        dump(x);
        UTEST_ASSERT(v == 12);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.pop(v));
        dump(x);
        UTEST_ASSERT(v == 11);
        UTEST_ASSERT(pv == &v);

        // Shift element
        pv = NULL;
        UTEST_ASSERT(pv = x.shift(&v));
        dump(x);
        UTEST_ASSERT(v == 16);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.shift(v));
        dump(x);
        UTEST_ASSERT(v == 15);
        UTEST_ASSERT(pv == &v);

        // Remove element
        pv = NULL;
        UTEST_ASSERT(pv = x.remove(6, &v));
        dump(x);
        UTEST_ASSERT(v == 2);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.remove(1, v));
        dump(x);
        UTEST_ASSERT(v == 13);
        UTEST_ASSERT(pv == &v);

        // Remove pointer
        pv = NULL;
        UTEST_ASSERT(pv = x.premove(x.get(5), &v));
        dump(x);
        UTEST_ASSERT(v == 3);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.premove(x.get(2), v));
        dump(x);
        UTEST_ASSERT(v == 7);
        UTEST_ASSERT(pv == &v);

        // Check size
        UTEST_ASSERT(x.size() == 8);
        UTEST_ASSERT(x.capacity() >= 8);
        UTEST_ASSERT(pv = x.first());
        UTEST_ASSERT(*pv == 14);
        UTEST_ASSERT(pv = x.last());
        UTEST_ASSERT(*pv == 9);

        // Flush
        x.flush();
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);
    }

    void test_multiple()
    {
        printf("Testing multiple operations on darray...\n");

        lltl::darray<int> x;
        int *pv;

        // Check initial state
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);

        // Append items
        UTEST_ASSERT(pv = x.append_n(2));
        UTEST_ASSERT(x.index_of(pv) == 0);
        UTEST_ASSERT(x.contains(pv));
        pv[0] = 0; pv[1] = 1;
        dump(x);

        // Add items
        UTEST_ASSERT(pv = x.add_n(2));
        UTEST_ASSERT(x.index_of(pv) == 2);
        UTEST_ASSERT(x.contains(pv));
        pv[0] = 2; pv[1] = 3;
        dump(x);

        // Push items
        UTEST_ASSERT(pv = x.push_n(3));
        UTEST_ASSERT(x.index_of(pv) == 4);
        UTEST_ASSERT(x.contains(pv));
        pv[0] = 4; pv[1] = 5; pv[2] = 6;
        dump(x);

        // Unshift items
        UTEST_ASSERT(pv = x.unshift_n(3));
        UTEST_ASSERT(x.index_of(pv) == 0);
        UTEST_ASSERT(x.contains(pv));
        pv[0] = 7; pv[1] = 8; pv[2] = 9;
        dump(x);

        // Prepend items
        UTEST_ASSERT(pv = x.prepend_n(2));
        UTEST_ASSERT(x.index_of(pv) == 0);
        UTEST_ASSERT(x.contains(pv));
        pv[0] = 10; pv[1] = 11;
        dump(x);

        // Insert items
        UTEST_ASSERT(pv = x.insert_n(4, 4));
        UTEST_ASSERT(x.index_of(pv) == 4);
        UTEST_ASSERT(x.contains(pv));
        pv[0] = 12; pv[1] = 13; pv[2] = 14; pv[3] = 15;
        dump(x);

        // Check items
        static const int numbers[] = { 10, 11, 7, 8, 12, 13, 14, 15, 9, 0, 1, 2, 3, 4, 5, 6 };
        check_darray(x, numbers, sizeof(numbers)/sizeof(int));

        // Pop items
        UTEST_ASSERT(!x.pop_n(17));
        UTEST_ASSERT(pv = x.pop_n(4));
        dump(x);
        UTEST_ASSERT(pv[0] == 3);
        UTEST_ASSERT(pv[1] == 4);
        UTEST_ASSERT(pv[2] == 5);
        UTEST_ASSERT(pv[3] == 6);

        // Shift items
        UTEST_ASSERT(!x.shift_n(13));
        UTEST_ASSERT(x.shift_n(4));
        dump(x);
        UTEST_ASSERT(pv = x.first());
        UTEST_ASSERT(*pv == 12);

        // Remove items
        UTEST_ASSERT(!x.remove_n(8, 1));
        UTEST_ASSERT(!x.remove_n(0, 9));
        UTEST_ASSERT(x.remove_n(2, 2));
        dump(x);
        UTEST_ASSERT(pv = x.get(2));
        UTEST_ASSERT(*pv == 9);

        // Remove pointer
        UTEST_ASSERT(!x.premove_n(x.get(1), 6));
        UTEST_ASSERT(x.premove_n(x.get(1), 3));
        dump(x);
        UTEST_ASSERT(pv = x.get(1));
        UTEST_ASSERT(*pv == 1);

        UTEST_ASSERT(x.size() == 3);
    }

    void test_multiple_with_copy()
    {
        printf("Testing multiple operations on darray with copying...\n");

        lltl::darray<int> x;
        int *pv, vv[5];

        // Check initial state
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);

        // Marker
        vv[4] = -1;

        // Set values
        vv[0] = 0; vv[1] = 1; vv[2] = 2; vv[3] = 3;
        UTEST_ASSERT(pv = x.set_n(4,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        // Append values
        vv[0] = 4; vv[1] = 5;
        UTEST_ASSERT(pv = x.append_n(2, vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 4);

        // Add values
        vv[0] = 6; vv[1] = 7; vv[2] = 8;
        UTEST_ASSERT(pv = x.append_n(3,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 6);

        // Push values
        vv[0] = 9; vv[1] = 10; vv[2] = 11;
        UTEST_ASSERT(pv = x.push_n(3,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 9);

        // Unshift values
        vv[0] = 12; vv[1] = 13;
        UTEST_ASSERT(pv = x.unshift_n(2,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        // Prepend values
        vv[0] = 14; vv[1] = 15;
        UTEST_ASSERT(pv = x.prepend_n(2,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        // Insert values
        vv[0] = 16; vv[1] = 17; vv[2] = 18; vv[3] = 19;
        UTEST_ASSERT(pv = x.insert_n(12,  4, vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 12);

        // Check items
        static const int numbers[] = { 14, 15, 12, 13, 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 8, 9, 10, 11 };
        check_darray(x, numbers, sizeof(numbers)/sizeof(int));

        // Pop items
        UTEST_ASSERT(pv = x.pop_n(4,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) < 0);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(vv[0] == 8)
        UTEST_ASSERT(vv[1] == 9)
        UTEST_ASSERT(vv[2] == 10)
        UTEST_ASSERT(vv[3] == 11)
        UTEST_ASSERT(vv[4] == -1)

        // Get items
        UTEST_ASSERT(pv = x.get_n(10, 2, vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) < 0);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(vv[0] == 6)
        UTEST_ASSERT(vv[1] == 7)
        UTEST_ASSERT(vv[2] == 10)

        // Shift items
        UTEST_ASSERT(pv = x.shift_n(3,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) < 0);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(vv[0] == 14)
        UTEST_ASSERT(vv[1] == 15)
        UTEST_ASSERT(vv[2] == 12)
        UTEST_ASSERT(vv[3] == 11)

        // Remove items
        UTEST_ASSERT(pv = x.remove_n(8, 2, vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) < 0);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(vv[0] == 7)
        UTEST_ASSERT(vv[1] == 16)
        UTEST_ASSERT(vv[2] == 12)

        // Remove pointer
        UTEST_ASSERT(pv = x.premove_n(x.get(2), 2, vv));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) < 0);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(vv[0] == 1)
        UTEST_ASSERT(vv[1] == 2)
        UTEST_ASSERT(vv[2] == 12)

        // Check final size
        UTEST_ASSERT(x.size() == 9);
    }

    void test_multiple_darray()
    {
        lltl::darray<int> x, y;

        printf("Testing multiple operations on darray with another darray...\n");

        int *pv, vv[4];

        // Append
        vv[0] = 0; vv[1] = 1; vv[2] = 2; vv[3] = 3;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.append(y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        vv[0] = 4; vv[1] = 5; vv[2] = 6; vv[3] = 7;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.append(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 4);

        // Add
        vv[0] = 8; vv[1] = 9;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.add(y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 8);

        vv[0] = 10; vv[1] = 11;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.add(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 10);

        // Push
        vv[0] = 12; vv[1] = 13;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.push(y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 12);

        vv[0] = 14; vv[1] = 15;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.push(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 14);

        // Unshift
        vv[0] = 16; vv[1] = 17; vv[2] = 18; vv[3] = 19;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.unshift(y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        vv[0] = 20; vv[1] = 21; vv[2] = 22; vv[3] = 23;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.unshift(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        // Prepend
        vv[0] = 24; vv[1] = 25; vv[2] = 26; vv[3] = 27;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.prepend(y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        vv[0] = 28; vv[1] = 29; vv[2] = 30; vv[3] = 31;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.prepend(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 0);

        // Insert
        vv[0] = 32; vv[1] = 33; vv[2] = 34; vv[3] = 35;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.insert(4, y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 4);

        vv[0] = 36; vv[1] = 37; vv[2] = 38; vv[3] = 39;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.insert(24, &y));
        dump(x);
        UTEST_ASSERT(x.index_of(pv) == 24);

        // Check items
        static const int numbers[] = {
            28, 29, 30, 31, 32, 33, 34, 35,
            24, 25, 26, 27, 20, 21, 22, 23,
            16, 17, 18, 19, 0, 1, 2, 3,
            36, 37, 38, 39, 4, 5, 6, 7,
            8, 9, 10, 11, 12, 13, 14, 15
        };
        printf("Check 1...\n");
        check_darray(x, numbers, sizeof(numbers)/sizeof(int));

        y.flush();
        dump(x);

        // Perform pop
        UTEST_ASSERT(pv = x.pop(y));
        UTEST_ASSERT(y.index_of(pv) == 0);
        dump(x);

        UTEST_ASSERT(pv = x.pop(&y));
        UTEST_ASSERT(y.index_of(pv) == 1);
        dump(x);

        // Perform shift
        UTEST_ASSERT(pv = x.shift(y));
        UTEST_ASSERT(y.index_of(pv) == 2);
        dump(x);

        UTEST_ASSERT(pv = x.shift(&y));
        UTEST_ASSERT(y.index_of(pv) == 3);
        dump(x);

        // Perform removal
        UTEST_ASSERT(pv = x.remove(12, y));
        UTEST_ASSERT(y.index_of(pv) == 4);
        dump(x);

        UTEST_ASSERT(pv = x.remove(20, &y));
        UTEST_ASSERT(y.index_of(pv) == 5);
        dump(x);

        // Perform pointer removal
        UTEST_ASSERT(pv = x.premove(x.get(16), y));
        UTEST_ASSERT(y.index_of(pv) == 6);
        dump(x);

        UTEST_ASSERT(pv = x.premove(x.get(28), &y));
        UTEST_ASSERT(y.index_of(pv) == 7);
        dump(x);

        // Perform multiple pop
        UTEST_ASSERT(pv = x.pop_n(2, y));
        UTEST_ASSERT(y.index_of(pv) == 8);
        dump(x);

        UTEST_ASSERT(pv = x.pop_n(2, &y));
        UTEST_ASSERT(y.index_of(pv) == 10);
        dump(x);

        // Perform multiple shift
        UTEST_ASSERT(pv = x.shift_n(2, y));
        UTEST_ASSERT(y.index_of(pv) == 12);
        dump(x);

        UTEST_ASSERT(pv = x.shift_n(2, &y));
        UTEST_ASSERT(y.index_of(pv) == 14);
        dump(x);

        // Perform multiple remove
        UTEST_ASSERT(pv = x.remove_n(10, 2, y));
        UTEST_ASSERT(y.index_of(pv) == 16);
        dump(x);

        UTEST_ASSERT(pv = x.remove_n(12, 2, &y));
        UTEST_ASSERT(y.index_of(pv) == 18);
        dump(x);

        // Perform multiple remove
        UTEST_ASSERT(pv = x.premove_n(x.get(1), 2, y));
        UTEST_ASSERT(y.index_of(pv) == 20);
        dump(x);

        UTEST_ASSERT(pv = x.premove_n(x.get(8), 2, &y));
        UTEST_ASSERT(y.index_of(pv) == 22);
        dump(x);

        // Perform multiple pointer-based remove

        // Check items
        static const int numbers2[] = {
            15, 14, 28, 29,
            22, 3, 19, 9,
            12, 13, 10, 11,
            30, 31, 32, 33,
            17, 18, 2, 36,
            35, 24, 0, 1
        };
        printf("Check 2...\n");
        check_darray(y, numbers2, sizeof(numbers2)/sizeof(int));
    }

    void test_xswap()
    {
        lltl::darray<int> x;
        printf("Testing xswap...\n");

        // Initialize
        for (int i=0; i<32; ++i)
        {
            UTEST_ASSERT(x.add(i));
        }
        dump(x);

        // Reverse order
        for (int i=0, j=x.size()-1; i<j; ++i, --j)
        {
            UTEST_ASSERT(x.xswap(i, j));
        }
        dump(x);

        for (int i=0; i<32; ++i)
        {
            int *pv = x.get(i);
            UTEST_ASSERT(*pv == (31-i));
        }
    }

    void test_long_xswap()
    {
        printf("Testing long xswap...\n");

        large_struct_t *a = new large_struct_t;
        UTEST_ASSERT(a != NULL);
        lsp_finally{ delete a; };

        large_struct_t *b = new large_struct_t;
        UTEST_ASSERT(b != NULL);
        lsp_finally{ delete b; };

        lltl::darray<large_struct_t> x;

        // Initialize
        for (size_t i=0; i<sizeof(large_struct_t::data)/sizeof(int); ++i)
        {
            a->data[i] = 0x55aa0000 | i;
            b->data[i] = (i << 16) | 0xcc33;
        }

        UTEST_ASSERT(x.add(a));
        UTEST_ASSERT(x.add(b));

        UTEST_ASSERT(!::memcmp(x.get(0), a, sizeof(large_struct_t)));
        UTEST_ASSERT(!::memcmp(x.get(1), b, sizeof(large_struct_t)));

        // Swap elements
        UTEST_ASSERT(x.xswap(0, 1));

        UTEST_ASSERT(!::memcmp(x.get(0), b, sizeof(large_struct_t)));
        UTEST_ASSERT(!::memcmp(x.get(1), a, sizeof(large_struct_t)));
    }

    void test_sort()
    {
        static const ssize_t N = 4;

        printf("Testing qsort...\n");

        int v[N];
        for (ssize_t i=0; i<N; ++i)
            v[i]    = N - i;

        lltl::darray<int> a;
        for (ssize_t i=0; i<N; ++i)
            UTEST_ASSERT(a.add(&v[i]));
        for (size_t i=0; i<N; ++i)
            UTEST_ASSERT(*(a.get(i)) == v[i]);

        a.qsort(test_int_cmp);
        for (ssize_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(*(a.get(i)) == v[N-i-1]);
            printf("%d ", *a.get(i));
        }
        printf("\n");

        a.qsort(test_int_cmp2);
        for (ssize_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(*(a.get(i)) == v[i]);
            printf("%d ", *a.get(i));
        }
        printf("\n");
    }

    UTEST_MAIN
    {
        test_single();
        test_single_with_copy();
        test_multiple();
        test_multiple_with_copy();
        test_multiple_darray();
        test_xswap();
        test_long_xswap();
        test_sort();
    }

UTEST_END



