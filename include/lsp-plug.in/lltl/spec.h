/*
 * spec.h
 *
 *  Created on: 9 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_SPEC_H_
#define LSP_PLUG_IN_LLTL_SPEC_H_

#include <lsp-plug.in/lltl/version.h>
#include <lsp-plug.in/lltl/types.h>

namespace lsp
{
    namespace lltl
    {
        //---------------------------------------------------------------------
        // Interface for pointers
        struct ptr_hash_iface: public hash_iface
        {
            inline ptr_hash_iface()
            {
                hash        = ptr_hash_func;
            }
        };

        struct ptr_compare_iface: public compare_iface
        {
            inline ptr_compare_iface()
            {
                compare     = ptr_cmp_func;
            }
        };

        //---------------------------------------------------------------------
        // Default specializations

        /**
         * Default specialization for hash interface
         */
        template <class T>
            struct hash_spec: public hash_iface
            {
                inline hash_spec()
                {
                    hash        = default_hash_func;
                }
            };

        /**
         * Default specialization for compare interface
         */
        template <class T>
            struct compare_spec: public compare_iface
            {
                inline compare_spec()
                {
                    compare     = ::memcmp;
                }
            };

        /**
         * Default specialization for allocator interface
         */
        template <class T>
            struct allocator_spec: public allocator_iface
            {
                static inline void *operator new(size_t size, void *ptr) { return ptr; }

                static void *clone_func(const void *src, size_t size)
                {
                    void *dst = ::malloc(size);
                    return (dst) ? new(dst) T(static_cast<const T *>(src)) : NULL;
                }

                static void free_func(void *ptr)
                {
                    (static_cast<T *>(ptr))->~T();
                    ::free(ptr);
                }

                inline allocator_spec()
                {
                    clone       = clone_func;
                    free        = free_func;
                }
            };

        //---------------------------------------------------------------------
        // Specialization for C-strings: char *
        template <>
            struct hash_spec<char>: public hash_iface
            {
                inline hash_spec()
                {
                    hash        = char_hash_func;
                }
            };

        template <>
            struct compare_spec<char>: public compare_iface
            {
                inline compare_spec()
                {
                    compare     = char_cmp_func;
                }
            };

        template <>
            struct allocator_spec<char>: public allocator_iface
            {
                inline allocator_spec()
                {
                    clone       = char_clone_func;
                    free        = ::free;
                }
            };
    }
}

#endif /* LSP_PLUG_IN_LLTL_SPEC_H_ */
