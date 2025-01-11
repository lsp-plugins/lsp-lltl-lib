/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 12 нояб. 2023 г.
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
#include <lsp-plug.in/lltl/ptrset.h>
#include <lsp-plug.in/lltl/spec.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("lltl", ptrset)

    void test_basic()
    {
        constexpr size_t N = 32;

        lltl::ptrset<int> s;
        int *xv[N], *yv[N];
        for (size_t i=0; i<N; ++i)
        {
            xv[i]   = new int(int(i));
            UTEST_ASSERT(xv[i] != NULL);
            yv[i]   = new int(int(i));
            UTEST_ASSERT(yv[i] != NULL);
        }
        lsp_finally {
            for (size_t i=0; i<N; ++i)
            {
                delete xv[i];
                delete yv[i];
            }
        };

        printf("Testing basic functions...\n");

        // Check initial state
        UTEST_ASSERT(s.size() == 0);
        UTEST_ASSERT(s.capacity() == 0);
        UTEST_ASSERT(s.is_empty());

        // Check put(), get() and contains()
        for (size_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(s.put(xv[i]));
            UTEST_ASSERT(s.get(xv[i]) == xv[i]);
            UTEST_ASSERT(s.contains(xv[i]));
        }
        UTEST_ASSERT(s.size() == 32);
        UTEST_ASSERT(!s.is_empty());
        for (size_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(s.get(xv[i]) == xv[i]);
            UTEST_ASSERT(s.contains(xv[i]));
        }

        // Check duplicate put()
        for (size_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(!s.put(xv[i]));
        }

        // Check for dget()
        for (size_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(s.dget(xv[i], yv[i]) == xv[i]);
            UTEST_ASSERT(s.dget(yv[i], xv[N - i - 1]) == xv[N - i - 1]);
        }

        // Check toggle()
        for (size_t i=0; i<N/2; ++i)
        {
            UTEST_ASSERT(s.toggle(xv[i]));
            UTEST_ASSERT(s.toggle(yv[i]));
        }
        UTEST_ASSERT(s.size() == N);

        for (size_t i=0; i<N; ++i)
        {
            int *cp = (i < N/2) ? yv[i] : xv[i];
            int *mp = (i < N/2) ? xv[i] : yv[i];
            UTEST_ASSERT(s.contains(cp));
            UTEST_ASSERT(!s.contains(mp));
        }
        for (size_t i=0; i<N/2; ++i)
        {
            UTEST_ASSERT(s.toggle(xv[i]));
            UTEST_ASSERT(s.toggle(yv[i]));
        }
        UTEST_ASSERT(s.size() == N);
        for (size_t i=0; i<N; ++i)
        {
            UTEST_ASSERT(s.contains(xv[i]));
        }

        // Check remove(), get(), and contains() of existing value
        for (size_t i=0; i<N/2; ++i)
        {
            UTEST_ASSERT(!s.remove(yv[i]));
            UTEST_ASSERT(s.remove(xv[i]));
            UTEST_ASSERT(s.size() == N - i - 1);
        }
        UTEST_ASSERT(s.size() == N/2);
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
    }

    void test_large()
    {
        lltl::parray<int> v, rv;
        lsp_finally {
            // Drop allocated items
            for (size_t i=0, n=v.size(); i<n; ++i)
            {
                int *item = v.uget(i);
                if (item != NULL)
                    delete item;
            }
            v.flush();
        };

        lltl::ptrset<int> s;
        int *p = NULL;

        printf("Generating large data...\n");
        for (size_t i=0; i<100000; ++i)
        {
            UTEST_ASSERT(p = new int(int(i)));
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
        printf("Testing iterators for %d items...\n", int(count));

        // Fill hash data
        lltl::ptrset<int> h;
        for (size_t i=0; i<count; ++i)
        {
            UTEST_ASSERT(h.put(new int(int(i))));
        }
        UTEST_ASSERT(h.size() == count);

        lltl::iterator<int> start = h.values();
        ssize_t x;

        // Fill values
        printf("  testing iterator for %d values...\n", int(count));
        lltl::parray<int> values, rvalues;

        x = 0;

        for (lltl::iterator<int> it = h.values(); it; ++it, ++x)
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
        UTEST_ASSERT_MSG(values.size() == h.size(),
            "values = %d, h = %d", int(values.size()), int(h.size()));

        x = count - 1;
        for (lltl::iterator<int> it = h.rvalues(); it; ++it, --x)
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
        UTEST_ASSERT_MSG(rvalues.size() == h.size(),
            "rvalues = %d, h = %d", int(rvalues.size()), int(h.size()));

        // Validate values
        for (size_t i=0; i<h.size(); ++i)
            UTEST_ASSERT(values.uget(i) == rvalues.uget(i));

        // Iterate with large steps
        lltl::iterator<int> fit = h.values();
        lltl::iterator<int> bit = h.rvalues();
        for (size_t i=0; i<h.size(); i += 21)
        {
            UTEST_ASSERT(fit);
            UTEST_ASSERT(bit);
            UTEST_ASSERT(fit.valid());
            UTEST_ASSERT(bit.valid());

            int *fval = values.uget(i);
            int *bval = rvalues.uget(h.size() - i - 1);
            UTEST_ASSERT( *fit == fval );
            UTEST_ASSERT( *bit == bval );

            fit    += 21;
            bit    += 21;
        }

        UTEST_ASSERT(!fit);
        UTEST_ASSERT(!bit);
        UTEST_ASSERT(!fit.valid());
        UTEST_ASSERT(!bit.valid());

        // Drop values
        for (lltl::iterator<int> it = values.values(); it; ++it)
            delete *it;
    }

    UTEST_MAIN
    {
        test_basic();
        test_large();
        test_iterator(10);
        test_iterator(100);
        test_iterator(1000);
        test_iterator(10000);
    }

UTEST_END



