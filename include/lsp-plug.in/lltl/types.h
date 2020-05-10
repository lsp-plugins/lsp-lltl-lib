/*
 * types.h
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_TYPES_H_
#define LSP_PLUG_IN_LLTL_TYPES_H_

#include <lsp-plug.in/lltl/version.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

namespace lsp
{
    namespace lltl
    {
        /**
         * Hashing function
         *
         * @param ptr pointer to the object to retrieve hash value
         * @param size size of the object in bytes
         * @return hash value
         */
        typedef     size_t (* hash_func_t)(const void *ptr, size_t size);

        /**
         * Comparison function
         * @param a pointer to object a
         * @param b pointer to object b
         * @param size size of objects a and b
         * @return negative value if a less than b, positive if a is greater than b, 0 otherwise
         */
        typedef     ssize_t (* compare_func_t)(const void *a, const void *b, size_t size);


        /**
         * Default hashing function
         *
         * @param ptr pointer to the object to retrieve hash value
         * @param size size of the object in bytes
         * @return hash value
         */
        size_t      default_hash_func(const void *ptr, size_t size);


        template <class T>
            struct hash_iface
            {
                hash_func_t        hash;
                compare_func_t     compare;

                inline hash_iface()
                {
                    hash        = default_hash_func;
                    compare     = ::memcmp;
                }
            };
    }
}

#endif /* LSP_PLUG_IN_LLTL_TYPES_H_ */
