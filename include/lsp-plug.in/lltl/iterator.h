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
#include <lsp-plug.in/lltl/types.h>

namespace lsp
{
    namespace lltl
    {
        struct raw_iterator;

        /**
         * Function to check validity of the raw iterator
         * @param i iterator to check validity
         * @return true if iterator is valid
         */
        typedef bool (*iter_check_t)(raw_iterator *i);

        /**
         * Advance to next record in post-increment/pre-increment mode.
         * Post-increment mode should malloc() and return copy of iterator advanced to n steps.
         *
         * @param i pointer to the iterator
         * @param n nummber of steps to advance
         * @return pointer to modified raw iterator
         */
        typedef raw_iterator *(*iter_move_t)(raw_iterator *i, size_t n);

        /**
         * Obtain current value the iterator points to
         * @param i pointer to iterator
         */
        typedef void *(*iter_get_t)(raw_iterator *i);

        /**
         * Remove current value the iterator points to and advance to the next value
         * @param i pointer to iterator
         * @return pointer to the item that has been removed
         */
        typedef void *(*iter_remove_t)(raw_iterator *i);

        /**
         * Raw iterator interface with set of required functions
         */
        struct LSP_LLTL_LIB_EXPORT raw_iterator
        {
            public:
                size_t              refs;       // Number of references
                size_t              change;     // Counter of changes for validity check
                iter_check_t        valid;      // Validity check
                iter_check_t        has_more;   // Check that there are still items to advance
                iter_move_t         bnext;      // Advance to next record (pre-increment)
                iter_move_t         anext;      // Advance to next record (post-increment)
                iter_get_t          get;        // Get current record
                iter_remove_t       remove;     // Current item removal

            public:
                explicit raw_iterator();

            public:
                static raw_iterator    *reference(raw_iterator *src);
                static raw_iterator    *dereference(raw_iterator *src);
                static raw_iterator    *replace(raw_iterator *it, raw_iterator *rep);
        };

        template <class T>
            class iterator
            {
                protected:
                    mutable raw_iterator    *it;    // Iterator reference

                protected:
                    inline static T *cast(void *ptr)                    { return static_cast<T *>(ptr);                     }

                protected:
                    template <class X>
                        friend iterator<X>  make_iterator(raw_iterator *it);

                    explicit inline iterator(raw_iterator *src)         { it = raw_iterator::reference(src);                }

                public:
                    explicit inline iterator(iterator<T> *src)          { it = raw_iterator::reference(src->it);            }
                    explicit inline iterator(iterator<T> &src)          { it = raw_iterator::reference(src.it);             }
                    inline ~iterator()                                  { it = raw_iterator::dereference(it); it = NULL;    }

                    inline iterator<T> &operator = (iterator<T> &src)   { it = raw_iterator::replace(it, src.it); return *this;  }
                    inline iterator<T> &operator = (iterator<T> *src)   { it = raw_iterator::replace(it, src->it); return *this; }

                public:
                    /**
                     * Check iterator for validity
                     * @return true if iterator is valid
                     */
                    inline bool         valid() const       { return (it != NULL) && (it->valid(it));               }

                    /**
                     * Check that iterator is valid and can advance
                     * @return true if iterator can advance
                     */
                    inline bool         contains() const    { return (it != NULL) && (it->has_more(it));            }
                    inline operator     bool() const        { return contains();                                    }

                    /**
                     * Check that iterator is valid and is pointing at the end of the collection
                     * @return true if iterator is valid and is pointing at the end of the collection
                     */
                    inline bool         end() const         { return (it == NULL) || (!it->has_more(it));           }
                    inline bool operator !() const          { return end();                                         }

                    /** Advance iterator (pre-increment)
                     *
                     * @return advanced iterator
                     */
                    inline iterator<T> &bnext()
                    {
                        if (it != NULL) it = it->bnext(it, 1);
                        return *this;
                    }
                    inline iterator<T> &operator ++()       { return bnext();                                       }

                    /** Advance iterator for the specified number of elements
                     *
                     * @return
                     */
                    inline iterator<T> &advance(size_t n)
                    {
                        if (it != NULL) it = it->bnext(it, n);
                        return *this;
                    }
                    inline iterator<T> &operator +=(ssize_t n)  { return advance(n);                                }

                    /** Advance iterator (post-increment)
                     *
                     * @return advanced iterator
                     */
                    inline iterator<T>  anext()
                    {
                        raw_iterator *prev = it;
                        raw_iterator *xit  = (it != NULL) ? it->anext(it, 1) : NULL;
                        it = raw_iterator::replace(it, xit);
                        return iterator<T>(prev);
                    }
                    inline iterator<T>  operator ++(int)    { return anext();                                       }

                    /** Get current item or return NULL if there is no item
                     *
                     * @return current item
                     */
                    inline T           *get()               { return (it != NULL) ? cast(it->get(it)) : NULL;       }
                    inline T *operator *()                  { return get();                                         }
                    inline T *operator ->()                 { return get();                                         }

                    /**
                     * Remove current item and advance to the next one (if it is present)
                     * @return true on success
                     */
                    inline T           *remove()            { return (it != NULL) ? cast(it->remove(it)) : NULL;    }

                    /**
                     * Swap data between two iterators
                     * @param src source iterator
                     */
                    inline void         swap(iterator<T> *src)  { T *tmp = it; it = src->it; src->it = tmp;         }
                    inline void         swap(iterator<T> &src)  { T *tmp = it; it = src.it; src.it = tmp;           }

                    /**
                     * Get number of change for collection
                     * @return number of change for collection
                     */
                    inline size_t       change() const      { return (it != NULL) ? it->change : 0;                 }

                    /**
                     * Get number of references
                     * @return number of references
                     */
                    inline size_t       refs() const      { return (it != NULL) ? it->refs : 0;                     }
            };

        template <class T>
            inline iterator<T>  make_iterator(raw_iterator *it)     { return iterator<T>(it);                       }
    }
}

#endif /* LSP_PLUG_IN_LLTL_ITERATOR_H_ */
