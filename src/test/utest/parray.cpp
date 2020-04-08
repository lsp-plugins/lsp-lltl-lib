/*
 * parray.cpp
 *
 *  Created on: 8 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("lltl", parray)

    void dump(lltl::parray<int> &x)
    {
        for (int i=0, n=x.size(); i<n; ++i)
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
        int array[0x20];
        for (int i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = i;
        int *s = array;

        printf("Testing single operations on parray...\n");

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

        // Check items
        static const int numbers[]  = { 6, 5, 3, 0, 2, 1, 4 };
        check_parray(x, numbers, sizeof(numbers)/sizeof(int));

        // Remove items
        UTEST_ASSERT(v = x.pop());
        dump(x);
        UTEST_ASSERT(*v == 4);

        UTEST_ASSERT(v = x.last());
        dump(x);
        UTEST_ASSERT(*v == 1);

        UTEST_ASSERT(x.shift());
        dump(x);
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 5);

        UTEST_ASSERT(!x.remove(5));
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 0);

        UTEST_ASSERT(x.remove(2));
        dump(x);
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 2);

        UTEST_ASSERT(!x.premove(NULL));
        UTEST_ASSERT(x.get(x.size()) == NULL);
        UTEST_ASSERT(x.premove(v));
        dump(x);
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 1);

        // Check size
        UTEST_ASSERT(x.size() == 3);
        UTEST_ASSERT(x.capacity() >= 3);
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 5);
        UTEST_ASSERT(v = x.last());
        UTEST_ASSERT(*v == 1);
    }

    void test_single_with_copy()
    {
        int array[0x20];
        for (int i=0, n=sizeof(array)/sizeof(int); i<n; ++i)
            array[i] = i;
        int *s = array;

        printf("Testing single operations on parray with copying...\n");

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


    UTEST_MAIN
    {
        test_single();
        test_single_with_copy();
//        test_multiple();
//        test_multiple_with_copy();
//        test_multiple_darray();
//        test_xswap();
//        test_long_xswap();
    }

UTEST_END


