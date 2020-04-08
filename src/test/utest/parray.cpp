/*
 * parray.cpp
 *
 *  Created on: 8 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("lltl", parray)

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

        printf("Testing single operations on darray...\n");

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
        UTEST_ASSERT(pv = x.add());
        *pv = s++;
        UTEST_ASSERT(!(pv = x.insert(10)));
        UTEST_ASSERT(pv = x.insert(1));
        *pv = s++;
        UTEST_ASSERT(pv = x.insert(0));
        *pv = s++;
        UTEST_ASSERT(pv = x.push());
        *pv = s++;
        UTEST_ASSERT(pv = x.unshift());
        *pv = s++;
        UTEST_ASSERT(pv = x.prepend());
        *pv = s++;

        // Check items
        static const int numbers[]  = { 7, 6, 4, 1, 3, 2, 5 };
        check_parray(x, numbers, sizeof(numbers)/sizeof(int));

        // Remove items
        UTEST_ASSERT(v = x.pop());
        UTEST_ASSERT(*v == 5);
        UTEST_ASSERT(v = x.last());
        UTEST_ASSERT(*v == 2);
        UTEST_ASSERT(x.shift());
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 6);
        UTEST_ASSERT(!x.remove(5));
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 1);
        UTEST_ASSERT(x.remove(2));
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 3);
        UTEST_ASSERT(!x.premove(NULL));
        UTEST_ASSERT(x.get(x.size()) == NULL);
        UTEST_ASSERT(!x.premove(x.uget(x.size())));
        UTEST_ASSERT(x.premove(v));
        UTEST_ASSERT(v = x.get(2));
        UTEST_ASSERT(*v == 2);

        // Check size
        UTEST_ASSERT(x.size() == 3);
        UTEST_ASSERT(x.capacity() >= 3);
        UTEST_ASSERT(v = x.first());
        UTEST_ASSERT(*v == 6);
        UTEST_ASSERT(v = x.last());
        UTEST_ASSERT(*v == 2);
    }


    UTEST_MAIN
    {
        test_single();
//        test_single_with_copy();
//        test_multiple();
//        test_multiple_with_copy();
//        test_multiple_darray();
//        test_xswap();
//        test_long_xswap();
    }

UTEST_END


