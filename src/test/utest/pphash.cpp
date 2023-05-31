/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 11 мая 2020 г.
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
#include <lsp-plug.in/lltl/pphash.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/stdlib/string.h>

UTEST_BEGIN("lltl", pphash)

    void test_basic()
    {
        lltl::parray<char> k, v;
        lltl::pphash<char, char> h;

        char **ps, *xv, *ov, *s;

        printf("Testing basic functions...\n");

        // Check initial state
        UTEST_ASSERT(h.size() == 0);
        UTEST_ASSERT(h.capacity() == 0);
        UTEST_ASSERT(h.is_empty());
        UTEST_ASSERT(xv = ::strdup("test value"));

        // Get from empty
        UTEST_ASSERT(!(s = h.get("test")));

        // Put items first
        UTEST_ASSERT(ps = h.put("key1", NULL));
        UTEST_ASSERT(*ps = ::strdup("value1"));
        UTEST_ASSERT(h.put("key2", ::strdup("value2"), NULL));
        UTEST_ASSERT(h.put("key3", ::strdup("value3"), NULL));
        UTEST_ASSERT(h.put(NULL, ::strdup("null value"), NULL));
        UTEST_ASSERT(h.size() == 4);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Create items
        UTEST_ASSERT(h.create("key4", ::strdup("value4")));
        UTEST_ASSERT(!h.create("key1", xv));
        UTEST_ASSERT(h.create("key5", NULL));
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Check for existence
        UTEST_ASSERT(h.exists("key1"));
        UTEST_ASSERT(h.exists("key2"));
        UTEST_ASSERT(h.exists("key3"));
        UTEST_ASSERT(h.exists("key4"));
        UTEST_ASSERT(h.exists("key5"));
        UTEST_ASSERT(h.exists(NULL));
        UTEST_ASSERT(!h.exists("unexisting"));
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Check reads
        UTEST_ASSERT(s = h.get("key3"));
        UTEST_ASSERT(::strcmp(s, "value3") == 0);
        UTEST_ASSERT(s = h.get(NULL));
        UTEST_ASSERT(::strcmp(s, "null value") == 0);
        UTEST_ASSERT(!(s = h.get("unexisting")));
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Get with write-back
        UTEST_ASSERT(ps = h.wbget("key3"));
        UTEST_ASSERT(::strcmp(*ps, "value3") == 0);
        ::free(*ps);
        UTEST_ASSERT(*ps = ::strdup("new value3"));
        UTEST_ASSERT(s = h.get("key3"));
        UTEST_ASSERT(::strcmp(s, "new value3") == 0);
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Gets with defaults
        UTEST_ASSERT(s = h.dget("key3", xv));
        UTEST_ASSERT(::strcmp(s, "new value3") == 0);
        UTEST_ASSERT(!(s = h.dget("key5", xv)));
        UTEST_ASSERT(s = h.dget("unexisting", xv));
        UTEST_ASSERT(::strcmp(s, "test value") == 0);
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Replace items
        ov  = xv;
        UTEST_ASSERT(!h.replace("unexisting", xv, &ov));
        UTEST_ASSERT(ov == xv);
        UTEST_ASSERT(h.replace("key1", xv, &ov));
        UTEST_ASSERT(ov != NULL);
        UTEST_ASSERT(::strcmp(ov, "value1") == 0);
        ::free(ov);
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Remove items
        UTEST_ASSERT(!h.remove("unexisting", &ov));
        UTEST_ASSERT(h.remove("key2", &ov));
        UTEST_ASSERT(ov != NULL);
        UTEST_ASSERT(::strcmp(ov, "value2") == 0);
        ::free(ov);
        UTEST_ASSERT(h.size() == 5);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Get keys
        UTEST_ASSERT(h.keys(&k));
        printf("hash keys:\n");
        for (size_t i=0, n=k.size(); i<n; ++i)
            printf("  %s\n", k.uget(i));
        k.flush();

        // Get values
        UTEST_ASSERT(h.values(&v));
        printf("hash values:\n");
        for (size_t i=0, n=v.size(); i<n; ++i)
            printf("  %s\n", v.uget(i));
        v.flush();

        // Get keys and values
        UTEST_ASSERT(h.items(&k, &v));
        UTEST_ASSERT(k.size() == v.size());

        printf("hash items:\n");
        for (size_t i=0, n=k.size(); i<n; ++i)
            printf("  %s = %s\n", k.uget(i), v.uget(i));
        k.flush();

        // Clear the hash
        h.clear();
        UTEST_ASSERT(h.size() == 0);
        UTEST_ASSERT(h.capacity() == 0x10);
        h.flush();
        UTEST_ASSERT(h.size() == 0);
        UTEST_ASSERT(h.capacity() == 0);

        // Drop values
        printf("freeing hash items\n");
        for (size_t i=0, n=v.size(); i<n; ++i)
        {
            char *data = v.uget(i);
            if (data != NULL)
            {
                printf("  freeing value: %s\n", data);
                ::free(data);
            }
        }
    }

    void test_large()
    {
        char buf[32], *s;
        lltl::pphash<char, char> h;

        printf("Generating large data...\n");
        for (size_t i=0; i<100000; ++i)
        {
            ::snprintf(buf, sizeof(buf), "%08lx", long(i));
            UTEST_ASSERT(h.put(buf, ::strdup(buf), NULL));
            if (!((i+1) % 10000))
                printf("  generated %d keys\n", int(i+1));
        }
        UTEST_ASSERT(h.size() == 100000);
        UTEST_ASSERT(h.capacity() == 0x8000);

        printf("Validating contents...\n");
        for (size_t i=0; i<100000; ++i)
        {
            ::snprintf(buf, sizeof(buf), "%08lx", long(i));
            UTEST_ASSERT(s = h.get(buf));
            UTEST_ASSERT(::strcmp(s, buf) == 0);
            ::free(s);
            if (!((i+1) % 10000))
                printf("  validated %d keys\n", int(i+1));
        }
        UTEST_ASSERT(h.size() == 100000);
        UTEST_ASSERT(h.capacity() == 0x8000);
    }

    void test_iterator(size_t count)
    {
        char key[20], value[20];

        printf("Testing iterators for %d items...\n", int(count));

        // Fill hash data
        lltl::pphash<char, char> h;
        for (size_t i=0; i<count; ++i)
        {
            sprintf(key, "%04d", int(i));
            sprintf(value, "0x%04x", int(i));
            UTEST_ASSERT(h.create(key, strdup(value)));
        }

        // Fill keys
        printf("  testing keys...\n", int(count));
        lltl::parray<char> keys, rkeys;
        for (lltl::iterator<char> it = h.keys(); it; ++it)
            UTEST_ASSERT(keys.push(*it));
        UTEST_ASSERT(keys.size() == h.size());
        for (lltl::iterator<char> it = h.rkeys(); it; ++it)
            UTEST_ASSERT(rkeys.unshift(*it));
        UTEST_ASSERT(rkeys.size() == h.size());

        // Validate keys
        for (size_t i=0; i<h.size(); ++i)
            UTEST_ASSERT(keys.uget(i) == rkeys.uget(i));

        // Fill values
        printf("  testing values...\n", int(count));
        lltl::parray<char> values, rvalues;
        for (lltl::iterator<char> it = h.values(); it; ++it)
            UTEST_ASSERT(values.push(*it));
        UTEST_ASSERT(values.size() == h.size());
        for (lltl::iterator<char> it = h.rvalues(); it; ++it)
            UTEST_ASSERT(rvalues.unshift(*it));
        UTEST_ASSERT(rvalues.size() == h.size());

        // Validate values
        for (size_t i=0; i<h.size(); ++i)
            UTEST_ASSERT(values.uget(i) == rvalues.uget(i));

        // Fill pairs
        printf("  testing pairs...\n", int(count));
        lltl::parray<lltl::pair<char, char>> items, ritems;
        for (lltl::iterator<lltl::pair<char, char>> it = h.items(); it; ++it)
            UTEST_ASSERT(items.push(*it));
        UTEST_ASSERT(items.size() == h.size());
        for (lltl::iterator<lltl::pair<char, char>> it = h.ritems(); it; ++it)
            UTEST_ASSERT(ritems.unshift(*it));
        UTEST_ASSERT(ritems.size() == h.size());

        // Validate pairs
        for (size_t i=0; i<h.size(); ++i)
            UTEST_ASSERT(items.uget(i) == ritems.uget(i));

        // Validate order
        printf("  checking order...\n", int(count));
        for (size_t i=0; i<h.size(); ++i)
        {
            lltl::pair<char, char> *p = items.uget(i);
            const char *xkey = keys.uget(i);
            const char *xvalue = values.uget(i);

            UTEST_ASSERT(p->key   == xkey);
            UTEST_ASSERT(p->value == xvalue);
        }

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


