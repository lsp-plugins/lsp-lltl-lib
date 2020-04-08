/*
 * cvector.h
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_PARRAY_H_
#define LSP_PLUG_IN_LLTL_PARRAY_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include <lsp-plug.in/lltl/version.h>

namespace lsp
{
    namespace lltl
    {
        /**
         * Raw pointer array implementation with fixed set of routines
         */
        struct raw_parray
        {
            public:
                size_t      nItems;
                void      **vItems;
                size_t      nCapacity;

            public:
                void        init();
                bool        grow(size_t capacity);
                bool        truncate(size_t capacity);
                void        flush();

                void        swap(raw_parray *src);
                bool        xswap(size_t i1, size_t i2);
                ssize_t     index_of(const void *ptr);

                void      **slice(size_t idx, size_t size);
                void      **get_n(size_t idx, size_t size, void **dst);

                void      **set(size_t n, void **src);
                void      **append(size_t n);
                void      **append(void *ptr);
                void      **append(size_t n, void **src);
                void      **insert(size_t index, size_t n);
                void      **insert(size_t index, void *ptr);
                void      **insert(size_t index, size_t n, void **src);

                void       *pop();
                void      **pop(void **dst);
                void      **pop(size_t n);
                void      **pop(size_t n, void **dst);
                void      **pop(size_t n, raw_parray *cs);

                void       *premove(const void *ptr);
                bool        premove(const void *ptr, size_t n);
                void      **premove(const void *ptr, size_t n, void **dst);
                void      **premove(const void *ptr, size_t n, raw_parray *cs);

                void       *iremove(size_t idx);
                bool        iremove(size_t idx, size_t n);
                void      **iremove(size_t idx, size_t n, void **dst);
                void      **iremove(size_t idx, size_t n, raw_parray *cs);
        };

        /**
         * Data array template
         */
        template <class T>
            class parray
            {
                private:
                    parray(const parray<T> &src);                                   // Disable copying
                    parray<T> & operator = (const parray<T> & src);                 // Disable copying

                private:
                    mutable raw_parray    v;

                    inline static T *cast(void *ptr)                                { return static_cast<T *>(ptr);             }
                    inline static T **pcast(void **ptr)                             { return reinterpret_cast<T **>(ptr);       }
                    inline static void **vcast(T **ptr)                             { return reinterpret_cast<void **>(ptr);    }
                    inline static const T *ccast(void *ptr)                         { return static_cast<const T *>(ptr);       }

                public:
                    explicit inline parray()
                    {
                        v.nItems      = 0;
                        v.vItems      = NULL;
                        v.nCapacity   = 0;
                    }

                    ~parray() { v.flush(); };

                public:
                    // Size and capacity
                    size_t size() const                                             { return v.nItems;                      }
                    size_t capacity() const                                         { return v.nCapacity;                   }

                public:
                    // Whole collection manipulations
                    inline void clear()                                             { v.nItems  = 0;                        }
                    inline void flush()                                             { v.flush();                            }
                    inline void truncate()                                          { v.flush();                            }
                    inline void truncate(size_t size)                               { v.truncate(size);                     }
                    inline bool reserve(size_t capacity)                            { return v.grow(capacity);              }
                    inline void swap(parray<T> &src)                                { v.swap(&src.v);                       }
                    inline void swap(parray<T> *src)                                { v.swap(&src->v);                      }

                public:
                    // Accessing elements (non-const)
                    inline bool test(size_t idx) const                              { return idx < v.nItems;                }
                    inline T *get(size_t idx)                                       { return (idx < v.nItems) ? cast(v.vItems[idx]) : NULL;         }
                    inline T **get_n(size_t idx, size_t n, T **x)                   { return pcast(v.get_n(idx, n, x));     }
                    inline T **pget(size_t idx)                                     { return (idx < v.nItems) ? pcast(&v.vItems[idx]) : NULL;       }
                    inline T *uget(size_t idx)                                      { return cast(v.vItems[idx]);           }
                    inline T *upget(size_t idx)                                     { return pcast(&v.vItems[idx]);         }
                    inline T *first()                                               { return (v.nItems > 0) ? cast(v.vItems[0]) : NULL; }
                    inline T *last()                                                { return (v.nItems > 0) ? cast(v.vItems[v.nItems - 1]) : NULL;  }
                    inline T **array()                                              { return pcast(v.vItems);               }
                    inline T **slice(size_t idx, size_t size)                       { return pcast(v.slice(idx, size));     }
                    inline ssize_t index_of(const T *p) const                       { return v.index_of(p);                 }

                public:
                    // Accessing elements (const)
                    inline const T *get(size_t idx) const                           { return (idx < v.nItems) ? cast(v.vItems[idx]) : NULL; }
                    inline T **get_n(size_t idx, size_t n, T **x) const             { return pcast(v.get_n(idx, n, x));  }
                    inline const T *uget(size_t idx) const                          { return cast(v.vItems[idx]);       }
                    inline const T *first() const                                   { return (v.nItems > 0) ? cast(v.vItems[0]) : NULL; }
                    inline const T *last() const                                    { return (v.nItems > 0) ? cast(v.vItems[v.nItems - 1]) : NULL;  }
                    inline const T *array() const                                   { return ccast(v.vItems); }
                    inline const T **slice(size_t idx, size_t size) const           { return pcast(v.slice(idx, size)); }

                public:
                    // Single modifications
                    inline T **append()                                             { return pcast(v.append(1));            }
                    inline T **add()                                                { return pcast(v.append(1));            }
                    inline T **push()                                               { return pcast(v.append(1));            }
                    inline T **unshift()                                            { return pcast(v.insert(0, 1));         }
                    inline T **prepend()                                            { return pcast(v.insert(0, 1));         }
                    inline T **insert(size_t idx)                                   { return pcast(v.insert(idx, 1));       }

                    inline T *pop()                                                 { return cast(v.pop());                 }
                    inline T *shift()                                               { return cast(v.iremove(0));            }
                    inline T *remove(size_t idx)                                    { return cast(v.iremove(idx));          }
                    inline T *premove(const T *ptr)                                 { return cast(v.premove(ptr));          }

                    inline bool xswap(size_t i1, size_t i2)                         { return v.xswap(i1, i2);               }
                    inline void uswap(size_t i1, size_t i2)
                    {
                        void *tmp       = v.vItems[i1];
                        v.vItems[i1]    = v.vItems[i2];
                        v.vItems[i2]    = tmp;
                    }

                public:
                    // Single modifications with data copying (pointer argument)
                    inline T **append(T *x)                                         { return pcast(v.append(x));            }
                    inline T **add(T *x)                                            { return pcast(v.append(x));            }
                    inline T **push(T *x)                                           { return pcast(v.append(x));            }
                    inline T **unshift(T *x)                                        { return pcast(v.insert(0, x));         }
                    inline T **prepend(T *x)                                        { return pcast(v.insert(0, x));         }
                    inline T **insert(size_t idx, T *x)                             { return pcast(v.insert(idx, x));       }

                    inline T **pop(T **x)                                           { return pcast(v.pop(vcast(x)));               }
                    inline T **shift(T **x)                                         { return pcast(v.iremove(0, 1, vcast(x)));     }
                    inline T **remove(size_t idx, T **x)                            { return pcast(v.iremove(idx, 1, vcast(x)));   }
                    inline T **premove(const T *ptr, T **x)                         { return pcast(v.premove(ptr, 1, vcast(x)));   }

                public:
                    // Multiple modifications
                    inline T **append_n(size_t n)                                   { return pcast(v.append(n));            }
                    inline T **add_n(size_t n)                                      { return pcast(v.append(n));            }
                    inline T **push_n(size_t n)                                     { return pcast(v.append(n));            }
                    inline T **unshift_n(size_t n)                                  { return pcast(v.insert(0, n));         }
                    inline T **prepend_n(size_t n)                                  { return pcast(v.insert(0, n));         }
                    inline T **insert_n(size_t idx, size_t n)                       { return pcast(v.insert(idx, n));       }

                    inline T **pop_n(size_t n)                                      { return pcast(v.pop(n));               }
                    inline bool shift_n(size_t n)                                   { return v.iremove(0, n);               }
                    inline bool remove_n(size_t idx, size_t n)                      { return v.iremove(idx, n);             }
                    inline bool premove_n(const T *ptr, size_t n)                   { return v.premove(ptr, n);             }

                public:
                    // Multiple modifications with data copying
                    inline T **set_n(size_t n, T **x)                               { return pcast(v.set(n, x));            }
                    inline T **append_n(size_t n, T **x)                            { return pcast(v.append(n, x));         }
                    inline T **add_n(size_t n, T **x)                               { return pcast(v.append(n, x));         }
                    inline T **push_n(size_t n, T **x)                              { return pcast(v.append(n, x));         }
                    inline T **unshift_n(size_t n, T **x)                           { return pcast(v.insert(0, n, x));      }
                    inline T **prepend_n(size_t n, T **x)                           { return pcast(v.insert(0, n, x));      }
                    inline T **insert_n(size_t idx, size_t n, T **x)                { return pcast(v.insert(idx, n, x));    }

                    inline T **pop_n(size_t n, T **x)                               { return pcast(v.pop(n, x));            }
                    inline T **shift_n(size_t n, T **x)                             { return pcast(v.iremove(0, n, x));     }
                    inline T **remove_n(size_t idx, size_t n, T **x)                { return pcast(v.iremove(idx, n, x));   }
                    inline T **premove_n(const T *ptr, size_t n, T **x)             { return pcast(v.premove(ptr, n, x));   }

                public:
                    // Collection-based modifications (pointer argument)
                    inline T **set(const parray<T> *x)                              { return ccast(v.set(x->v.nItems, x->v.vItems));             }
                    inline T **append(const parray<T> *x)                           { return ccast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T **add(const parray<T> *x)                              { return ccast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T **push(const parray<T> *x)                             { return ccast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T **unshift(const parray<T> *x)                          { return ccast(v.insert(0, x->v.nItems, x->v.vItems));       }
                    inline T **prepend(const parray<T> *x)                          { return ccast(v.insert(0, x->v.nItems, x->v.vItems));       }
                    inline T **insert(size_t idx, const parray<T> *x)               { return ccast(v.insert(idx, x->v.nItems, x->v.vItems));     }

                    inline T **pop(parray<T> *x)                                    { return ccast(v.pop(1, &x->v));                             }
                    inline T **shift(parray<T> *x)                                  { return ccast(v.iremove(0, 1, &x->v));                      }
                    inline T **remove(size_t idx, parray<T> *x)                     { return ccast(v.iremove(idx, 1, &x->v));                    }
                    inline T **premove(const T *ptr, parray<T> *x)                  { return ccast(v.premove(ptr, 1, &x->v));                    }

                    inline T **pop_n(size_t n, parray<T> *x)                        { return ccast(v.pop(n, &x->v));                             }
                    inline T **shift_n(size_t n, parray<T> *x)                      { return ccast(v.iremove(0, n, &x->v));                      }
                    inline T **remove_n(size_t idx, size_t n, parray<T> *x)         { return ccast(v.iremove(idx, n, &x->v));                    }
                    inline T **premove_n(const T *ptr, size_t n, parray<T> *x)      { return ccast(v.premove(ptr, n, &x->v));                    }

                public:
                    // Collection-based modifications (reference argument)
                    inline T **set(const parray<T> &x)                              { return set(&x);                                           }
                    inline T **append(const parray<T> &x)                           { return append(&x);                                        }
                    inline T **add(const parray<T> &x)                              { return add(&x);                                           }
                    inline T **push(const parray<T> &x)                             { return push(&x);                                          }
                    inline T **unshift(const parray<T> &x)                          { return prepend(&x);                                       }
                    inline T **prepend(const parray<T> &x)                          { return prepend(&x);                                       }
                    inline T **insert(size_t idx, const parray<T> &x)               { return insert(idx, &x);                                   }

                    inline T **pop(parray<T> &x)                                    { return pop(&x);                                           }
                    inline T **shift(parray<T> &x)                                  { return shift(&x);                                         }
                    inline T **remove(size_t idx, parray<T> &x)                     { return remove(idx, &x);                                   }
                    inline T **premove(const T *ptr, parray<T> &x)                  { return premove(ptr, &x);                                  }

                    inline T **pop_n(size_t n, parray<T> &x)                        { return pop_n(n, &x);                                      }
                    inline T **shift_n(size_t n, parray<T> &x)                      { return shift_n(n, &x);                                    }
                    inline T **remove_n(size_t idx, size_t n, parray<T> &x)         { return remove_n(idx, n, &x);                              }
                    inline T **premove_n(const T *ptr, size_t n, parray<T> &x)      { return premove_n(ptr, n, &x);                             }

                public:
                    // Operators
                    inline T *operator[](size_t idx)                                { return get(idx);                      }
                    inline const T *operator[](size_t idx) const                    { return get(idx);                      }
            };
    }
}

#endif /* LSP_PLUG_IN_LLTL_PARRAY_H_ */
