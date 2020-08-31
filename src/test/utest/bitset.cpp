/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 30 июл. 2020 г.
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

#include <lsp-plug.in/lltl/bitset.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("lltl", bitset)

    void test_resize()
    {
        printf("Testing resize...\n");
        lltl::bitset x;
        UTEST_ASSERT(x.size() == 0);
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.is_empty());

        UTEST_ASSERT(x.resize(10));
        UTEST_ASSERT(x.capacity() > 0);
        UTEST_ASSERT(!x.is_empty());
        UTEST_ASSERT(x.size() == 10);

        UTEST_ASSERT(x.resize(128));
        UTEST_ASSERT(x.capacity() > 0);
        UTEST_ASSERT(!x.is_empty());
        UTEST_ASSERT(x.size() == 128);

        UTEST_ASSERT(x.resize(0));
        UTEST_ASSERT(x.capacity() == 0);
        UTEST_ASSERT(x.is_empty());
        UTEST_ASSERT(x.size() == 0);
    }

    void test_fill()
    {
        printf("Testing fill...\n");
        lltl::bitset x;

        UTEST_ASSERT(x.resize(255));
        for (size_t i=0; i<255; ++i)
            UTEST_ASSERT_MSG(!x.get(i), "bit at index %d is 1", int(i));

        x.set_all();
        for (size_t i=0; i<255; ++i)
            UTEST_ASSERT_MSG(x.get(i), "bit at index %d is 0", int(i));

        UTEST_ASSERT(x.resize(511));
        for (size_t i=0; i<255; ++i)
            UTEST_ASSERT_MSG(x.get(i), "bit at index %d is 0", int(i));
        for (size_t i=256; i<511; ++i)
            UTEST_ASSERT_MSG(!x.get(i), "bit at index %d is 1", int(i));

        UTEST_ASSERT(x.resize(128));
        for (size_t i=0; i<128; ++i)
            UTEST_ASSERT_MSG(x.get(i), "bit at index %d is 0", int(i));

        x.toggle_all();
        for (size_t i=0; i<128; ++i)
            UTEST_ASSERT_MSG(!x.get(i), "bit at index %d is 1", int(i));

        UTEST_ASSERT(x.resize(255));
        for (size_t i=0; i<255; ++i)
            UTEST_ASSERT_MSG(!x.get(i), "bit at index %d is 1", int(i));

        x.toggle_all();
        for (size_t i=0; i<255; ++i)
            UTEST_ASSERT_MSG(x.get(i), "bit at index %d is 0", int(i));

        UTEST_ASSERT(x.resize(128));
        for (size_t i=0; i<128; ++i)
            UTEST_ASSERT_MSG(x.get(i), "bit at index %d is 0", int(i));

        x.toggle_all();
        for (size_t i=0; i<128; ++i)
            UTEST_ASSERT_MSG(!x.get(i), "bit at index %d is 1", int(i));

        x.set_all();
        for (size_t i=0; i<128; ++i)
            UTEST_ASSERT_MSG(x.get(i), "bit at index %d is 0", int(i));

        x.unset_all();
        for (size_t i=0; i<128; ++i)
            UTEST_ASSERT_MSG(!x.get(i), "bit at index %d is 1", int(i));
    }

    void test_single()
    {
        lltl::bitset x;
        uint8_t buf[0x20];

        UTEST_FOREACH(size, 10, 128, 192, 255)
        {
            printf("Testing single for size %d...\n", int(size));

            UTEST_ASSERT(x.resize(size));
            x.unset_all();

            // Set 10101010...
            for (size_t i=0; i<size; i += 2)
                x.set(i);

            for (size_t i=0; i<size; ++i)
            {
                int b = (i % 2) == 0;
                int v = x.get(i);
                UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d", int(i), v, b);
            }

            // Invert: 01010101...
            x.toggle_all();
            for (size_t i=0; i<size; ++i)
            {
                int b = (i % 2) == 1;
                int v = x.get(i);
                UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d", int(i), v, b);
            }

            // Set 00010001...
            for (size_t i=1; i<size; i += 4)
                UTEST_ASSERT_MSG(x.unset(i), "unset returned 0 but expected to return 1 at index %d", int(i));
            for (size_t i=0; i<size; ++i)
            {
                int b = (i % 4) == 3;
                int v = x.get(i);
                UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d", int(i), v, b);
            }

            // Invert: 11101110...
            x.toggle_all();
            for (size_t i=0; i<size; ++i)
            {
                int b = (i % 4) != 3;
                int v = x.get(i);
                UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d", int(i), v, b);
            }

            // Toggle: 01110111...
            for (size_t i=0; i<size; i += 4)
            {
                UTEST_ASSERT_MSG(x.toggle(i), "toggle returned 0 but expected to return 1 at index %d", int(i));
                UTEST_ASSERT_MSG(!x.toggle(i+3), "toggle returned 1 but expected to return 0 at index %d", int(i+3));
            }

            for (size_t i=0; i<size; ++i)
            {
                int b = (i % 4) != 0;
                int v = x.get(i);
                UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d", int(i), v, b);
            }

            // Randomize
            ::bzero(buf, sizeof(buf));
            for (size_t i=0; i<size; ++i)
            {
                bool set = rand() % 2;
                x.set(i, set);
                if (set)
                    buf[i / 8] |= (1 << (i % 8));
            }

            for (size_t i=0; i<size; ++i)
            {
                int b = (buf[i / 8] & (1 << (i % 8))) != 0;
                int v = x.get(i);
                UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d", int(i), v, b);
            }
        }
    }

    void test_multi_set()
    {
        lltl::bitset x;

        UTEST_FOREACH(size, 10, 128, 192, 255)
        {
            printf("Testing multiple set for size %d...\n", int(size));

            UTEST_ASSERT(x.resize(size));

            for (size_t first=1; first<size; ++first)
                for (size_t last=first; last<size; ++last)
                {
                    x.unset_all();
                    x.set(first, last - first);

                    for (size_t i=0; i<size; ++i)
                    {
                        int b = (i >= first) && (i < last);
                        int v = x.get(i);
                        UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d first=%d, last=%d", int(i), v, b, int(first), int(last));
                    }
                }
        }
    }

    void test_multi_unset()
    {
        lltl::bitset x;

        UTEST_FOREACH(size, 10, 128, 192, 255)
        {
            printf("Testing multiple unset for size %d...\n", int(size));

            UTEST_ASSERT(x.resize(size));

            for (size_t first=0; first<size; ++first)
                for (size_t last=first; last<size; ++last)
                {
                    x.set_all();
                    x.unset(first, last - first);

                    for (size_t i=0; i<size; ++i)
                    {
                        int b = (i < first) || (i >= last);
                        int v = x.get(i);
                        UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d first=%d, last=%d", int(i), v, b, int(first), int(last));
                    }
                }
        }
    }

    void test_multi_toggle()
    {
        lltl::bitset x;

        UTEST_FOREACH(size, 10, 128, 192, 255)
        {
            printf("Testing multiple toggle for size %d...\n", int(size));

            UTEST_ASSERT(x.resize(size));

            for (size_t first=0; first<size; ++first)
                for (size_t last=first; last<size; ++last)
                {
                    x.set_all();
                    x.toggle(first, last - first);

                    for (size_t i=0; i<size; ++i)
                    {
                        int b = (i < first) || (i >= last);
                        int v = x.get(i);
                        UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d first=%d, last=%d", int(i), v, b, int(first), int(last));
                    }

                    x.unset_all();
                    x.toggle(first, last - first);

                    for (size_t i=0; i<size; ++i)
                    {
                        int b = (i >= first) && (i < last);
                        int v = x.get(i);
                        UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d first=%d, last=%d", int(i), v, b, int(first), int(last));
                    }
                }
        }
    }

    void test_set_random()
    {
        lltl::bitset x;
        bool buf[0x100];

        UTEST_FOREACH(size, 10, 128, 192, 255)
        {
            printf("Testing random set for size %d...\n", int(size));

            UTEST_ASSERT(x.resize(size));
            for (size_t i=0; i<size; ++i)
                buf[i] = rand() % 2;

            for (size_t first=0; first<size; ++first)
                for (size_t last=first; last<size; ++last)
                {
                    x.unset_all();
                    x.set(first, last - first, buf);

                    for (size_t i=0; i<size; ++i)
                    {
                        int b = (i >= first) && (i < last) ? buf[i-first] : 0;
                        int v = x.get(i);
                        UTEST_ASSERT_MSG(v == b, "bit at index %d is %d but expected to be %d first=%d, last=%d", int(i), v, b, int(first), int(last));
                    }
                }
        }
    }

    UTEST_MAIN
    {
        test_resize();
        test_fill();
        test_single();
        test_multi_set();
        test_multi_unset();
        test_multi_toggle();
        test_set_random();
    }

UTEST_END;


