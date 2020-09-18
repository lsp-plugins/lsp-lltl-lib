/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 31 июл. 2020 г.
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

#include <lsp-plug.in/lltl/phashset.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace lltl
    {
        void raw_phashset::destroy_bin(bin_t *bin)
        {
            for (tuple_t *curr = bin->data; curr != NULL; )
            {
                tuple_t *next   = curr->next;
                ::free(curr);
                curr            = next;
            }
            bin->size   = 0;
            bin->data   = NULL;
        }

        raw_phashset::tuple_t *raw_phashset::find_tuple(const void *value, size_t hash)
        {
            if (bins == NULL)
                return NULL;
            bin_t *bin = &bins[hash & (cap - 1)];

            if (value != NULL)
            {
                for (tuple_t *curr = bin->data; curr != NULL; curr = curr->next)
                {
                    if ((curr->hash == hash) && (cmp.compare(value, curr->value, vsize) == 0))
                        return curr;
                }
            }
            else
            {
                for (tuple_t *curr = bin->data; curr != NULL; curr = curr->next)
                {
                    if (curr->value == NULL)
                        return curr;
                }
            }
            return NULL;
        }

        raw_phashset::tuple_t *raw_phashset::remove_tuple(const void *value, size_t hash)
        {
            if (bins == NULL)
                return NULL;
            bin_t *bin = &bins[hash & (cap - 1)];

            if (value != NULL)
            {
                for (tuple_t **pcurr = &bin->data; *pcurr != NULL; )
                {
                    tuple_t *curr = *pcurr;
                    if ((curr->hash == hash) && (cmp.compare(value, curr->value, vsize) == 0))
                    {
                        *pcurr      = curr->next;
                        curr->next  = NULL;
                        --bin->size;
                        --size;
                        return curr;
                    }
                    pcurr   = &curr->next;
                }
            }
            else
            {
                for (tuple_t **pcurr = &bin->data; *pcurr != NULL; )
                {
                    tuple_t *curr = *pcurr;
                    if (curr->value == NULL)
                    {
                        *pcurr      = curr->next;
                        curr->next  = NULL;
                        --bin->size;
                        --size;
                        return curr;
                    }
                    pcurr   = &curr->next;
                }
            }
            return NULL;
        }

        raw_phashset::tuple_t *raw_phashset::create_tuple(size_t hash)
        {
            // Allocate tuple
            tuple_t *tuple  = static_cast<tuple_t *>(::malloc(sizeof(tuple_t)));
            if (tuple == NULL)
                return NULL;

            // Need to grow?
            if (size >= cap)
            {
                if (!grow())
                {
                    ::free(tuple);
                    return NULL;
                }
            }

            // Initialize tuple
            bin_t *bin      = &bins[hash & (cap - 1)];
            ++bin->size;
            ++size;

            tuple->hash     = hash;
            tuple->next     = bin->data;
            bin->data       = tuple;

            return tuple;
        }

        bool raw_phashset::grow()
        {
            bin_t *xbin, *ybin;
            size_t ncap, mask;

            // No previous allocations?
            if (cap == 0)
            {
                xbin            = static_cast<bin_t *>(::malloc(0x10 * sizeof(bin_t)));
                if (xbin == NULL)
                    return false; // Very bad things?

                cap             = 0x10;
                bins            = xbin;
                for (size_t i=0; i<cap; ++i, ++xbin)
                {
                    xbin->size      = 0;
                    xbin->data      = NULL;
                }

                return true;
            }

            // Twice increase the capacity of hash
            ncap            = cap << 1;
            xbin            = static_cast<bin_t *>(::realloc(bins, ncap * sizeof(bin_t)));
            if (xbin == NULL)
                return false; // Very bad things?

            // Now we need to split data
            mask            = (ncap - 1) ^ (cap - 1); // mask indicates the bit which has been set
            bins            = xbin;
            ybin            = &xbin[cap];

            for (size_t i=0; i<cap; ++i, ++xbin, ++ybin)
            {
                // There is no data in ybin by default
                ybin->size      = 0;
                ybin->data      = 0;

                // Migrate items from xbin list to ybin list
                for (tuple_t **pcurr = &xbin->data; *pcurr != NULL; )
                {
                    tuple_t *curr   = *pcurr;
                    if (curr->hash & mask)
                    {
                        *pcurr          = curr->next;
                        curr->next      = ybin->data;
                        ybin->data      = curr;
                        --xbin->size;
                        ++ybin->size;
                    }
                    else
                        pcurr           = &curr->next;
                }
            }

            // Split success
            cap         = ncap;

            return true;
        }

        void raw_phashset::flush()
        {
            // Drop all bins
            if (bins != NULL)
            {
                for (size_t i=0; i<cap; ++i)
                    destroy_bin(&bins[i]);
                ::free(bins);
                bins    = NULL;
            }

            size    = 0;
            cap     = 0;
        }

        void raw_phashset::clear()
        {
            // Just reset the size value for each bin
            if (bins != NULL)
            {
                for (size_t i=0; i<cap; ++i)
                    destroy_bin(&bins[i]);
            }

            size    = 0;
        }

        void raw_phashset::swap(raw_phashset *src)
        {
            raw_phashset tmp    = *this;
            *this               = *src;
            *src                = tmp;
        }

        void *raw_phashset::get(const void *value, void *dfl)
        {
            size_t h        = (value != NULL) ? hash.hash(value, vsize) : 0;
            tuple_t *tuple  = find_tuple(value, h);
            return (tuple != NULL) ? tuple->value : dfl;
        }

        void **raw_phashset::wbget(const void *value)
        {
            size_t h        = (value != NULL) ? hash.hash(value, vsize) : 0;
            tuple_t *tuple  = find_tuple(value, h);
            return (tuple != NULL) ? &tuple->value : NULL;
        }

        void **raw_phashset::put(void *value, void **ret)
        {
            size_t h        = (value != NULL) ? hash.hash(value, vsize) : 0;

            // Find tuple
            tuple_t *tuple  = find_tuple(value, h);
            if (tuple != NULL)
            {
                if (ret != NULL)
                    *ret        = tuple->value;
                tuple->value    = value;
                return &tuple->value;
            }

            // Not found, allocate new tuple
            tuple           = create_tuple(h);
            if (tuple == NULL)
                return NULL;

            tuple->value    = value;
            if (ret != NULL)
                *ret        = NULL;

            return &tuple->value;
        }

        void *raw_phashset::any()
        {
            if (size <= 0)
                return NULL;

            for (size_t i=0; i<cap; ++i)
            {
                bin_t *b = &bins[i];
                if (b->data != NULL)
                    return b->data->value;
            }

            return NULL;
        }

        bool raw_phashset::toggle(void *value)
        {
            size_t h        = (value != NULL) ? hash.hash(value, vsize) : 0;

            // Try to remove tuple
            tuple_t *tuple  = remove_tuple(value, h);
            if (tuple != NULL)
            {
                // Free tuple data
                ::free(tuple);
            }
            else
            {
                // Create new tuple
                tuple           = create_tuple(h);
                if (tuple == NULL)
                    return false;

                tuple->value    = value;
            }

            return true;
        }

        void **raw_phashset::create(void *value)
        {
            size_t h        = (value != NULL) ? hash.hash(value, vsize) : 0;

            // Find tuple
            tuple_t *tuple  = find_tuple(value, h);
            if (tuple != NULL)
                return NULL;

            // Create new tuple
            tuple           = create_tuple(h);
            if (tuple == NULL)
                return NULL;

            tuple->value    = value;

            return &tuple->value;
        }

        bool raw_phashset::remove(const void *value, void **ov)
        {
            size_t h        = (value != NULL) ? hash.hash(value, vsize) : 0;

            // Find tuple
            tuple_t *tuple  = remove_tuple(value, h);
            if (tuple == NULL)
                return false;

            if (ov != NULL)
                *ov         = tuple->value;

            // Free tuple data
            ::free(tuple);
            return true;
        }

        bool raw_phashset::values(raw_parray *v)
        {
            raw_parray kv;

            // Initialize collection
            kv.init();
            if (!kv.grow(size))
                return false;

            // Make a snapshot
            for (size_t i=0; i<cap; ++i)
                for (tuple_t *t = bins[i].data; t != NULL; t = t->next)
                {
                    if (!kv.append(t->value))
                    {
                        kv.flush();
                        return false;
                    }
                }

            // Return collection data
            kv.swap(v);
            kv.flush();

            return true;
        }
    }
}
