/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 31 июл. 2020 г.
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
#include <lsp-plug.in/lltl/phashset.h>
#include <lsp-plug.in/lltl/spec.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/stdlib/string.h>

namespace lsp
{
    namespace
    {
        typedef struct item_t
        {
            int v;

            explicit item_t(int x): v(x) {}
        } item_t;
    }

    namespace lltl
    {
        template <>
            struct hash_spec<item_t>: public ptr_hash_iface {};

        template <>
            struct compare_spec<item_t>: public ptr_compare_iface {};
    }
}

UTEST_BEGIN("lltl", phashset)

    void test_basic()
    {
        lltl::phashset<item_t> s;
        item_t *xr = NULL;
        item_t *xv[32];
        size_t id = 0;

        printf("Testing basic functions...\n");

        // Check initial state
        UTEST_ASSERT(s.size() == 0);
        UTEST_ASSERT(s.capacity() == 0);
        UTEST_ASSERT(s.is_empty());

        // Check put(), get() and contains()
        for (size_t i=0; i<5; ++i, ++id)
        {
            UTEST_ASSERT(xv[id] = new item_t(i));
            UTEST_ASSERT(s.put(xv[id], &xr));
            UTEST_ASSERT(xr == NULL);
            UTEST_ASSERT(s.get(xv[id]) == xv[id]);
            UTEST_ASSERT(s.contains(xv[id]));
        }
        UTEST_ASSERT(s.size() == 5);
        UTEST_ASSERT(!s.is_empty());

        // Check create() and dget()
        for (size_t i=0; i<5; ++i)
        {
            UTEST_ASSERT(!s.create(xv[i]));
            UTEST_ASSERT(s.dget(xv[i], NULL) == xv[i]);
        }

        // Check toggle()
        for (size_t i=0; i<5; ++i)
        {
            UTEST_ASSERT(s.toggle(xv[i]));
            UTEST_ASSERT(s.dget(xv[i], NULL) == NULL);
            UTEST_ASSERT(s.toggle(xv[i]));
            UTEST_ASSERT(s.dget(xv[i], NULL) == xv[i]);
        }

        // Check create() of unexisting value
        for (size_t i=0; i<5; ++i, ++id)
        {
            UTEST_ASSERT(xv[id] = new item_t(i));
            UTEST_ASSERT(s.create(xv[id]));
            UTEST_ASSERT(s.contains(xv[id]));
            UTEST_ASSERT(s.get(xv[id]) == xv[id]);
        }
        UTEST_ASSERT(s.size() == 10);
        UTEST_ASSERT(!s.is_empty());

        // Check dget(), get() and contains() of unexisting value
        for (size_t i=0; i<5; ++i, ++id)
        {
            UTEST_ASSERT(xv[id] = new item_t(i));
            UTEST_ASSERT(s.dget(xv[id], xv[i]) == xv[i]);
            UTEST_ASSERT(s.get(xv[id]) == NULL);
            UTEST_ASSERT(!s.contains(xv[id]));
        }

        // Check remove(), get(), and contains() of existing value
        for (size_t i=0; i<5; ++i)
        {
            UTEST_ASSERT(s.remove(xv[i], &xr));
            UTEST_ASSERT(xr == xv[i]);
            UTEST_ASSERT(s.get(xv[i]) == NULL);
            UTEST_ASSERT(!s.contains(xv[i]));
        }
        UTEST_ASSERT(s.size() == 5);
        UTEST_ASSERT(!s.is_empty());

        // Clear the collection
        s.clear();
        UTEST_ASSERT(s.size() == 0);
        UTEST_ASSERT(s.is_empty());

        // Check for NULL
        UTEST_ASSERT(s.put(NULL));
        UTEST_ASSERT(!s.is_empty());
        UTEST_ASSERT(s.size() == 1);
        UTEST_ASSERT(s.contains(NULL));
        UTEST_ASSERT(s.get(NULL) == NULL);
        UTEST_ASSERT(s.dget(NULL, xv[0]) == NULL);

        // Drop items
        for (size_t i=0; i<id; ++i)
            delete xv[i];
    }

    void test_large()
    {
        lltl::parray<item_t> v, rv;
        lsp_finally {
            // Drop allocated items
            for (size_t i=0, n=v.size(); i<n; ++i)
            {
                item_t *item = v.uget(i);
                if (item != NULL)
                    delete item;
            }
            v.flush();
        };

        lltl::phashset<item_t> s;
        item_t *p = NULL;

        printf("Generating large data...\n");
        for (size_t i=0; i<100000; ++i)
        {
            UTEST_ASSERT(p = new item_t(i));
            UTEST_ASSERT(v.add(p));
            UTEST_ASSERT(s.put(p));
            if (!((i+1) % 10000))
                printf("  generated %d items\n", int(i+1));
        }
        UTEST_ASSERT(v.size() == 100000);
        UTEST_ASSERT(s.size() == 100000);

        printf("Validating contents...\n");
        for (size_t i=0; i<100000; ++i)
        {
            p = v.uget(i);
            UTEST_ASSERT(s.contains(p));
            if (!((i+1) % 10000))
                printf("  validated %d keys\n", int(i+1));
        }
        UTEST_ASSERT(v.size() == 100000);

        printf("Obtaining values...\n");
        UTEST_ASSERT(s.values(&rv));
        UTEST_ASSERT(rv.size() == 100000);
    }

    void test_iterator(size_t count)
    {
        char value[20];

        printf("Testing iterators for %d items...\n", int(count));

        // Fill hash data
        lltl::phashset<char> h;
        for (size_t i=0; i<count; ++i)
        {
            sprintf(value, "%04d", int(i));
            UTEST_ASSERT(h.create(strdup(value)));
        }

        lltl::iterator<char> start = h.values();
        ssize_t x;

        // Fill values
        printf("  testing values...\n", int(count));
        lltl::parray<char> values, rvalues;

        x = 0;

        for (lltl::iterator<char> it = h.values(); it; ++it, ++x)
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
            UTEST_ASSERT(it.remaining() == size_t(count - x));
            UTEST_ASSERT(it.max_advance() == size_t(count - x - 1));
            UTEST_ASSERT(it.count() == count);

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

            // Store the value
            UTEST_ASSERT(values.push(*it));
        }
        UTEST_ASSERT(values.size() == h.size());

        x = count - 1;
        for (lltl::iterator<char> it = h.rvalues(); it; ++it, --x)
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
            UTEST_ASSERT(it.count() == count);

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

            // Store the value
            UTEST_ASSERT(rvalues.unshift(*it));
        }
        UTEST_ASSERT(rvalues.size() == h.size());

        // Validate values
        for (size_t i=0; i<h.size(); ++i)
            UTEST_ASSERT(values.uget(i) == rvalues.uget(i));

        // Drop values
        for (lltl::iterator<char> it = values.values(); it; ++it)
            free(*it);
    }

    UTEST_MAIN
    {
        test_basic();
        test_large();
        test_iterator(10);
        test_iterator(100);
        test_iterator(1000);
    }

UTEST_END
