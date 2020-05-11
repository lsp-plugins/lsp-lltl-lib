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
#include <stdlib.h>
#include <sys/types.h>

namespace lsp
{
    namespace lltl
    {
        /**
         * Hashing function
         *
         * @param ptr pointer to the object to retrieve hash value, never NULL
         * @param size size of the object in bytes
         * @return hash value
         */
        typedef     size_t (* hash_func_t)(const void *ptr, size_t size);

        /**
         * Comparison function
         * @param a pointer to object a, never NULL
         * @param b pointer to object b, never NULL
         * @param size size of objects a and b
         * @return negative value if a less than b, positive if a is greater than b, 0 otherwise
         */
        typedef     ssize_t (* compare_func_t)(const void *a, const void *b, size_t size);

        /**
         * Copy function
         * @param src source object to copy, never NULL
         * @param size size of the object
         * @return pointer to copied object
         */
        typedef     void *(* copy_func_t)(const void *src, size_t size);

        /**
         * Resource destruction and freeing function
         *
         * @param ptr pointer to the data, never NULL
         */
        typedef     void (* free_func_t)(void *ptr);

        /**
         * Default hashing function
         *
         * @param ptr pointer to the object to retrieve hash value
         * @param size size of the object in bytes
         * @return hash value
         */
        size_t      default_hash_func(const void *ptr, size_t size);

        /**
         * Hash function for computing C string hash
         * @param ptr pointer to the C string
         * @param size size of char type
         * @return hash function
         */
        size_t      char_hash_func(const void *ptr, size_t size);

        /**
         * Comparison function for comparing C strings
         * @param a C string a
         * @param b C string b
         * @param size size of char type
         * @return comparison result
         */
        ssize_t     char_cmp_func(const void *a, const void *b, size_t size);

        /**
         * Copying character data (C string)
         * @param ptr pointer to character string
         * @param size size of char type
         * @return pointer to allocated C string
         */
        void       *char_copy_func(const void *ptr, size_t size);

        /**
         * Hash interface: set of functions to manage keys and objects
         */
        struct hash_iface
        {
            hash_func_t         hash;       // Hashing function
            compare_func_t      compare;    // Comparison function
            copy_func_t         copy;       // Copy function
            free_func_t         free;       // Free function
        };

        // Default specialization for hash interface
        template <class T>
            struct hash_impl: public hash_iface
            {
                static inline void *operator new(size_t size, void *ptr) { return ptr; }

                static void *copy_func(const void *src, size_t size)
                {
                    void *dst = ::malloc(size);
                    return (dst) ? new(dst) T(static_cast<const T *>(src)) : NULL;
                }

                static void free_func(void *ptr)
                {
                    (static_cast<T *>(ptr))->~T();
                    ::free(ptr);
                }

                inline hash_impl()
                {
                    hash        = default_hash_func;
                    compare     = ::memcmp;
                    copy        = copy_func;
                    free        = free_func;
                }
            };

        // Specialization for char * data (C string)
        template <>
            struct hash_impl<char>: public hash_iface
            {
                inline hash_impl()
                {
                    hash        = char_hash_func;
                    compare     = char_cmp_func;
                    copy        = char_copy_func;
                    free        = ::free;
                }
            };
    }
}

#endif /* LSP_PLUG_IN_LLTL_TYPES_H_ */
