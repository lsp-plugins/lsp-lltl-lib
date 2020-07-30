/*
 * bitset.h
 *
 *  Created on: 30 июл. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_BITSET_H_
#define LSP_PLUG_IN_LLTL_BITSET_H_

#include <lsp-plug.in/common/types.h>

namespace lsp
{
    namespace lltl
    {
        class bitset
        {
            private:
                bitset &operator = (const bitset &);

            protected:
                size_t          nSize;
                size_t          nCapacity;
                umword_t       *vData;

            public:
                explicit        bitset();
                ~bitset();

            public:
                inline size_t   size() const                { return nSize;                         }
                inline size_t   capacity() const            { return nCapacity * sizeof(umword_t);  }

            public:
                bool            resize(size_t size);
                void            flush();
                void            clear();

            public:
                bool            get(size_t index) const;

                void            set_all();
                bool            set(size_t index);
                bool            set(size_t index, bool value);
                size_t          set(size_t index, size_t count);
                size_t          set(size_t index, size_t count, const bool *values);

                void            unset_all();
                bool            unset(size_t index);
                size_t          unset(size_t index, size_t count);

                void            toggle_all();
                bool            toggle(size_t index);
                size_t          toggle(size_t index, size_t count);

            public:
                void            swap(bitset *dst);
        };
    }
}

#endif /* LSP_PLUG_IN_LLTL_BITSET_H_ */
