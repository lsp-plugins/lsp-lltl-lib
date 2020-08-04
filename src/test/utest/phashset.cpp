/*
 * phashset.cpp
 *
 *  Created on: 31 июл. 2020 г.
 *      Author: sadko
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

    UTEST_MAIN
    {
        test_basic();
        test_large();
    }

UTEST_END
