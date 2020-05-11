/*
 * pphash.h
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_PPHASH_H_
#define LSP_PLUG_IN_LLTL_PPHASH_H_

#include <lsp-plug.in/lltl/version.h>
#include <lsp-plug.in/lltl/types.h>
#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace lltl
    {
        struct raw_pphash
        {
            public:
                typedef struct tuple_t
                {
                    size_t      hash;       // Hash code
                    void       *key;        // Key
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
                size_t          ksize;      // Size of key object
                hash_func_t     hash;       // Hash function
                compare_func_t  compare;    // Comparison function

            protected:
                void            destroy_bin(bin_t *bin);
                bool            grow();
                tuple_t        *find_tuple(const void *key, size_t hash, bin_t *bin);
                tuple_t        *remove_tuple(const void *key, size_t hash, bin_t *bin);
                tuple_t        *create_tuple(void *key, size_t hash);

            public:
                void            flush();
                void            clear();
                void            swap(raw_pphash *src);
                void           *get(const void *key, void *dfl);
                void          **wget(const void *key);
                void          **put(void *key, void *value, void **ok, void **ov);
                void          **replace(void *key, void *value, void **ok, void **ov);
                void          **create(void *key, void *value);
                bool            remove(const void *key, void **ok, void **ov);
                bool            keys(raw_parray *k);
                bool            values(raw_parray *v);
                bool            items(raw_parray *k, raw_parray *v);
        };


        template <class K, class V>
            class pphash
            {
                private:
                    pphash(const pphash<K, V> &src);                                // Disable copying
                    pphash<K, V> & operator = (const pphash<K, V> & src);           // Disable copying

                private:
                    mutable raw_pphash    v;

                    inline static V *vcast(void *ptr)       { return static_cast<V *>(ptr);         }
                    inline static V **pvcast(void *ptr)     { return reinterpret_cast<V **>(ptr);   }
                    inline static K **pkcast(void *ptr)     { return reinterpret_cast<K **>(ptr);   }
                    inline static void **pvcast(V **ptr)    { return reinterpret_cast<void **>(ptr);    }
                    inline static void **pkcast(K **ptr)    { return reinterpret_cast<void **>(ptr);    }

                public:
                    explicit inline pphash()
                    {
                        hash_iface<K> iface;

                        v.size          = 0;
                        v.cap           = 0;
                        v.bins          = NULL;
                        v.ksize         = sizeof(K);
                        v.hash          = iface.hash;
                        v.compare       = iface.compare;
                    }

                    ~pphash()                                               { v.flush();                                                    }

                public:
                    inline size_t       size() const                        { return v.size;                                                }

                public:
                    void clear()                                            { v.clear();                                                    }
                    inline void flush()                                     { v.flush();                                                    }
                    inline void swap(pphash<K, V> &src)                     { v.swap(&src);                                                 }
                    inline void swap(pphash<K, V> *src)                     { v.swap(src);                                                  }

                public:
                    /**
                     * Check that value associated with key exists
                     * @param key key
                     * @return true if value exists
                     */
                    inline bool exists(const K *key) const                  { return v.wget(key) != NULL;                                   }

                    /**
                     * Get value by key
                     * @param key key to use
                     * @return associated value or NULL if not exists
                     */
                    inline V *get(const K *key) const                       { return vcast(v.get(key, NULL));                               }

                    /**
                     * Get value by key or return default value if the value in hash was not found
                     * @param key key to use
                     * @param dfl default value to return if there is no such key in the hash
                     * @return the associated value
                     */
                    inline V *dget(const K *key, void *dfl) const           { return vcast(v.get(key, dfl));                                }

                    /**
                     * Get value for writing
                     * @param key the key to lookup the value
                     * @return pointer to the associated value that can be overwritten
                     */
                    inline V **wget(const K *key)                           { return pvcast(v.wget(key));                                   }

                public:
                    /**
                     * Put the value to the hash
                     * @param key key to use
                     * @param value value to put
                     * @param ok key removed from hash
                     * @param ov value removed from hash
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **put(K *key, V *value, K **ok, V **ov)        { return pvcast(v.put(key, value, pkcast(ok), pvcast(ov)));     }

                    /**
                     * Put the value to the hash
                     * @param key key to use
                     * @param ok key removed from hash
                     * @param ov value removed from hash
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **put(K *key, K **ok, V **ov)                  { return pvcast(v.put(key, NULL, pkcast(ok), pvcast(ov)));      }

                    /**
                     * Create the entry, do nothing if there is already existing entry with such key
                     * @param key key to use
                     * @param value value to use
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **create(K *key, V *value)                     { return pvcast(v.create(key, value));                          }

                    /**
                     * Create the entry, do nothing if there is already existing entry with such key
                     * @param key key to use
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **create(K *key)                               { return pvcast(v.create(key, NULL));                           }

                    /**
                     * Replace the entry ONLY if it exists
                     * @param key key to use
                     * @param value value to use
                     * @param ok key removed from hash
                     * @param ov value removed from hash
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **replace(K *key, V *value, K **ok, V **ov)    { return pvcast(v.replace(key, value, pkcast(ok), pvcast(ov))); }

                    /**
                     * Replace the entry ONLY if it exists
                     * @param key key to use
                     * @param ok old key removed from hash
                     * @param ov old value removed from hash
                     * @return pointer to write data or NULL if no allocation possible
                     */
                    inline V **replace(K *key, K **ok, V **ov)              { return pvcast(v.replace(key, NULL, pkcast(ok), pvcast(ov)));  }

                    /**
                     * Remove the associated key
                     * @param key the key to use for seacrh
                     * @param ok key removed from hash
                     * @param ov value removed from hash
                     * @return true if the data has been removed
                     */
                    inline bool remove(const K *key, K **ok, V **ov)        { return pvcast(v.remove(key, pkcast(ok), pvcast(ov)));         }

                public:
                    /**
                     * Store all keys to destination array
                     * @param k array to store keys
                     * @return true if all keys have been successfully stored
                     */
                    inline bool keys(parray<K> *k)                          { return v.keys(k->v);                                          }

                    /**
                     * Store all values to destination array
                     * @param v array to store values
                     * @return true if all keys have been successfully stored
                     */
                    inline bool values(parray<V> *v)                        { return v.values(v->v);                                        }

                    /**
                     * Store all items to destination array
                     * @param k array to store keys
                     * @param v array to store values
                     * @return true if all keys have been successfully stored
                     */
                    inline bool items(parray<K> *k, parray<V> *v)           { return v.items(k->v, v->v);                                   }
            };
    }
}



#endif /* LSP_PLUG_IN_LLTL_PPHASH_H_ */
