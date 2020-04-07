/*
 * cstorage.cpp
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/lltl/cstorage.h>

UTEST_BEGIN("lltl", cstorage)

    void test_single()
    {
        static const int numbers[] = { 7, 6, 4, 1, 3, 2, 5 };
        printf("Testing single operations on cstorage...\n");

        lltl::cstorage<int> x;
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
        UTEST_ASSERT(pv = x.append());
        *pv = 2;
        UTEST_ASSERT(!(pv = x.insert(10)));
        UTEST_ASSERT(pv = x.insert(1));
        *pv = 3;
        UTEST_ASSERT(pv = x.insert(0));
        *pv = 4;
        UTEST_ASSERT(pv = x.push());
        *pv = 5;
        UTEST_ASSERT(pv = x.unshift());
        *pv = 6;
        UTEST_ASSERT(pv = x.prepend());
        *pv = 7;

        // Check items
        int n = sizeof(numbers)/sizeof(int);
        UTEST_ASSERT(x.size() == size_t(n));
        UTEST_ASSERT(x.capacity() >= size_t(n));
        for (int i=0; i<n; ++i)
        {
            UTEST_ASSERT_MSG(pv = x.get(i), "Failed at index %d", i);
            UTEST_ASSERT_MSG(*pv == numbers[i], "Failed at index %d: got %d, expected %d", i, *pv, numbers[i]);
        }
        UTEST_ASSERT(x.get(n) == NULL);

        // Remove items
        UTEST_ASSERT(pv = x.pop());
        UTEST_ASSERT(*pv == 5);
        UTEST_ASSERT(pv = x.last());
        UTEST_ASSERT(*pv == 2);
        UTEST_ASSERT(x.shift());
        UTEST_ASSERT(pv = x.first());
        UTEST_ASSERT(*pv == 6);
        UTEST_ASSERT(!x.remove(5));
        UTEST_ASSERT(pv = x.get(2));
        UTEST_ASSERT(*pv == 1);
        UTEST_ASSERT(x.remove(2));
        UTEST_ASSERT(pv = x.get(2));
        UTEST_ASSERT(*pv == 3);
        UTEST_ASSERT(!x.premove(NULL));
        UTEST_ASSERT(x.get(x.size()) == NULL);
        UTEST_ASSERT(!x.premove(x.at(x.size())));
        UTEST_ASSERT(x.premove(pv));
        UTEST_ASSERT(pv = x.get(2));
        UTEST_ASSERT(*pv == 2);

        // Check size
        UTEST_ASSERT(x.size() == 3);
        UTEST_ASSERT(x.capacity() >= 3);
        UTEST_ASSERT(pv = x.first());
        UTEST_ASSERT(*pv == 6);
        UTEST_ASSERT(pv = x.last());
        UTEST_ASSERT(*pv == 2);
    }

    void test_single_with_copy()
    {
        static const int numbers[] = { 16, 15, 14, 13, 6, 7, 0, 1, 2, 3, 4, 5, 8, 9, 11, 12 };

        printf("Testing single operations with data copying on cstorage...\n");

        lltl::cstorage<int> x;
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
        UTEST_ASSERT(x.add(1));
        UTEST_ASSERT(x.add(2));
        UTEST_ASSERT(x.add(3));
        UTEST_ASSERT(x.size() == 4);
        UTEST_ASSERT(x.capacity() >= 4);

        // Insert items
        UTEST_ASSERT(!x.insert(100, 100));
        UTEST_ASSERT(!x.insert(100, &v));
        UTEST_ASSERT(x.size() == 4);
        UTEST_ASSERT(x.capacity() >= 4);
        v = 4;
        UTEST_ASSERT(x.insert(4, &v));
        UTEST_ASSERT(x.insert(5, 5));
        v = 6;
        UTEST_ASSERT(x.insert(0, &v));
        UTEST_ASSERT(x.insert(1, 7));
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
        UTEST_ASSERT(x.push(12));
        UTEST_ASSERT(x.size() == 12);
        UTEST_ASSERT(x.capacity() >= 12);

        // Unshift items
        v = 13;
        UTEST_ASSERT(x.unshift(&v));
        UTEST_ASSERT(x.unshift(14));
        UTEST_ASSERT(x.size() == 14);
        UTEST_ASSERT(x.capacity() >= 14);

        // prepend items
        v = 15;
        UTEST_ASSERT(x.prepend(&v));
        UTEST_ASSERT(x.prepend(16));
        UTEST_ASSERT(x.size() == 16);
        UTEST_ASSERT(x.capacity() >= 16);

        // Check items
        int n = sizeof(numbers)/sizeof(int);
        UTEST_ASSERT(x.size() == size_t(n));
        UTEST_ASSERT(x.capacity() >= size_t(n));
        for (int i=0; i<n; ++i)
        {
            UTEST_ASSERT_MSG(pv = x.get(i), "Failed at index %d", i);
            UTEST_ASSERT_MSG(*pv == numbers[i], "Failed at index %d: got %d, expected %d", i, *pv, numbers[i]);
        }
        UTEST_ASSERT(x.get(n) == NULL);

        // Pop element
        pv = NULL;
        UTEST_ASSERT(pv = x.pop(&v));
        UTEST_ASSERT(v == 12);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.pop(v));
        UTEST_ASSERT(v == 11);
        UTEST_ASSERT(pv == &v);

        // Shift element
        pv = NULL;
        UTEST_ASSERT(pv = x.shift(&v));
        UTEST_ASSERT(v == 16);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.shift(v));
        UTEST_ASSERT(v == 15);
        UTEST_ASSERT(pv == &v);

        // Remove element
        pv = NULL;
        UTEST_ASSERT(pv = x.remove(6, &v));
        UTEST_ASSERT(v == 2);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.remove(1, v));
        UTEST_ASSERT(v == 13);
        UTEST_ASSERT(pv == &v);

        // Remove pointer
        pv = NULL;
        UTEST_ASSERT(pv = x.premove(x.get(5), &v));
        UTEST_ASSERT(v == 3);
        UTEST_ASSERT(pv == &v);

        pv = NULL;
        UTEST_ASSERT(pv = x.premove(x.get(2), v));
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

    UTEST_MAIN
    {
        test_single();
        test_single_with_copy();
    }

UTEST_END



