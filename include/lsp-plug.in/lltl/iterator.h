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

        struct LSP_LLTL_LIB_PUBLIC iter_cmp_result_t
        {
            ssize_t             res;
            bool                valid;
        };

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
                static const raw_iterator invalid;

            public:
                const iter_vtbl_t      *vtable;     // Virtual table, SHOULD NEVER be NULL, use invalid_vtbl for invalid iterators
                void                   *container;  // Pointer to container that holds data, SHOULD be NULL if iterator is invalid
                void                   *item;       // Pointer to current item
                size_t                  offset;     // Offset from the beginning of the container
                bool                    reversive;  // Reversive flag

            public:
                void                    advance(ssize_t n);
                iter_cmp_result_t       compare_to(const raw_iterator *b) const;
                bool                    valid() const;
        };

        /**
         * Iterator class
         */
        template <class T>
        class iterator
        {
            public:
                static const iterator<T> invalid;

            protected:
                mutable raw_iterator        v;

            private:
                inline static T *cast(void *ptr)                                { return reinterpret_cast<T *>(ptr);       }
                inline static const T *ccast(void *ptr)                         { return const_cast<const T *>(cast(ptr));  }

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
                    v   = raw_iterator::invalid;
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
                inline iterator<T> & operator ++() { v.advance(1);  return *this; }
                inline iterator<T> & operator --() { v.advance(-1); return *this; }

                inline iterator<T> operator ++ (int)
                {
                    raw_iterator tmp = v;
                    v.advance(1);
                    return iterator<T>(lsp::move(tmp));
                }

                inline iterator<T> operator -- (int)
                {
                    raw_iterator tmp = v;
                    v.advance(-1);
                    return iterator<T>(lsp::move(tmp));
                }

                inline iterator<T> operator + (ssize_t offset)
                {
                    raw_iterator tmp = v;
                    v.advance(offset);
                    return iterator<T>(lsp::move(tmp));
                }

                inline iterator<T> operator - (ssize_t offset)
                {
                    raw_iterator tmp = v;
                    v.advance(-offset);
                    return iterator<T>(lsp::move(tmp));
                }

                inline iterator<T> operator += (ssize_t offset) { v.advance(offset);   return *this; }
                inline iterator<T> operator -= (ssize_t offset) { v.advance(-offset);  return *this; }

            public: // Validation
                inline operator bool() const    { return v.valid(); }
                inline bool operator !() const  { return v.valid(); }

            public: // Dereferencing, should be applied for valid iterators only
                inline T * operator *()
                {
                    T *tmp          = cast(v.vtable->get(&v));
                    return tmp;
                }

                inline const T * operator *() const
                {
                    const T *tmp    = ccast(v.vtable->get(&v));
                    return tmp;
                }

                inline T & operator ->()
                {
                    T *tmp          = cast(v.vtable->get(&v));
                    return *tmp;
                }

                inline const T & operator ->() const
                {
                    const T *tmp    = ccast(v.vtable->get(&v));
                    return *tmp;
                }

            public:  // Comparison operators
                inline bool operator < (const iterator<T> & it) const
                {
                    iter_cmp_result_t res = v.compare_to(&it.v);
                    return (res.valid) ? res.res < 0 : false;
                }

                inline bool operator <= (const iterator<T> & it) const
                {
                    iter_cmp_result_t res = v.compare_to(&it.v);
                    return (res.valid) ? res.res <= 0 : false;
                }

                inline bool operator == (const iterator<T> & it) const
                {
                    iter_cmp_result_t res = v.compare_to(&it.v);
                    return (res.valid) ? res.res == 0 : false;
                }

                inline bool operator != (const iterator<T> & it) const
                {
                    iter_cmp_result_t res = v.compare_to(&it.v);
                    return (res.valid) ? res.res != 0 : false;
                }

                inline bool operator >= (const iterator<T> & it) const
                {
                    iter_cmp_result_t res = v.compare_to(&it.v);
                    return (res.valid) ? res.res >= 0 : false;
                }

                inline bool operator > (const iterator<T> & it) const
                {
                    iter_cmp_result_t res = v.compare_to(&it.v);
                    return (res.valid) ? res.res > 0 : false;
                }
        };

        template <class T>
        const iterator<T> iterator<T>::invalid = iterator<T>(raw_iterator::invalid);

    } /* namespace lltl */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_LLTL_ITERATOR_H_ */
