/*
 * phashset.h
 *
 *  Created on: 31 июл. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_PHASHSET_H_
#define LSP_PLUG_IN_LLTL_PHASHSET_H_

#include <lsp-plug.in/lltl/version.h>
#include <lsp-plug.in/lltl/types.h>
#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace lltl
    {
        struct raw_phashset
        {
            public:
                typedef struct tuple_t
                {
                    size_t      hash;       // Hash code
                    void       *value;      // Value
                    tuple_t    *next;       // Next tuple
                } tuple_t;

                typedef struct bin_t
                {
                    size_t      size;       // Number of used tuples in storage
                    tuple_t    *data;       // Tuples
                } bin_t;

            public:
                size_t          size;       // Overall size of the hash
                size_t          cap;        // Capacity in bins
                bin_t          *bins;       // Overall array of bins
                size_t          vsize;      // Size of value object
                hash_iface      hash;       // Hash interface
                compare_iface   cmp;        // Copy interface

            protected:
                void            destroy_bin(bin_t *bin);
                bool            grow();
                tuple_t        *find_tuple(const void *value, size_t hash);
                tuple_t        *remove_tuple(const void *value, size_t hash);
                tuple_t        *create_tuple(size_t hash);

            public:
                void            flush();
                void            clear();
                void            swap(raw_phashset *src);
                void           *get(const void *value, void *dfl);
                void          **wbget(const void *value);
                void          **put(void *value, void **ret);
                void          **create(void *value);
                bool            toggle(void *value);
                bool            remove(const void *value, void **ret);
                bool            values(raw_parray *v);
                void           *any();
        };

        /**
         * Raw pointer implementation of hash set.
         * There are no automatic memory management for values, so the caller is required to
         * properly collect the garbage.
         */
        template <class V>
            class phashset
            {
                private:
                    phashset(const phashset<V> &src);                               // Disable copying
                    phashset<V> & operator = (const phashset<V> & src);             // Disable copying

                private:
                    mutable raw_phashset    v;

                    inline static V *vcast(void *ptr)       { return static_cast<V *>(ptr);             }
                    inline static V **pvcast(void *ptr)     { return reinterpret_cast<V **>(ptr);       }
                    inline static void **pvcast(V **ptr)    { return reinterpret_cast<void **>(ptr);    }

                public:
                    explicit inline phashset()
                    {
                        hash_spec<V>        hash;
                        compare_spec<V>     cmp;

                        v.size          = 0;
                        v.cap           = 0;
                        v.bins          = NULL;
                        v.vsize         = sizeof(V);
                        v.hash          = hash;
                        v.cmp           = cmp;
                    }

                    ~phashset()                                             { v.flush();                                                    }

                public:
                    /**
                     * Get number of stored elements in collection
                     * @return number of stored elements in collection
                     */
                    inline size_t       size() const                        { return v.size;                                                }

                    /**
                     * Get number of bins in collection
                     * @return number of bins in collection
                     */
                    inline size_t       capacity() const                    { return v.cap;                                                 }

                    /**
                     * Check whether collection is empty
                     * @return true if collection does not contain any element
                     */
                    inline bool         is_empty() const                    { return v.size <= 0;                                           }

                public:
                    /**
                     * Clear all bin data.
                     * Automatically destroys keys.
                     * Caller is responsible for destroying values.
                     */
                    void clear()                                            { v.clear();                                                    }

                    /**
                     * Clear and destroy all bins.
                     * Automatically destroys keys.
                     * Caller is responsible for destroying values.
                     */
                    inline void flush()                                     { v.flush();                                                    }

                    /**
                     * Performs internal data exchange with another collection of the same type
                     * @param src collection to perform exchange
                     */
                    inline void swap(phashset<V> &src)                      { v.swap(&src.v);                                               }

                    /**
                     * Performs internal data exchange with another collection of the same type
                     * @param src collection to perform exchange
                     */
                    inline void swap(phashset<V> *src)                      { v.swap(&src->v);                                              }

                public:
                    /**
                     * Check that value associated with key exists (same to contains)
                     * @param value the desired value
                     * @return true if value exists
                     */
                    inline bool exists(const V *value) const                { return v.wbget(value) != NULL;                                }

                    /**
                     * Check that value associated with key exists (same to exists)
                     * @param value the desired value
                     * @return true if value exists
                     */
                    inline bool contains(const V *value) const              { return v.wbget(value) != NULL;                                }

                    /**
                     * Get value by key
                     * @param value the desired value
                     * @return associated value or NULL if not exists
                     */
                    inline V *get(const V *value) const                     { return vcast(v.get(value, NULL));                             }

                    /**
                     * Get value by key or return default value if the value was not found
                     * @param value the desired value
                     * @param dfl default value to return if there is no such value in the set
                     * @return the associated value
                     */
                    inline V *dget(const V *key, V *dfl) const              { return vcast(v.get(key, dfl));                                }

                    /**
                     * Remove the item from set if it is present in the set, add the item if not
                     * @param value value to toggle
                     * @return true on success
                     */
                    inline bool toggle(V *value) const                      { return v.toggle(value);                                       }

                    /**
                     * Get any single value present in the collection
                     * @return any value present in the collection or NULL if none
                     */
                    inline V *any() const                                   { return vcast(v.any());                                        }

                public:
                    /**
                     * Put the value to the set
                     * @param value value to put
                     * @param ov value removed from hash
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **put(V *value, V **ov = NULL)                 { return pvcast(v.put(value, pvcast(ov)));                      }

                    /**
                     * Create the value, do nothing if there is already existing value
                     * @param value value to use
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **create(V *value)                             { return pvcast(v.create(value));                               }

                    /**
                     * Remove the associated key
                     * @param key the key to use for seacrh
                     * @param ok key removed from hash
                     * @param ov value removed from hash
                     * @return true if the data has been removed
                     */
                    inline bool remove(const V *value, V **ov = NULL)       { return v.remove(value, pvcast(ov));               }

                public:
                    /**
                     * Store all values to the destination array
                     * @param vv array to store values
                     * @return true if all keys have been successfully stored
                     */
                    inline bool values(parray<V> *vv)                        { return v.values(vv->raw());                      }
            };
    }
}



#endif /* LSP_PLUG_IN_LLTL_PHASHSET_H_ */
