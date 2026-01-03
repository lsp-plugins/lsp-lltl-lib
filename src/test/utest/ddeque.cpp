/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 3 янв. 2026 г.
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

#include <lsp-plug.in/lltl/ddeque.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("lltl", ddeque)

    void test_simple_single_operations()
    {
        lltl::ddeque<int> v(8);
        int *ptr;
        int x = 1;
        int value = 0;

        printf("Testing ddeque simple single operations...\n");

        // Test fail pop
        UTEST_ASSERT(v.size() == 0);
        UTEST_ASSERT(v.is_empty());
        UTEST_ASSERT(!v.pop_back(value));
        UTEST_ASSERT(!v.pop_front(value));
        UTEST_ASSERT(v.front() == NULL);
        UTEST_ASSERT(v.back() == NULL);
        UTEST_ASSERT(value == 0);

        // Test single push_back and pop_back
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.size() == 1);
        UTEST_ASSERT(!v.is_empty());
        UTEST_ASSERT(ptr = v.front());
        UTEST_ASSERT(*ptr == 1);
        UTEST_ASSERT(ptr = v.back());
        UTEST_ASSERT(*ptr == 1);
        UTEST_ASSERT(v.pop_back(value));
        UTEST_ASSERT(value == 1);

        // Test fail pop_back and pop_front
        UTEST_ASSERT(v.size() == 0);
        UTEST_ASSERT(v.is_empty());
        UTEST_ASSERT(!v.pop_back(value));
        UTEST_ASSERT(!v.pop_front(value));
        UTEST_ASSERT(v.front() == NULL);
        UTEST_ASSERT(v.back() == NULL);
        UTEST_ASSERT(value == 1);

        // Test single push_back and pop_front
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.size() == 1);
        UTEST_ASSERT(!v.is_empty());
        UTEST_ASSERT(ptr = v.front());
        UTEST_ASSERT(*ptr == 2);
        UTEST_ASSERT(ptr = v.back());
        UTEST_ASSERT(*ptr == 2);
        UTEST_ASSERT(v.pop_front(value));
        UTEST_ASSERT(value == 2);

        // Test fail pop_back and pop_front
        UTEST_ASSERT(v.size() == 0);
        UTEST_ASSERT(v.is_empty());
        UTEST_ASSERT(!v.pop_back(value));
        UTEST_ASSERT(!v.pop_front(value));
        UTEST_ASSERT(v.front() == NULL);
        UTEST_ASSERT(v.back() == NULL);
        UTEST_ASSERT(value == 2);

        // Test 2x push_back + 2x pop_back
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.size() == 2);
        UTEST_ASSERT(ptr = v.front());
        UTEST_ASSERT(*ptr == 3);
        UTEST_ASSERT(ptr = v.back());
        UTEST_ASSERT(*ptr == 4);
        UTEST_ASSERT(v.pop_back(value));
        UTEST_ASSERT(value == 4);
        UTEST_ASSERT(v.pop_back(value));
        UTEST_ASSERT(value == 3);

        // Test 2x push_back + 2x pop_front
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.size() == 2);
        UTEST_ASSERT(ptr = v.front());
        UTEST_ASSERT(*ptr == 5);
        UTEST_ASSERT(ptr = v.back());
        UTEST_ASSERT(*ptr == 6);
        UTEST_ASSERT(v.pop_front(value));
        UTEST_ASSERT(value == 5);
        UTEST_ASSERT(v.pop_front(value));
        UTEST_ASSERT(value == 6);

        // Test 2x push_front + 2x pop_back
        UTEST_ASSERT(v.push_front(x++));
        UTEST_ASSERT(v.push_front(x++));
        UTEST_ASSERT(v.size() == 2);
        UTEST_ASSERT(ptr = v.front());
        UTEST_ASSERT(*ptr == 8);
        UTEST_ASSERT(ptr = v.back());
        UTEST_ASSERT(*ptr == 7);
        UTEST_ASSERT(v.pop_back(value));
        UTEST_ASSERT(value == 7);
        UTEST_ASSERT(v.pop_back(value));
        UTEST_ASSERT(value == 8);

        // Test 2x push_front + 2x pop_front
        UTEST_ASSERT(v.push_front(x++));
        UTEST_ASSERT(v.push_front(x++));
        UTEST_ASSERT(v.size() == 2);
        UTEST_ASSERT(ptr = v.front());
        UTEST_ASSERT(*ptr == 10);
        UTEST_ASSERT(ptr = v.back());
        UTEST_ASSERT(*ptr == 9);
        UTEST_ASSERT(v.pop_front(value));
        UTEST_ASSERT(value == 10);
        UTEST_ASSERT(v.pop_front(value));
        UTEST_ASSERT(value == 9);
    }

    void test_cleanup_operations()
    {
        printf("Testing ddeque cleanup operations...\n");

        lltl::ddeque<int> v(8);
        int x = 1;

        // Test cleanup
        UTEST_ASSERT(v.size() == 0);
        UTEST_ASSERT(v.capacity() == 0);
        UTEST_ASSERT(v.chunks() == 0);
        UTEST_ASSERT(v.used_chunks() == 0);
        UTEST_ASSERT(v.extra_chunks() == 0);

        // Some push operations
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.push_back(x++));
        UTEST_ASSERT(v.size() == 2);
        UTEST_ASSERT(v.capacity() == 8);
        UTEST_ASSERT(v.chunks() == 1);
        UTEST_ASSERT(v.used_chunks() == 1);
        UTEST_ASSERT(v.extra_chunks() == 0);

        // Some pop operations
        UTEST_ASSERT(v.pop_back());
        UTEST_ASSERT(v.pop_back());
        UTEST_ASSERT(v.size() == 0);
        UTEST_ASSERT(v.capacity() == 8);
        UTEST_ASSERT(v.chunks() == 1);
        UTEST_ASSERT(v.used_chunks() == 0);
        UTEST_ASSERT(v.extra_chunks() == 1);

        // Push many items
        for (size_t i=0; i<32; ++i)
            UTEST_ASSERT(v.push_back(x++));

        UTEST_ASSERT(v.size() == 32);
        UTEST_ASSERT(v.capacity() == 32);
        UTEST_ASSERT(v.chunks() == 4);
        UTEST_ASSERT(v.used_chunks() == 4);
        UTEST_ASSERT(v.extra_chunks() == 0);

        // Pop half items
        for (size_t i=0; i<16; ++i)
            UTEST_ASSERT(v.pop_back());

        UTEST_ASSERT(v.size() == 16);
        UTEST_ASSERT(v.capacity() == 32);
        UTEST_ASSERT(v.chunks() == 4);
        UTEST_ASSERT(v.used_chunks() == 2);
        UTEST_ASSERT(v.extra_chunks() == 2);

        // Test clear()
        v.clear();
        UTEST_ASSERT(v.size() == 0);
        UTEST_ASSERT(v.capacity() == 32);
        UTEST_ASSERT(v.chunks() == 4);
        UTEST_ASSERT(v.used_chunks() == 0);
        UTEST_ASSERT(v.extra_chunks() == 4);

        // Push many items
        for (size_t i=0; i<32; ++i)
            UTEST_ASSERT(v.push_front(x++));

        UTEST_ASSERT(v.size() == 32);
        UTEST_ASSERT(v.capacity() == 32);
        UTEST_ASSERT(v.chunks() == 4);
        UTEST_ASSERT(v.used_chunks() == 4);
        UTEST_ASSERT(v.extra_chunks() == 0);

        // Pop half items
        for (size_t i=0; i<16; ++i)
            UTEST_ASSERT(v.pop_back());

        UTEST_ASSERT(v.size() == 16);
        UTEST_ASSERT(v.capacity() == 32);
        UTEST_ASSERT(v.chunks() == 4);
        UTEST_ASSERT(v.used_chunks() == 2);
        UTEST_ASSERT(v.extra_chunks() == 2);

        // Test truncate()
        v.truncate();
        UTEST_ASSERT(v.size() == 16);
        UTEST_ASSERT(v.capacity() == 16);
        UTEST_ASSERT(v.chunks() == 2);
        UTEST_ASSERT(v.used_chunks() == 2);
        UTEST_ASSERT(v.extra_chunks() == 0);

        // Push many items
        for (size_t i=0; i<16; ++i)
            UTEST_ASSERT(v.push_front(x++));

        UTEST_ASSERT(v.size() == 32);
        UTEST_ASSERT(v.capacity() == 32);
        UTEST_ASSERT(v.chunks() == 4);
        UTEST_ASSERT(v.used_chunks() == 4);
        UTEST_ASSERT(v.extra_chunks() == 0);

        // Pop half items
        for (size_t i=0; i<16; ++i)
            UTEST_ASSERT(v.pop_front());

        UTEST_ASSERT(v.size() == 16);
        UTEST_ASSERT(v.capacity() == 32);
        UTEST_ASSERT(v.chunks() == 4);
        UTEST_ASSERT(v.used_chunks() == 2);
        UTEST_ASSERT(v.extra_chunks() == 2);

        // Test reserve()
        UTEST_ASSERT(v.reserve(64));
        UTEST_ASSERT(v.size() == 16);
        UTEST_ASSERT(v.capacity() == 64);
        UTEST_ASSERT(v.chunks() == 8);
        UTEST_ASSERT(v.used_chunks() == 2);
        UTEST_ASSERT(v.extra_chunks() == 6);

        // Test flush()
        v.flush();
        UTEST_ASSERT(v.size() == 0);
        UTEST_ASSERT(v.capacity() == 0);
        UTEST_ASSERT(v.chunks() == 0);
        UTEST_ASSERT(v.used_chunks() == 0);
        UTEST_ASSERT(v.extra_chunks() == 0);
    }

    UTEST_MAIN
    {
        test_simple_single_operations();
        test_cleanup_operations();
    }

UTEST_END




