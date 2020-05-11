/*
 * pphash.cpp
 *
 *  Created on: 11 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/lltl/pphash.h>
#include <stdlib.h>

namespace lsp
{
    namespace lltl
    {
        void raw_pphash::destroy_bin(bin_t *bin)
        {
            for (tuple_t *curr = bin->data; curr != NULL; )
            {
                tuple_t *next   = curr->next;
                ::free(curr);
                curr            = next;
            }
            bin->data   = NULL;
        }

        raw_pphash::tuple_t *raw_pphash::find_tuple(const void *key, size_t hash, bin_t *bin)
        {
            if (bin == NULL)
                return NULL;

            if (key != NULL)
            {
                for (tuple_t *curr = bin->data; curr != NULL; curr = curr->next)
                {
                    if ((curr->hash == hash) && (compare(key, curr->key, ksize) == 0))
                        return curr;
                }
            }
            else
            {
                for (tuple_t *curr = bin->data; curr != NULL; curr = curr->next)
                {
                    if (curr->key == NULL)
                        return curr;
                }
            }
            return NULL;
        }

        raw_pphash::tuple_t *raw_pphash::remove_tuple(const void *key, size_t hash, bin_t *bin)
        {
            if (bin == NULL)
                return NULL;

            if (key != NULL)
            {
                for (tuple_t **pcurr = &bin->data; *pcurr != NULL; )
                {
                    tuple_t *curr = *pcurr;
                    if ((curr->hash == hash) && (compare(key, curr->key, ksize) == 0))
                    {
                        *pcurr      = curr->next;
                        curr->next  = NULL;
                        --bin->size;
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
                    if (curr->key == NULL)
                    {
                        *pcurr      = curr->next;
                        curr->next  = NULL;
                        --bin->size;
                        return curr;
                    }
                    pcurr   = &curr->next;
                }
            }
            return NULL;
        }

        raw_pphash::tuple_t *raw_pphash::create_tuple(void *key, size_t hash)
        {
            tuple_t *tuple  = reinterpret_cast<tuple_t *>(::malloc(sizeof(tuple_t)));
            if (tuple == NULL)
                return NULL;

            if ((++size) >= cap)
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

            tuple->hash     = hash;
            tuple->key      = key;
            tuple->next     = bin->data;
            bin->data       = tuple;

            return tuple;
        }

        bool raw_pphash::grow()
        {
            // Twice increase the capacity of hash
            size_t ncap     = (cap == 0) ? 0x10 : (cap << 1);
            bin_t *xbin     = reinterpret_cast<bin_t *>(::realloc(bins, ncap * sizeof(bin_t)));
            if (xbin == NULL)
                return false; // Very bad things?

            // Now we need to split data
            size_t mask     = (ncap - 1) ^ (cap - 1); // mask indicates the bit which has been set
            bins            = xbin;
            bin_t *ybin     = &xbin[cap];

            for (size_t i=0; i<cap; ++i, ++xbin, ++ybin)
            {
                // There is no data in ybin by default
                ybin->size      = 0;

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

        void raw_pphash::flush()
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

        void raw_pphash::clear()
        {
            // Just reset the size value for each bin
            if (bins != NULL)
            {
                for (size_t i=0; i<cap; ++i)
                    destroy_bin(&bins[i]);
            }

            size    = 0;
        }

        void raw_pphash::swap(raw_pphash *src)
        {
            raw_pphash tmp  = *this;
            *this           = *src;
            *src            = tmp;
        }

        void *raw_pphash::get(const void *key, void *dfl)
        {
            size_t h        = (key != NULL) ? hash(key, ksize) : 0;
            tuple_t *tuple  = find_tuple(key, h, &bins[h & (cap - 1)]);
            return (tuple != NULL) ? tuple->value : dfl;
        }

        void **raw_pphash::wget(const void *key)
        {
            size_t h        = (key != NULL) ? hash(key, ksize) : 0;
            tuple_t *tuple  = find_tuple(key, h, &bins[h & (cap - 1)]);
            return (tuple != NULL) ? &tuple->value : NULL;
        }

        void **raw_pphash::put(void *key, void *value, void **ok, void **ov)
        {
            size_t h        = (key != NULL) ? hash(key, ksize) : 0;

            // Find tuple
            tuple_t *tuple  = find_tuple(key, h, &bins[h & (cap - 1)]);
            if (tuple != NULL)
            {
                if (ok != NULL)
                    *ok         = tuple->key;
                if (ov != NULL)
                    *ov         = tuple->value;
                tuple->key      = key;
                tuple->value    = value;
                return &tuple->value;
            }

            // Not found, allocate new tuple
            tuple           = create_tuple(key, h);
            if (tuple == NULL)
                return NULL;

            tuple->value    = value;

            if (ok != NULL)
                *ok         = NULL;
            if (ov != NULL)
                *ov         = NULL;

            return &tuple->value;
        }

        void **raw_pphash::create(void *key, void *value)
        {
            size_t h        = (key != NULL) ? hash(key, ksize) : 0;

            // Find tuple
            tuple_t *tuple  = find_tuple(key, h, &bins[h & (cap - 1)]);
            if (tuple != NULL)
                return NULL;

            // Create new tuple
            tuple           = create_tuple(key, h);
            if (tuple == NULL)
                return NULL;

            tuple->value    = value;

            return &tuple->value;
        }

        void **raw_pphash::replace(void *key, void *value, void **ok, void **ov)
        {
            size_t h        = (key != NULL) ? hash(key, ksize) : 0;

            // Find tuple
            tuple_t *tuple  = find_tuple(key, h, &bins[h & (cap - 1)]);
            if (tuple == NULL)
                return NULL;

            if (ok != NULL)
                *ok         = tuple->key;
            if (ov != NULL)
                *ov         = tuple->value;
            tuple->key      = key;
            tuple->value    = value;
            return &tuple->value;
        }

        bool raw_pphash::remove(const void *key, void **ok, void **ov)
        {
            size_t h        = (key != NULL) ? hash(key, ksize) : 0;

            // Find tuple
            tuple_t *tuple  = remove_tuple(key, h, &bins[h & (cap - 1)]);
            if (tuple == NULL)
                return false;

            if (ok != NULL)
                *ok         = tuple->key;
            if (ov != NULL)
                *ov         = tuple->value;

            ::free(tuple);
            return true;
        }

        bool raw_pphash::keys(raw_parray *k)
        {
            raw_parray kt;

            // Initialize collection
            kt.init();
            if (!kt.grow(size))
                return false;

            // Make a snapshot
            for (size_t i=0; i<cap; ++i)
                for (tuple_t *t = bins[i].data; t != NULL; t = t->next)
                {
                    if (!kt.append(t->key))
                    {
                        kt.flush();
                        return false;
                    }
                }

            // Return collection data
            kt.swap(k);
            kt.flush();

            return true;
        }

        bool raw_pphash::values(raw_parray *v)
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

        bool raw_pphash::items(raw_parray *k, raw_parray *v)
        {
            raw_parray kt, vt;

            // Initialize collections
            kt.init();
            vt.init();

            if (!kt.grow(size))
                return false;
            if (!vt.grow(size))
            {
                kt.flush();
                return false;
            }

            // Make a snapshot
            for (size_t i=0; i<cap; ++i)
                for (tuple_t *t = bins[i].data; t != NULL; ++t)
                {
                    if ((!kt.append(t->key)) ||
                        (!vt.append(t->value)))
                    {
                        kt.flush();
                        vt.flush();
                        return false;
                    }
                }

            // Return collection data
            kt.swap(k);
            vt.swap(v);

            kt.flush();
            vt.flush();

            return true;
        }
    }
}


