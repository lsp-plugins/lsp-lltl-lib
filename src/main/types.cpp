/*
 * types.cpp
 *
 *  Created on: 11 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/lltl/types.h>
#include <lsp-plug.in/common/types.h>

namespace lsp
{
    namespace lltl
    {
        size_t default_hash_func(const void *ptr, size_t size)
        {
            size_t v, hash = 0;
            const size_t *p = static_cast<const size_t *>(ptr);

            // Main part
            for ( ; size >= sizeof(size_t); size -= sizeof(size_t))
            {
                v       = *(p++);

                hash    = (hash << (sizeof(size_t)*4 + 1)) | (hash >> (sizeof(size_t)*4 - 1));  // Rotate hash
                v       = (v << 7) + (v << 4) + v;                          // v *= 137
                hash   ^= v;
            }

            // optional part
            if (size > 0)
            {
                v = 0;
                for (const uint8_t *z = reinterpret_cast<const uint8_t *>(p); size > 0; --size)
                    v       = (v << 8) | *(z++);

                hash    = (hash << (sizeof(size_t)*4 + 1)) | (hash >> (sizeof(size_t)*4 - 1));  // Rotate hash
                v       = (v << 7) + (v << 4) + v;                          // v *= 137
                hash   ^= v;
            }

            return hash;
        }

        size_t char_hash_func(const void *ptr, size_t size)
        {
            const uint8_t *s = static_cast<const uint8_t *>(ptr);
            size_t hash = 0;

            while (true)
            {
                size_t  v = *(s++);
                if (v == 0)
                    break;

                hash    = ((hash << 7) + (hash << 4) + hash) ^ v;
            }

            return hash;
        }

        size_t ptr_hash_func(const void *ptr, size_t size)
        {
            uintptr_t a     = uintptr_t(ptr);
            uintptr_t v     = (a >> 3) | (a << (sizeof(uintptr_t)*8 - 3));

            // Generate two pseudo-random values
            uintptr_t h1    = v * 0x4ef1d1e9 + 0x46777db9;
            uintptr_t h2    = v * 0x4b0faf0d + 0x412318bb;

            // Shuffle data
            #ifdef ARCH_64BIT
                h1              = ((h1 & 0xffff0000ffff0000ULL) >> 16) | ((h1 & 0x0000ffff0000ffffULL) << 16);
                h2              = (h2 >> 13) | (h2 << 51);
            #else
                h1              = ((h1 & 0xffff0000UL) >> 16) | ((h1 & 0x0000ffffUL) << 16);
                h2              = (h2 >> 13) | (h2 << 19);
            #endif

            // Return the final result as xor'ed expression
            return h1 ^ h2 ^ a;
        }

        ssize_t char_cmp_func(const void *a, const void *b, size_t size)
        {
            return ::strcmp(static_cast<const char *>(a), static_cast<const char *>(b));
        }

        ssize_t ptr_cmp_func(const void *a, const void *b, size_t size)
        {
            return (a > b) ? 1 : (a < b) ? -1 : 0;
        }

        void *char_clone_func(const void *ptr, size_t size)
        {
            return ::strdup(static_cast<const char *>(ptr));
        }
    }
}


