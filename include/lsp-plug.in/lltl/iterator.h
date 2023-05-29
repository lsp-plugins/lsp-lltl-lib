/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 17 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_LLTL_ITERATOR_H_
#define LSP_PLUG_IN_LLTL_ITERATOR_H_

#include <lsp-plug.in/lltl/version.h>

#include <lsp-plug.in/common/variadic.h>
#include <lsp-plug.in/lltl/types.h>

namespace lsp
{
    namespace lltl
    {
        struct raw_iterator;

        /**
         * Advance to next record in post-increment/pre-increment mode.
         * Post-increment mode should malloc() and return copy of iterator advanced to n steps.
         *
         * @param i pointer to the iterator
         * @param n number of steps to advance (can be negative)
         * @return pointer to modified raw iterator
         */
        typedef void (*iter_move_t)(raw_iterator *i, ssize_t n);

        typedef void *(*iter_get_t)(raw_iterator *i);

        typedef ssize_t (*iter_compare_t)(const raw_iterator *a, const raw_iterator *b);

        /**
         * Virtual table of functions for the iterator
         */
        struct LSP_LLTL_LIB_PUBLIC iter_vtbl_t
        {
            iter_move_t         move;
            iter_get_t          get;
            iter_compare_t      cmp;
        };

        /**
         * Raw iterator interface with set of required functions
         */
        struct LSP_LLTL_LIB_PUBLIC raw_iterator
        {
            public:
                static const iter_vtbl_t invalid_vtbl; // Virtual table for invalid iterator

            public:
                const iter_vtbl_t      *vtable;     // Virtual table, SHOULD NEVER be NULL, use invalid_vtbl for invalid iterators
                void                   *container;  // Pointer to container that holds data, SHOULD be NULL if iterator is invalid
                void                   *item;       // Pointer to current item
                ssize_t                 offset;     // Offset from the beginning of the container

            public:
                void                    make_invalid();
        };

        /**
         * Iterator class
         */
        template <class T>
        class iterator
        {
            protected:
                mutable raw_iterator        v;

            public: // Construction/destruction
                inline explicit iterator(const raw_iterator & begin)
                {
                    v   = begin;
                }

                inline explicit iterator(raw_iterator && begin)
                {
                    v   = lsp::move(begin);
                }

                iterator(const iterator<T> & src)
                {
                    v   = src.v;
                }

                iterator(iterator<T> && src)
                {
                    v   = lsp::move(src.v);
                }

                ~iterator()
                {
                    v.make_invalid();
                }

            public: // Assignment
                inline iterator<T> & operator = (const iterator<T> & src)
                {
                    v   = src.v;
                    return *this;
                }

                inline iterator<T> & operator = (iterator<T> && src)
                {
                    v   = lsp::move(src.v);
                    return *this;
                }

            public: // Positioning
                inline iterator<T> & operator ++() { v.vtable->move(&v, 1); return *this; }
                inline iterator<T> & operator --() { v.vtable->move(&v, -1); return *this; }

                inline iterator<T> operator ++ (int)
                {
                    raw_iterator tmp = v;
                    v.vtable->move(&v, 1);
                    return iterator<T>(lsp::move(tmp));
                }

                inline iterator<T> operator -- (int)
                {
                    raw_iterator tmp = v;
                    v.vtable->move(&v, -1);
                    return iterator<T>(lsp::move(tmp));
                }

                inline iterator<T> operator + (ssize_t offset)
                {
                    raw_iterator tmp = v;
                    v.vtable->move(&v, offset);
                    return iterator<T>(lsp::move(tmp));
                }

                inline iterator<T> operator - (ssize_t offset)
                {
                    raw_iterator tmp = v;
                    v.vtable->move(&v, -offset);
                    return iterator<T>(lsp::move(tmp));
                }

            public: // Validation
                inline operator bool() const    { return v.container != NULL; }
                inline bool operator !() const  { return v.container == NULL; }

            public: // Dereferencing, should be applied for valid iterators only
                inline T & operator *()
                {
                    T *tmp     = static_cast<T *>(v.vtable->get(&v));
                    return *tmp;
                }

                inline const T & operator *() const
                {
                    const T *tmp     = const_cast<const T *>(static_cast<T *>(v.vtable->get(&v)));
                    return *tmp;
                }

                inline T & operator ->()
                {
                    T *tmp     = static_cast<T *>(v.vtable->get(&v));
                    return *tmp;
                }

                inline const T & operator ->() const
                {
                    const T *tmp     = const_cast<const T *>(static_cast<T *>(v.vtable->get(&v)));
                    return *tmp;
                }

            public:  // Comparison operators
                inline bool operator < (const iterator<T> & it) const
                {
                    return (v.container == it.v.container) &&
                        (v.vtable->cmp(&v, &it.v) < 0);
                }

                inline bool operator <= (const iterator<T> & it) const
                {
                    return (v.container == it.v.container) &&
                        (v.vtable->cmp(&v, &it.v) <= 0);
                }

                inline bool operator == (const iterator<T> & it) const
                {
                    return (v.container == it.v.container) &&
                        (v.vtable->cmp(&v, &it.v) == 0);
                }

                inline bool operator != (const iterator<T> & it) const
                {
                    return (v.container == it.v.container) &&
                        (v.vtable->cmp(&v, &it.v) != 0);
                }

                inline bool operator >= (const iterator<T> & it) const
                {
                    return (v.container == it.v.container) &&
                        (v.vtable->cmp(&v, &it.v) >= 0);
                }

                inline bool operator > (const iterator<T> & it) const
                {
                    return (v.container == it.v.container) &&
                        (v.vtable->cmp(&v, &it.v) > 0);
                }
        };

    } /* namespace lltl */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_LLTL_ITERATOR_H_ */
