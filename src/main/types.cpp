/*
 * types.cpp
 *
 *  Created on: 11 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/lltl/types.h>

namespace lsp
{
    namespace lltl
    {
        size_t default_hash_func(const void *ptr, size_t size)
        {
            size_t v, hash = 0;
            const size_t *p = static_cast<const size_t *>(ptr);

            // Main part
            while (size >= sizeof(size_t))
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
    }
}


