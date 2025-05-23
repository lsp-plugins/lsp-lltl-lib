/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 8 апр. 2020 г.
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

#include <lsp-plug.in/lltl/parray.h>
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

UTEST_BEGIN("lltl", parray)

    void dump(lltl::parray<int> &x)
    {
        for (size_t i=0, n=x.size(); i<n; ++i)
            printf("%d ", *x.get(i));
        printf("\n");
    }


    void check_parray(lltl::parray<int> &x, const int *numbers, int n)
    {
        int **pv;

        UTEST_ASSERT(x.size() == size_t(n));
        UTEST_ASSERT(x.capacity() >= size_t(n));
        for (int i=0; i<n; ++i)
        {
            UTEST_ASSERT_MSG(pv = x.pget(i), "Failed pget at index %d", i);
            UTEST_ASSERT_MSG(*pv != NULL, "NULL element at index %d", i);
            UTEST_ASSERT_MSG(**pv == numbers[i], "Failed at index %d: got %d, expected %d", i, **pv, numbers[i]);
        }
        UTEST_ASSERT(x.get(n) == NULL);
    }

    void test_single()
    {
        printf("Testing single operations on parray...\n");
        int array[0x20];
        for (size_t i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = int(i);
        int *s = array;

        lltl::parray<int> x;
        int *v, **pv;

        // Check initial state
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);

        // Add items
        UTEST_ASSERT(pv = x.append());
        *pv = s++;
        dump(x);
        UTEST_ASSERT(pv = x.add());
        *pv = s++;
        dump(x);
        UTEST_ASSERT(!(pv = x.insert(10)));
        UTEST_ASSERT(pv = x.insert(1));
        *pv = s++;
        dump(x);
        UTEST_ASSERT(pv = x.insert(0));
        *pv = s++;
        dump(x);
        UTEST_ASSERT(pv = x.push());
        *pv = s++;
        dump(x);
        UTEST_ASSERT(pv = x.unshift());
        *pv = s++;
        dump(x);
        UTEST_ASSERT(pv = x.prepend());
        *pv = s++;
        dump(x);
        UTEST_ASSERT(pv = x.unshift());
        *pv = s++;
        dump(x);
        UTEST_ASSERT(pv = x.set(1, &array[10]));
        UTEST_ASSERT(**pv == 10);
        dump(x);

        // Check items
        static const int numbers[]  = { 7, 10, 5, 3, 0, 2, 1, 4 };
        check_parray(x, numbers, sizeof(numbers)/sizeof(int));

        // Remove items
        UTEST_ASSERT(v = x.qremove(0));
        dump(x);
        UTEST_ASSERT(*v == 7);
        UTEST_ASSERT(v = x.get(0));
        UTEST_ASSERT(*v == 4);

        UTEST_ASSERT(v = x.pop());
        dump(x);
        UTEST_ASSERT(*v == 1);

        UTEST_ASSERT(v = x.last());
        dump(x);
        UTEST_ASSERT(*v == 2);

        UTEST_ASSERT(x.shift());
        dump(x);
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 10);

        UTEST_ASSERT(!x.remove(5));
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 3);

        UTEST_ASSERT(x.remove(2));
        dump(x);
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 0);

        UTEST_ASSERT(!x.premove(NULL));
        UTEST_ASSERT(x.get(x.size()) == NULL);
        UTEST_ASSERT(x.premove(v));
        dump(x);
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 2);

        // Check size
        UTEST_ASSERT(x.size() == 3);
        UTEST_ASSERT(x.capacity() >= 3);
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 10);
        UTEST_ASSERT(v = x.last());
        UTEST_ASSERT(*v == 2);
    }

    void test_single_with_copy()
    {
        printf("Testing single operations on parray with copying...\n");
        int array[0x20];
        for (size_t i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = int(i);
        int *s = array;

        lltl::parray<int> x;
        int *v, **pv;

        // Check initial state
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);

        // Add items
        UTEST_ASSERT(x.add(s++));
        dump(x);
        UTEST_ASSERT(x.add(s++));
        dump(x);
        UTEST_ASSERT(x.add(s++));
        dump(x);
        UTEST_ASSERT(x.add(s++));
        dump(x);
        UTEST_ASSERT(x.size() == 4);
        UTEST_ASSERT(x.capacity() >= 4);

        // Insert items
        UTEST_ASSERT(!x.insert(100, s));
        UTEST_ASSERT(x.size() == 4);
        UTEST_ASSERT(x.capacity() >= 4);
        UTEST_ASSERT(x.insert(4, s++));
        dump(x);
        UTEST_ASSERT(x.insert(5, s++));
        dump(x);
        UTEST_ASSERT(x.insert(0, s++));
        dump(x);
        UTEST_ASSERT(x.insert(1, s++));
        dump(x);
        UTEST_ASSERT(x.size() == 8);
        UTEST_ASSERT(x.capacity() >= 8);

        // Append items
        UTEST_ASSERT(x.append(s++));
        UTEST_ASSERT(x.append(s++));
        UTEST_ASSERT(x.size() == 10);
        UTEST_ASSERT(x.capacity() >= 10);

        // Push items
        UTEST_ASSERT(x.push(s++));
        dump(x);
        UTEST_ASSERT(x.push(s++));
        dump(x);
        UTEST_ASSERT(x.size() == 12);
        UTEST_ASSERT(x.capacity() >= 12);

        // Unshift items
        UTEST_ASSERT(x.unshift(s++));
        dump(x);
        UTEST_ASSERT(x.unshift(s++));
        dump(x);
        UTEST_ASSERT(x.size() == 14);
        UTEST_ASSERT(x.capacity() >= 14);

        // prepend items
        UTEST_ASSERT(x.prepend(s++));
        dump(x);
        UTEST_ASSERT(x.prepend(s++));
        dump(x);
        UTEST_ASSERT(x.size() == 16);
        UTEST_ASSERT(x.capacity() >= 16);

        // Check items
        static const int numbers[] = { 15, 14, 13, 12, 6, 7, 0, 1, 2, 3, 4, 5, 8, 9, 10, 11 };
        check_parray(x, numbers, sizeof(numbers)/sizeof(int));

        // Pop element
        v = NULL;
        UTEST_ASSERT(pv = x.pop(&v));
        dump(x);
        UTEST_ASSERT(*v == 11);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.pop(&v));
        dump(x);
        UTEST_ASSERT(*v == 10);
        UTEST_ASSERT(pv == &v);

        // Shift element
        pv = NULL;
        UTEST_ASSERT(pv = x.shift(&v));
        dump(x);
        UTEST_ASSERT(*v == 15);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.shift(&v));
        dump(x);
        UTEST_ASSERT(*v == 14);
        UTEST_ASSERT(pv == &v);

        // Remove element
        pv = NULL;
        UTEST_ASSERT(pv = x.remove(6, &v));
        dump(x);
        UTEST_ASSERT(*v == 2);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.remove(1, &v));
        dump(x);
        UTEST_ASSERT(*v == 12);
        UTEST_ASSERT(pv == &v);

        // Remove pointer
        pv = NULL;
        UTEST_ASSERT(pv = x.premove(x.get(5), &v));
        dump(x);
        UTEST_ASSERT(*v == 3);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.premove(x.get(2), &v));
        dump(x);
        UTEST_ASSERT(*v == 7);
        UTEST_ASSERT(pv == &v);

        // Check size
        UTEST_ASSERT(x.size() == 8);
        UTEST_ASSERT(x.capacity() >= 8);
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 13);
        UTEST_ASSERT(v = x.last());
        UTEST_ASSERT(*v == 9);

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
        int array[0x20];
        for (size_t i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = int(i);
        int *s = array;

        lltl::parray<int> x;
        int *v, **pv;

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
        pv[0] = s++; pv[1] = s++;
        dump(x);

        // Add items
        UTEST_ASSERT(pv = x.add_n(2));
        pv[0] = s++; pv[1] = s++;
        dump(x);

        // Push items
        UTEST_ASSERT(pv = x.push_n(3));
        pv[0] = s++; pv[1] = s++; pv[2] = s++;
        dump(x);

        // Unshift items
        UTEST_ASSERT(pv = x.unshift_n(3));
        pv[0] = s++; pv[1] = s++; pv[2] = s++;
        dump(x);

        // Prepend items
        UTEST_ASSERT(pv = x.prepend_n(2));
        pv[0] = s++; pv[1] = s++;
        dump(x);

        // Insert items
        UTEST_ASSERT(pv = x.insert_n(4, 4));
        pv[0] = s++; pv[1] = s++; pv[2] = s++; pv[3] = s++;
        dump(x);

        // Check items
        static const int numbers[] = { 10, 11, 7, 8, 12, 13, 14, 15, 9, 0, 1, 2, 3, 4, 5, 6 };
        check_parray(x, numbers, sizeof(numbers)/sizeof(int));

        // Pop items
        UTEST_ASSERT(!x.pop_n(17));
        UTEST_ASSERT(pv = x.pop_n(4));
        dump(x);
        UTEST_ASSERT(*(pv[0]) == 3);
        UTEST_ASSERT(*(pv[1]) == 4);
        UTEST_ASSERT(*(pv[2]) == 5);
        UTEST_ASSERT(*(pv[3]) == 6);

        // Shift items
        UTEST_ASSERT(!x.shift_n(13));
        UTEST_ASSERT(x.shift_n(4));
        dump(x);
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 12);

        // Remove items
        UTEST_ASSERT(!x.remove_n(8, 1));
        UTEST_ASSERT(!x.remove_n(0, 9));
        UTEST_ASSERT(x.remove_n(2, 2));
        dump(x);
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 9);

        // Remove pointer
        UTEST_ASSERT(!x.premove_n(x.get(1), 6));
        UTEST_ASSERT(x.premove_n(x.get(1), 3));
        dump(x);
        UTEST_ASSERT(v = x.get(1));
        UTEST_ASSERT(*v == 1);

        UTEST_ASSERT(x.size() == 3);
    }

    void test_multiple_with_copy()
    {
        printf("Testing multiple operations on darray with copying...\n");
        int array[0x20];
        for (size_t i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = int(i);
        int *s = array;

        lltl::parray<int> x;
        int **pv, *vv[5];

        // Check initial state
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.get(0) == NULL);
        UTEST_ASSERT(x.first() == NULL);
        UTEST_ASSERT(x.last() == NULL);
        UTEST_ASSERT(x.array() == NULL);
        UTEST_ASSERT(x.slice(0, 0) == NULL);

        // Marker
        vv[4] = NULL;

        // Set values
        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(pv = x.set_n(4, vv));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);
        UTEST_ASSERT(x.contains(*pv));

        // Append values
        vv[0] = s++; vv[1] = s++;
        UTEST_ASSERT(pv = x.append_n(2,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 4);
        UTEST_ASSERT(x.contains(*pv));

        // Add values
        vv[0] = s++; vv[1] = s++; vv[2] = s++;
        UTEST_ASSERT(pv = x.append_n(3,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 6);
        UTEST_ASSERT(x.contains(*pv));

        // Push values
        vv[0] = s++; vv[1] = s++; vv[2] = s++;
        UTEST_ASSERT(pv = x.push_n(3,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 9);
        UTEST_ASSERT(x.contains(*pv));

        // Unshift values
        vv[0] = s++; vv[1] = s++;
        UTEST_ASSERT(pv = x.unshift_n(2,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);
        UTEST_ASSERT(x.contains(*pv));

        // Prepend values
        vv[0] = s++; vv[1] = s++;
        UTEST_ASSERT(pv = x.prepend_n(2,  vv));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);
        UTEST_ASSERT(x.contains(*pv));

        // Insert values
        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(pv = x.insert_n(12,  4, vv));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 12);
        UTEST_ASSERT(x.contains(*pv));

        // Check items
        static const int numbers[] = { 14, 15, 12, 13, 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 8, 9, 10, 11 };
        check_parray(x, numbers, sizeof(numbers)/sizeof(int));

        // Pop items
        UTEST_ASSERT(pv = x.pop_n(4,  vv));
        dump(x);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(*(vv[0]) == 8);
        UTEST_ASSERT(*(vv[1]) == 9);
        UTEST_ASSERT(*(vv[2]) == 10);
        UTEST_ASSERT(*(vv[3]) == 11);
        UTEST_ASSERT(vv[4] == NULL);

        // Get items
        UTEST_ASSERT(pv = x.get_n(10, 2, vv));
        dump(x);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(*(vv[0]) == 6);
        UTEST_ASSERT(*(vv[1]) == 7);
        UTEST_ASSERT(*(vv[2]) == 10);

        // Shift items
        UTEST_ASSERT(pv = x.shift_n(3,  vv));
        dump(x);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(*(vv[0]) == 14);
        UTEST_ASSERT(*(vv[1]) == 15);
        UTEST_ASSERT(*(vv[2]) == 12);
        UTEST_ASSERT(*(vv[3]) == 11);

        // Remove items
        UTEST_ASSERT(pv = x.remove_n(8, 2, vv));
        dump(x);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(*(vv[0]) == 7);
        UTEST_ASSERT(*(vv[1]) == 16);
        UTEST_ASSERT(*(vv[2]) == 12);

        // Remove pointer
        UTEST_ASSERT(pv = x.premove_n(x.get(2), 2, vv));
        dump(x);
        UTEST_ASSERT(pv == vv);
        UTEST_ASSERT(*(vv[0]) == 1);
        UTEST_ASSERT(*(vv[1]) == 2);
        UTEST_ASSERT(*(vv[2]) == 12);

        // Check final size
        UTEST_ASSERT(x.size() == 9);
    }

    void test_multiple_parray()
    {
        printf("Testing multiple operations on parray with another parray...\n");
        int array[0x40];
        for (int i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = i;
        int *s = array;

        lltl::parray<int> x, y;
        int **pv, *vv[4];

        // Append
        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.append(y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);

        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.append(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 4);

        // Add
        vv[0] = s++; vv[1] = s++;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.add(y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 8);

        vv[0] = s++; vv[1] = s++;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.add(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 10);

        // Push
        vv[0] = s++; vv[1] = s++;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.push(y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 12);

        vv[0] = s++; vv[1] = s++;
        UTEST_ASSERT(y.set_n(2, vv));
        UTEST_ASSERT(pv = x.push(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 14);

        // Unshift
        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.unshift(y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);

        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.unshift(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);

        // Prepend
        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.prepend(y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);

        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.prepend(&y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 0);

        // Insert
        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.insert(4, y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 4);

        vv[0] = s++; vv[1] = s++; vv[2] = s++; vv[3] = s++;
        UTEST_ASSERT(y.set_n(4, vv));
        UTEST_ASSERT(pv = x.insert(24, &y));
        dump(x);
        UTEST_ASSERT(x.index_of(*pv) == 24);

        // Check items
        static const int numbers[] = {
            28, 29, 30, 31, 32, 33, 34, 35,
            24, 25, 26, 27, 20, 21, 22, 23,
            16, 17, 18, 19, 0, 1, 2, 3,
            36, 37, 38, 39, 4, 5, 6, 7,
            8, 9, 10, 11, 12, 13, 14, 15
        };
        printf("Check 1...\n");
        check_parray(x, numbers, sizeof(numbers)/sizeof(int));

        y.flush();
        dump(x);

        // Perform pop
        UTEST_ASSERT(pv = x.pop(y));
        UTEST_ASSERT(y.index_of(*pv) == 0);
        dump(x);

        UTEST_ASSERT(pv = x.pop(&y));
        UTEST_ASSERT(y.index_of(*pv) == 1);
        dump(x);

        // Perform shift
        UTEST_ASSERT(pv = x.shift(y));
        UTEST_ASSERT(y.index_of(*pv) == 2);
        dump(x);

        UTEST_ASSERT(pv = x.shift(&y));
        UTEST_ASSERT(y.index_of(*pv) == 3);
        dump(x);

        // Perform removal
        UTEST_ASSERT(pv = x.remove(12, y));
        UTEST_ASSERT(y.index_of(*pv) == 4);
        dump(x);

        UTEST_ASSERT(pv = x.remove(20, &y));
        UTEST_ASSERT(y.index_of(*pv) == 5);
        dump(x);

        // Perform pointer removal
        UTEST_ASSERT(pv = x.premove(x.get(16), y));
        UTEST_ASSERT(y.index_of(*pv) == 6);
        dump(x);

        UTEST_ASSERT(pv = x.premove(x.get(28), &y));
        UTEST_ASSERT(y.index_of(*pv) == 7);
        dump(x);

        // Perform multiple pop
        UTEST_ASSERT(pv = x.pop_n(2, y));
        UTEST_ASSERT(y.index_of(*pv) == 8);
        dump(x);

        UTEST_ASSERT(pv = x.pop_n(2, &y));
        UTEST_ASSERT(y.index_of(*pv) == 10);
        dump(x);

        // Perform multiple shift
        UTEST_ASSERT(pv = x.shift_n(2, y));
        UTEST_ASSERT(y.index_of(*pv) == 12);
        dump(x);

        UTEST_ASSERT(pv = x.shift_n(2, &y));
        UTEST_ASSERT(y.index_of(*pv) == 14);
        dump(x);

        // Perform multiple remove
        UTEST_ASSERT(pv = x.remove_n(10, 2, y));
        UTEST_ASSERT(y.index_of(*pv) == 16);
        dump(x);

        UTEST_ASSERT(pv = x.remove_n(12, 2, &y));
        UTEST_ASSERT(y.index_of(*pv) == 18);
        dump(x);

        // Perform multiple remove
        UTEST_ASSERT(pv = x.premove_n(x.get(1), 2, y));
        UTEST_ASSERT(y.index_of(*pv) == 20);
        dump(x);

        UTEST_ASSERT(pv = x.premove_n(x.get(8), 2, &y));
        UTEST_ASSERT(y.index_of(*pv) == 22);
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
        check_parray(y, numbers2, sizeof(numbers2)/sizeof(int));
    }

    void test_xswap()
    {
        printf("Testing xswap...\n");

        int array[0x40];
        for (int i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = i;
        int *s = array;

        lltl::parray<int> x;

        // Initialize
        for (int i=0; i<0x40; ++i)
        {
            UTEST_ASSERT(x.add(s++));
        }
        dump(x);

        // Reverse order
        for (ssize_t i=0, j=x.size()-1; i<j; ++i, --j)
        {
            UTEST_ASSERT(x.xswap(i, j));
        }
        dump(x);

        for (int i=0; i<0x40; ++i)
        {
            int *pv = x.get(i);
            UTEST_ASSERT(*pv == (0x3f-i));
        }
    }

    void test_sort()
    {
        static const ssize_t N = 4;

        printf("Testing qsort...\n");

        int v[N];
        for (ssize_t i=0; i<N; ++i)
            v[i]    = int(N - i);

        lltl::parray<int> a;
        for (ssize_t i=0; i<N; ++i)
            UTEST_ASSERT(a.add(&v[i]));
        for (size_t i=0; i<N; ++i)
            UTEST_ASSERT(a.get(i) == &v[i]);

        a.qsort(test_int_cmp);
        for (ssize_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(a.get(i) == &v[N-i-1]);
            printf("%d ", *a.get(i));
        }
        printf("\n");

        a.qsort(test_int_cmp2);
        for (ssize_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(a.get(i) == &v[i]);
            printf("%d ", *a.get(i));
        }
        printf("\n");
    }

    void test_iterator()
    {
        static const ssize_t N = 8;

        printf("Testing iterator...\n");
        int v[N];
        for (ssize_t i=0; i<N; ++i)
            v[i]    = int(i);

        lltl::parray<int> a;
        for (ssize_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(a.add(&v[i]));
        }
        for (size_t i=0; i<N; ++i)
            UTEST_ASSERT(a.get(i) == &v[i]);

        lltl::iterator<int> start = a.values();
        ssize_t x, n;

        x = 0;
        n = 0;
        for (lltl::iterator<int> it = a.values(); it; ++it, ++x, ++n)
        {
            // Check for validation
            UTEST_ASSERT(it);
            UTEST_ASSERT(it.valid());
            UTEST_ASSERT(!(!it));
            UTEST_ASSERT(!(it.invalid()));

            // Check for siblings
            UTEST_ASSERT(it & start);
            UTEST_ASSERT(it.sibling_of(start));
            UTEST_ASSERT(!(it | start));
            UTEST_ASSERT(!it.not_sibling_of(start));

            // Calc position, size and remaining
            UTEST_ASSERT(it.forward());
            UTEST_ASSERT(!it.reversive());
            UTEST_ASSERT(it.index() == size_t(x));
            UTEST_ASSERT(it.remaining() == size_t(N - x));
            UTEST_ASSERT(it.max_advance() == size_t(N - x - 1));
            UTEST_ASSERT(it.count() == N);

            // Check distance computation
            UTEST_ASSERT((it - start) == x);
            UTEST_ASSERT((start - it) == -x);

            // Check comparisons
            UTEST_ASSERT(it != lltl::iterator<int>::INVALID);
            UTEST_ASSERT(!(it == lltl::iterator<int>::INVALID));
            UTEST_ASSERT(it >= start);
            UTEST_ASSERT(start <= it);
            if (x > 0)
            {
                UTEST_ASSERT(it != start);
                UTEST_ASSERT(start != it);
                UTEST_ASSERT(it > start);
                UTEST_ASSERT(start < it);
            }
            else
            {
                UTEST_ASSERT(it == start);
                UTEST_ASSERT(start == it);
            }

            // Check value read
            UTEST_ASSERT(*it == &v[x]);
        }
        UTEST_ASSERT(n == N);

        x = N - 1;
        n = 0;
        for (lltl::iterator<int> it = a.rvalues(); it; ++it, --x, ++n)
        {
            // Check for validation
            UTEST_ASSERT(it);
            UTEST_ASSERT(it.valid());
            UTEST_ASSERT(!(!it));
            UTEST_ASSERT(!(it.invalid()));

            // Check for siblings
            UTEST_ASSERT(it & start);
            UTEST_ASSERT(it.sibling_of(start));
            UTEST_ASSERT(!(it | start));
            UTEST_ASSERT(!it.not_sibling_of(start));

            // Calc position, size and remaining
            UTEST_ASSERT(!it.forward());
            UTEST_ASSERT(it.reversive());
            UTEST_ASSERT(it.index() == size_t(x));
            UTEST_ASSERT(it.remaining() == size_t(x + 1));
            UTEST_ASSERT(it.max_advance() == size_t(x));
            UTEST_ASSERT(it.count() == N);

            // Check distance computation
            UTEST_ASSERT((it - start) == x);
            UTEST_ASSERT((start - it) == -x);

            // Check comparisons
            UTEST_ASSERT(it != lltl::iterator<int>::INVALID);
            UTEST_ASSERT(!(it == lltl::iterator<int>::INVALID));
            UTEST_ASSERT(it <= start);
            UTEST_ASSERT(start <= it);
            if (x > 0)
            {
                UTEST_ASSERT(it != start);
                UTEST_ASSERT(start != it);
                UTEST_ASSERT(it < start);
                UTEST_ASSERT(start < it);
            }
            else
            {
                UTEST_ASSERT(it == start);
                UTEST_ASSERT(start == it);
            }

            // Check value read
            UTEST_ASSERT(*it == &v[x]);
        }
        UTEST_ASSERT(n == N);
    }

    UTEST_MAIN
    {
        test_single();
        test_single_with_copy();
        test_multiple();
        test_multiple_with_copy();
        test_multiple_parray();
        test_xswap();
        test_sort();
        test_iterator();
    }

UTEST_END


