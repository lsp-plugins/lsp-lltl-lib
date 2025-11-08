/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 8 нояб. 2025 г.
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

#include <lsp-plug.in/lltl/hash_index.h>

namespace lsp
{
    namespace lltl
    {
        const iter_vtbl_t raw_hash_index::key_iterator_vtbl =
        {
            iter_move,
            iter_get_key,
            iter_compare,
            iter_compare,
            iter_count
        };

        const iter_vtbl_t raw_hash_index::value_iterator_vtbl =
        {
            iter_move,
            iter_get_value,
            iter_compare,
            iter_compare,
            iter_count
        };

        const iter_vtbl_t raw_hash_index::pair_iterator_vtbl =
        {
            iter_move,
            iter_get_pair,
            iter_compare,
            iter_compare,
            iter_count
        };

        bool raw_hash_index::grow()
        {
            bin_t *xbin, *ybin;
            size_t ncap, mask;

            // No previous allocations?
            if (cap == 0)
            {
                xbin            = reinterpret_cast<bin_t *>(::malloc(0x10 * sizeof(bin_t)));
                if (xbin == NULL)
                    return false; // Very bad things?

                cap             = 0x10;
                bins            = xbin;
                for (size_t i=0; i<cap; ++i, ++xbin)
                {
                    xbin->size      = 0;
                    xbin->head      = NULL;
                    xbin->tail      = NULL;
                }

                return true;
            }

            // Twice increase the capacity of hash
            ncap            = cap << 1;
            xbin            = reinterpret_cast<bin_t *>(::realloc(bins, ncap * sizeof(bin_t)));
            if (xbin == NULL)
                return false; // Very bad things?

            // Now we need to split data

            // TODO

            // Split success
            cap         = ncap;

            return true;
        }

        raw_hash_index::lookup_t raw_hash_index::find_node(const void *key, size_t hash)
        {
            if (bins == NULL)
                return lookup_t { NULL, 0 };
            bin_t *bin = &bins[hash & (cap - 1)];

            size_t size = bin->size;
            if (key != NULL)
            {
                for (node_t *curr = bin->head; curr != NULL; curr = curr->next)
                {
                    const size_t count  = lsp_min(size, raw_hash_node_size);
                    size -= raw_hash_node_size;

                    for (size_t i=0; i<count; ++i)
                    {
                        if ((curr->hash[i] == hash) && (cmp.compare(key, curr->v[i].key, ksize) == 0))
                            return lookup_t { curr, i };
                    }
                }
            }
            else
            {
                for (node_t *curr = bin->head; curr != NULL; curr = curr->next)
                {
                    const size_t count  = lsp_min(size, raw_hash_node_size);
                    size -= raw_hash_node_size;

                    for (size_t i=0; i<count; ++i)
                    {
                        if (curr->v[i].key == NULL)
                            return lookup_t { curr, i };
                    }
                }
            }
            return lookup_t { NULL, 0 };
        }

        raw_pair_t *raw_hash_index::create_item(const void *key, size_t hash)
        {
            // Need to grow?
            if (size >= cap*4)
            {
                if (!grow())
                    return NULL;
            }

            // Add item to the bin
            bin_t *bin      = &bins[hash & (cap - 1)];

            size_t index    = (bin->size + 1) % raw_hash_node_size;
            node_t *curr    = bin->tail;
            if (curr == NULL)
            {
                // Create new node
                node_t *node    = static_cast<node_t *>(malloc(sizeof(node_t)));
                if (node == NULL)
                    return NULL;

                // Link node to exising list
                node->next      = NULL;
                node->prev      = NULL;
                bin->head       = node;
                bin->tail       = node;
                curr            = node;
            }
            else if (index == 0)
            {

                // Create new node
                node_t *node    = static_cast<node_t *>(malloc(sizeof(node_t)));
                if (node == NULL)
                    return NULL;

                // Link node to exising list
                node->next      = NULL;
                node->prev      = curr;
                curr->next      = node;
                bin->tail       = node;
                curr            = node;
            }

            // Store values
            curr->hash[index]   = hash;
            curr->v[index].key  = const_cast<void *>(key);
            ++bin->size;
            ++size;

            return &curr->v[index];
        }

        void **raw_hash_index::create(const void *key, void *value)
        {
            size_t h        = (key != NULL) ? hash.hash(key, ksize) : 0;

            // Find node
            lookup_t pos    = find_node(key, h);
            if (pos.node != NULL)
                return NULL;

            // Create new element
            raw_pair_t *dst = create_item(key, h);
            if (dst== NULL)
                return NULL;

            dst->value      = value;

            return &dst->value;
        }

        bool raw_hash_index::remove(const void *key, void **ov)
        {
            size_t h        = (key != NULL) ? hash.hash(key, ksize) : 0;

            // Find node
            lookup_t pos    = find_node(key, h);
            node_t *node    = pos.node;
            if (node == NULL)
                return false;
            size_t index    = pos.index;

            // Move elements
            if (ov != NULL)
                *ov         = node->v[index].value;

            bin_t *bin      = &bins[h & (cap - 1)];
            node_t *src     = node->next;
            const size_t last_idx = (bin->size - 1) % raw_hash_node_size;
            if (src != NULL)
            {
                // Removal not in the last node
                do
                {
                    const size_t src_index  = (src->next != NULL) ? raw_hash_node_size - 1 : last_idx;
                    node->hash[index]       = src->hash[src_index];
                    node->v[index]          = src->v[src_index];

                    index                   = src_index;
                    node                    = src;
                } while (node->next);
            }
            else
            {
                // Removal in the last node
                const size_t src_index    = last_idx;
                if (index < src_index)
                {
                    node->hash[index]   = node->hash[src_index];
                    node->v[index]      = node->v[src_index];
                }
            }

            // Remove last node from bin if needed
            if (last_idx == 0)
            {
                node            = bin->tail;
                bin->tail       = node->prev;
                if (bin->head == node)
                    bin->head       = NULL;
                if (bin->tail != NULL)
                    bin->tail->next = NULL;
            }

            --bin->size;
            --size;

            return true;
        }

        void **raw_hash_index::replace(const void *key, void *value, void **ov)
        {
            size_t h        = (key != NULL) ? hash.hash(key, ksize) : 0;

            // Find node
            lookup_t pos    = find_node(key, h);
            if (pos.node != NULL)
                return NULL;

            // Replace value if it exists
            raw_pair_t *p   = &pos.node->v[pos.index];
            if (ov != NULL)
                *ov         = p->value;

            p->value        = value;
            return &p->value;
        }

        void **raw_hash_index::put(const void *key, void *value, void **ov)
        {
            size_t h        = (key != NULL) ? hash.hash(key, ksize) : 0;

            // Find node
            lookup_t pos    = find_node(key, h);
            if (pos.node != NULL)
            {
                raw_pair_t *p   = &pos.node->v[pos.index];
                if (ov != NULL)
                    *ov         = p->value;

                p->value        = value;
                return &p->value;
            }

            // Create new element
            raw_pair_t *dst = create_item(key, h);
            if (dst== NULL)
                return NULL;

            dst->value      = value;
            if (ov != NULL)
                *ov         = NULL;

            return &dst->value;
        }

        void **raw_hash_index::wbget(const void *key)
        {
            size_t h        = (key != NULL) ? hash.hash(key, ksize) : 0;
            lookup_t pos    = find_node(key, h);
            return (pos.node != NULL) ? &pos.node->v[pos.index].value : NULL;
        }

        void *raw_hash_index::get(const void *key, void *dfl)
        {
            size_t h        = (key != NULL) ? hash.hash(key, ksize) : 0;
            lookup_t pos    = find_node(key, h);
            return (pos.node != NULL) ? pos.node->v[pos.index].value : NULL;
        }

        void *raw_hash_index::key(const void *key, void *dfl)
        {
            size_t h        = (key != NULL) ? hash.hash(key, ksize) : 0;
            lookup_t pos    = find_node(key, h);
            return (pos.node != NULL) ? pos.node->v[pos.index].key: NULL;
        }

        bool raw_hash_index::keys(raw_parray *k) const
        {
            raw_parray kt;

            // Initialize collection
            kt.init();
            if (!kt.grow(size))
                return false;

            // Make a snapshot
            for (size_t i=0; i<cap; ++i)
            {
                const bin_t *bin = &bins[i];
                size_t size = bin->size;

                for (const node_t *node = bin->head; node != NULL; node = node->next)
                {
                    const size_t count  = lsp_min(size, raw_hash_node_size);
                    size               -= raw_hash_node_size;

                    for (size_t i=0; i<count; ++i)
                    {
                        if (!kt.append(node->v[i].key))
                            return false;
                    }
                }
            }

            // Return collection data
            kt.swap(k);

            return true;
        }

        bool raw_hash_index::values(raw_parray *v) const
        {
            raw_parray kv;

            // Initialize collection
            kv.init();
            if (!kv.grow(size))
                return false;

            // Make a snapshot
            for (size_t i=0; i<cap; ++i)
            {
                const bin_t *bin = &bins[i];
                size_t size = bin->size;

                for (const node_t *node = bin->head; node != NULL; node = node->next)
                {
                    const size_t count  = lsp_min(size, raw_hash_node_size);
                    size               -= raw_hash_node_size;

                    for (size_t i=0; i<count; ++i)
                    {
                        if (!kv.append(node->v[i].value))
                            return false;
                    }
                }
            }

            // Return collection data
            kv.swap(v);

            return true;
        }

        bool raw_hash_index::items(raw_parray *k, raw_parray *v) const
        {
            raw_parray kt, vt;

            // Initialize collections
            kt.init();
            vt.init();

            if (!kt.grow(size))
                return false;
            if (!vt.grow(size))
                return false;

            // Make a snapshot
            for (size_t i=0; i<cap; ++i)
            {
                const bin_t *bin = &bins[i];
                size_t size = bin->size;

                for (const node_t *node = bin->head; node != NULL; node = node->next)
                {
                    const size_t count  = lsp_min(size, raw_hash_node_size);
                    size               -= raw_hash_node_size;

                    for (size_t i=0; i<count; ++i)
                    {
                        if (!kt.append(node->v[i].key))
                            return false;
                        if (!vt.append(node->v[i].value))
                            return false;
                    }
                }
            }

            // Return collection data
            kt.swap(k);
            vt.swap(v);

            return true;
        }

        void raw_hash_index::flush()
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

        void raw_hash_index::clear()
        {
            // Just reset the size value for each bin
            if (bins != NULL)
            {
                for (size_t i=0; i<cap; ++i)
                    destroy_bin(&bins[i]);
            }

            size    = 0;
        }

        void raw_hash_index::destroy_bin(bin_t *bin)
        {
            for (node_t *curr = bin->head; curr != NULL; )
            {
                node_t *next    = curr->next;
                ::free(curr);
                curr            = next;
            }
            bin->size   = 0;
            bin->head   = NULL;
            bin->tail   = NULL;
        }

        void raw_hash_index::swap(raw_hash_index *src)
        {
            raw_hash_index tmp  = *this;
            *this               = *src;
            *src                = tmp;
        }

        void raw_hash_index::iter_move(raw_iterator *i, ssize_t n)
        {
            // TODO
        }

        void *raw_hash_index::iter_get_key(raw_iterator *i)
        {
            node_t *node = static_cast<node_t *>(i->item);
            return node->v[i->offset].key;
        }

        void *raw_hash_index::iter_get_value(raw_iterator *i)
        {
            node_t *node = static_cast<node_t *>(i->item);
            return node->v[i->offset].value;
        }

        void *raw_hash_index::iter_get_pair(raw_iterator *i)
        {
            node_t *node = static_cast<node_t *>(i->item);
            return &node->v[i->offset];
        }

        ssize_t raw_hash_index::iter_compare(const raw_iterator *a, const raw_iterator *b)
        {
            return a->index - b->index;
        }

        size_t raw_hash_index::iter_count(const raw_iterator *i)
        {
            raw_hash_index *self  = static_cast<raw_hash_index *>(i->container);
            return self->size;
        }

    } /* namespace lltl */
} /* namespace lsp */


