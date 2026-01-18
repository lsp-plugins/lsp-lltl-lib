/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 18 янв. 2026 г.
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

#include <lsp-plug.in/common/atomic.h>
#include <lsp-plug.in/lltl/shbuffer.h>
#include <lsp-plug.in/test-fw/utest.h>

namespace lsp
{
    static uatomic_t deleter_calls;

    static inline void reset_calls()
    {
        atomic_store(&deleter_calls, 0);
    }

    static inline uatomic_t num_calls()
    {
        return atomic_load(&deleter_calls);
    }

    static void deleter(int *ptr)
    {
        atomic_add(&deleter_calls, 1);
        free(ptr);
    }
} /* namespace lsp */

UTEST_BEGIN("lltl", shbuffer)

    void test_copy()
    {
        const int v = 42;

        lltl::shbuffer<int> p;
        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);

        UTEST_ASSERT(p.set(&v));
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p != &v);
        UTEST_ASSERT(*p != &v);
        UTEST_ASSERT(**p == v);

        {
            lltl::shbuffer<int> q;
            UTEST_ASSERT(q.references() == 0);
            UTEST_ASSERT(!q);
            UTEST_ASSERT(q == NULL);

            q = p;
            UTEST_ASSERT(q.references() == 2);
            UTEST_ASSERT(q);
            UTEST_ASSERT(q != NULL);
        }

        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
    }

    void test_move()
    {
        const int v = 42;

        lltl::shbuffer<int> p;
        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);

        UTEST_ASSERT(p.set(&v));
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);

        {
            lltl::shbuffer<int> q;
            UTEST_ASSERT(q.references() == 0);
            UTEST_ASSERT(!q);
            UTEST_ASSERT(q == NULL);

            q = lsp::move(p);
            UTEST_ASSERT(q.references() == 1);
            UTEST_ASSERT(q);
            UTEST_ASSERT(q != NULL);

            UTEST_ASSERT(p.references() == 0);
            UTEST_ASSERT(!p);
            UTEST_ASSERT(p == NULL);
        }

        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);
    }

    void test_reset()
    {
        const int v = 42;

        lltl::shbuffer<int> p(&v);
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 1);
        UTEST_ASSERT(p.bytes() == sizeof(int));

        p.reset();

        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);
    }

    void test_replace()
    {
        const int v1 = 42;
        const int v2 = 13;

        lltl::shbuffer<int> p(&v1);
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 1);
        UTEST_ASSERT(p.bytes() == sizeof(int));
        UTEST_ASSERT(*p != &v1);
        UTEST_ASSERT(**p == v1);

        UTEST_ASSERT(p.set(&v2));
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 1);
        UTEST_ASSERT(p.bytes() == sizeof(int));
        UTEST_ASSERT(*p != &v2);
        UTEST_ASSERT(**p == v2);

        p = NULL;

        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);
    }

    void test_map()
    {
        const int v1 = 42;
        const int v2 = 13;

        lltl::shbuffer<int> p(&v1, 1, NULL);
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 1);
        UTEST_ASSERT(p.bytes() == sizeof(int));
        UTEST_ASSERT(*p == &v1);
        UTEST_ASSERT(**p == v1);

        UTEST_ASSERT(p.map(&v2));
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 1);
        UTEST_ASSERT(p.bytes() == sizeof(int));
        UTEST_ASSERT(*p == &v2);
        UTEST_ASSERT(**p == v2);

        p = NULL;

        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);
    }

    void test_map_autodelete()
    {
        reset_calls();
        UTEST_ASSERT(num_calls() == 0);

        int *v1 = static_cast<int *>(malloc(sizeof(int)));
        UTEST_ASSERT(v1 != NULL);
        *v1 = 42;

        int *v2 = static_cast<int *>(malloc(sizeof(int)));
        UTEST_ASSERT(v2 != NULL);
        *v2 = 12;

        lltl::shbuffer<int> p(v1, 1, deleter);
        UTEST_ASSERT(num_calls() == 0);
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 1);
        UTEST_ASSERT(p.bytes() == sizeof(int));
        UTEST_ASSERT(*p == v1);

        UTEST_ASSERT(p.map(v2, 1, deleter));
        UTEST_ASSERT(num_calls() == 1);
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 1);
        UTEST_ASSERT(p.bytes() == sizeof(int));
        UTEST_ASSERT(*p == v2);

        p = NULL;
        UTEST_ASSERT(num_calls() == 2);

        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);
    }

    void test_indexing()
    {
        const int v[4] = {1, 2, 3, 4};

        lltl::shbuffer<int> p(v, 4);
        UTEST_ASSERT(p.references() == 1);
        UTEST_ASSERT(p);
        UTEST_ASSERT(p != NULL);
        UTEST_ASSERT(p.count() == 4);
        UTEST_ASSERT(p.bytes() == sizeof(int) * 4);

        for (int i=0; i<4; ++i)
        {
            UTEST_ASSERT(p[i] != NULL);
            UTEST_ASSERT(p.get(i) != NULL);
            UTEST_ASSERT(*(p[i]) == (i + 1));
            UTEST_ASSERT(*(p.get(i)) == (i + 1));
        }

        UTEST_ASSERT(p[5] == NULL);
        UTEST_ASSERT(p.get(5) == NULL);

        p = NULL;

        UTEST_ASSERT(p.references() == 0);
        UTEST_ASSERT(!p);
        UTEST_ASSERT(p == NULL);
    }

    UTEST_MAIN
    {
        test_copy();
        test_move();
        test_reset();
        test_replace();
        test_map();
        test_map_autodelete();
        test_indexing();
    }

UTEST_END






