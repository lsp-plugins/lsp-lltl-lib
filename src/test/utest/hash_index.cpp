/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 9 нояб. 2025 г.
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
#include <lsp-plug.in/lltl/hash_index.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/stdlib/string.h>

inline namespace
{
    typedef struct payload_t
    {
    } payload_t;

    static inline payload_t * make_payload(size_t value)
    {
        return reinterpret_cast<payload_t *>(value);
    }

    size_t payload_hash_func(const void *ptr, size_t size)
    {
        return size_t(ptr);
    }

    ssize_t payload_cmp_func(const void *a, const void *b, size_t size)
    {
        return intptr_t(a) - intptr_t(b);
    }
}

namespace lsp
{
    namespace lltl
    {
        template <>
        struct hash_spec<payload_t>: public hash_iface
        {
            inline hash_spec()
            {
                hash        = payload_hash_func;
            }
        };

        template <>
        struct compare_spec<payload_t>: public compare_iface
        {
            inline compare_spec()
            {
                compare     = payload_cmp_func;
            }
        };
    } /* namespace lltl */
} /* namespace lsp */

UTEST_BEGIN("lltl", hash_index)

    void test_reallocation()
    {
        lltl::hash_index<payload_t, payload_t> index;
        UTEST_ASSERT(index.capacity() == 0);

        for (size_t i=0; i<0x800; i += 0x10)
        {
            payload_t *payload = make_payload(0x1000 + i);
            UTEST_ASSERT(index.create(payload, payload));
        }
        UTEST_ASSERT(index.capacity() > 0x10);
        UTEST_ASSERT(index.size() == 0x80);

        index.clear();
        UTEST_ASSERT(index.capacity() > 0);
        UTEST_ASSERT(index.size() == 0);
        UTEST_ASSERT(index.is_empty());
    }

    void test_large()
    {
        lltl::hash_index<payload_t, payload_t> index;

        // Fill index
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *payload = make_payload(0x100000 + i);
            UTEST_ASSERT(index.create(payload, payload));
        }
        UTEST_ASSERT(index.size() == 0x100000);

        // Fail to create already existing values
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *payload = make_payload(0x100000 + i);
            UTEST_ASSERT(!index.create(payload, payload));
        }

        // Verify that index contains data
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *payload = make_payload(0x100000 + i);
            UTEST_ASSERT(index.contains(payload));
            UTEST_ASSERT(index.get(payload) == payload);
        }

        // Cleanup index and fill it using put() operation
        index.clear();
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *payload = make_payload(0x100000 + i);
            payload_t *old = make_payload(42);
            UTEST_ASSERT(index.put(payload, payload, &old));
            UTEST_ASSERT(old == NULL);
        }
        UTEST_ASSERT(index.size() == 0x100000);

        // Verify that index does not contain data
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *payload = make_payload(0x200000 + i);
            UTEST_ASSERT(!index.contains(payload));
        }

        // Replace items
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *src = make_payload(0x100000 + i);
            payload_t *dst = make_payload(0x200000 + i);
            payload_t *old = NULL;

            UTEST_ASSERT(index.replace(src, dst, &old));
            UTEST_ASSERT(old == src);
        }

        // Fail replacing unexisting items
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *src = make_payload(0x100000 + i);
            payload_t *dst = make_payload(0x200000 + i);
            payload_t *old = NULL;

            UTEST_ASSERT(!index.replace(dst, src, &old));
            UTEST_ASSERT(old == NULL);
        }

        // Verify that index contains data
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *key  = make_payload(0x100000 + i);
            payload_t *value= make_payload(0x200000 + i);

            UTEST_ASSERT(index.contains(key));
            UTEST_ASSERT(index.get(key) == value);
        }

        // Replace items using put();
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *src = make_payload(0x100000 + i);
            payload_t *exp = make_payload(0x200000 + i);
            payload_t *dst = make_payload(0x300000 + i);
            payload_t *old = NULL;

            UTEST_ASSERT(index.put(src, dst, &old));
            UTEST_ASSERT(old == exp);
        }

        // Obtain keys
        lltl::parray<payload_t> keys;
        UTEST_ASSERT(index.keys(&keys));
        UTEST_ASSERT(keys.size() == index.size());
        keys.qsort(payload_cmp_func);
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *payload = make_payload(0x100000 + i);
            UTEST_ASSERT(keys.uget(i) == payload);
        }
        keys.flush();

        // Obtain values
        lltl::parray<payload_t> values;
        UTEST_ASSERT(index.values(&values));
        UTEST_ASSERT(values.size() == index.size());
        values.qsort(payload_cmp_func);
        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *payload = make_payload(0x300000 + i);
            UTEST_ASSERT(values.uget(i) == payload);
        }
        values.flush();

        // Obtain both keys and values
        UTEST_ASSERT(keys.is_empty());
        UTEST_ASSERT(values.is_empty());
        UTEST_ASSERT(index.items(&keys, &values));
        UTEST_ASSERT(keys.size() == index.size());
        UTEST_ASSERT(values.size() == index.size());
        keys.qsort(payload_cmp_func);
        values.qsort(payload_cmp_func);

        for (size_t i=0; i<0x100000; ++i)
        {
            payload_t *key  = make_payload(0x100000 + i);
            payload_t *value= make_payload(0x300000 + i);

            UTEST_ASSERT(keys.uget(i) == key);
            UTEST_ASSERT(values.uget(i) == value);
        }

        // Flush index
        index.flush();
        UTEST_ASSERT(index.capacity() == 0);
        UTEST_ASSERT(index.size() == 0);
        UTEST_ASSERT(index.is_empty());
    }

    void test_iterator_partial()
    {
        lltl::hash_index<payload_t, payload_t> index;
        UTEST_ASSERT(index.capacity() == 0);

        for (size_t i=0; i<0x10; ++i)
        {
            payload_t *payload = make_payload(0x1000 + i * 0x10);
            UTEST_ASSERT(index.create(payload, payload));
        }
        for (size_t i=0; i<0x10; ++i)
        {
            payload_t *payload = make_payload(0x1008 + i * 0x10);
            UTEST_ASSERT(index.create(payload, payload));
        }

        UTEST_ASSERT(index.size() == 0x20);

        // Form the list of keys using iterator
        lltl::parray<payload_t> keys;
        size_t idx = 0;
        for (lltl::iterator<payload_t> it = index.keys(); it.valid(); ++it)
        {
            payload_t *payload = it.get();
            UTEST_ASSERT(payload != NULL);
            UTEST_ASSERT(keys.add(payload));
            UTEST_ASSERT(it.index() == idx);
            ++idx;
        }
        UTEST_ASSERT(keys.size() == index.size());

        // Form the list of reverse keys using iterator
        lltl::parray<payload_t> rkeys;
        idx = index.size();
        for (lltl::iterator<payload_t> it = index.rkeys(); it.valid(); ++it)
        {
            payload_t *payload = it.get();
            UTEST_ASSERT(payload != NULL);
            UTEST_ASSERT(rkeys.unshift(payload));

            --idx;
            UTEST_ASSERT(it.index() == idx);
        }
        UTEST_ASSERT(rkeys.size() == index.size());

        // Check that order of elements matchers
        for (size_t i=0, n=keys.size(); i<n; ++i)
        {
            payload_t *a = keys.uget(i);
            payload_t *b = rkeys.uget(i);
            UTEST_ASSERT(a == b);
        }

        // Check the validity of contents
        keys.qsort(payload_cmp_func);

        for (size_t i=0; i<0x20; ++i)
        {
            payload_t *exp = make_payload(0x1000 + i * 0x08);
            payload_t *act = keys.uget(i);
            UTEST_ASSERT(exp == act);
        }
    }

    void test_iterator_full()
    {
        lltl::hash_index<payload_t, payload_t> index;
        UTEST_ASSERT(index.capacity() == 0);

        for (size_t i=0; i<0x1000; ++i)
        {
            payload_t *payload = make_payload(0x10000 + i);
            UTEST_ASSERT(index.create(payload, payload));
        }

        UTEST_ASSERT(index.size() == 0x1000);

        // Form the list of keys using iterator
        lltl::parray<payload_t> keys;
        size_t idx = 0;
        for (lltl::iterator<payload_t> it = index.keys(); it.valid(); ++it)
        {
            payload_t *payload = it.get();
            UTEST_ASSERT(payload != NULL);
            UTEST_ASSERT(keys.add(payload));
            UTEST_ASSERT(it.index() == idx);
            ++idx;
        }
        UTEST_ASSERT(keys.size() == index.size());

        // Form the list of reverse keys using iterator
        lltl::parray<payload_t> rkeys;
        idx = index.size();
        for (lltl::iterator<payload_t> it = index.rkeys(); it.valid(); ++it)
        {
            payload_t *payload = it.get();
            UTEST_ASSERT(payload != NULL);
            UTEST_ASSERT(rkeys.unshift(payload));

            --idx;
            UTEST_ASSERT(it.index() == idx);
        }
        UTEST_ASSERT(rkeys.size() == index.size());

        // Check that order of elements matchers
        for (size_t i=0, n=keys.size(); i<n; ++i)
        {
            payload_t *a = keys.uget(i);
            payload_t *b = rkeys.uget(i);
            UTEST_ASSERT(a == b);
        }

        // Check the validity of contents
        keys.qsort(payload_cmp_func);

        for (size_t i=0; i<0x1000; ++i)
        {
            payload_t *exp = make_payload(0x10000 + i);
            payload_t *act = keys.uget(i);
            UTEST_ASSERT(exp == act);
        }
    }

    UTEST_MAIN
    {
        test_reallocation();
        test_large();
        test_iterator_partial();
        test_iterator_full();
    }

UTEST_END





