/*
 * cstorage.h
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_DARRAY_H_
#define LSP_PLUG_IN_LLTL_DARRAY_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

namespace lsp
{
    namespace lltl
    {
        /**
         * Raw data array implementation with fixed set of routines
         */
        struct raw_darray
        {
            public:
                uint8_t    *vItems;
                size_t      nCapacity;
                size_t      nItems;
                size_t      nSizeOf;

            public:
                ~raw_darray();

            public:
                void        init(size_t n_sizeof);
                bool        grow(size_t capacity);
                bool        truncate(size_t capacity);
                void        swap(raw_darray *src);
                bool        xswap(size_t i1, size_t i2);
                void        uswap(size_t i1, size_t i2);
                void        flush();
                ssize_t     index_of(const void *ptr);

                uint8_t    *slice(size_t idx, size_t size);
                uint8_t    *get_n(size_t idx, size_t size, void *dst);

                uint8_t    *set(size_t n, const void *src);
                uint8_t    *append(size_t n);
                uint8_t    *append(size_t n, const void *src);
                uint8_t    *insert(size_t index, size_t n);
                uint8_t    *insert(size_t index, size_t n, const void *src);

                uint8_t    *pop(size_t n);
                uint8_t    *pop(size_t n, void *dst);
                uint8_t    *pop(size_t n, raw_darray *cs);
                bool        premove(const void *ptr, size_t n);
                uint8_t    *premove(const void *ptr, size_t n, void *dst);
                uint8_t    *premove(const void *ptr, size_t n, raw_darray *cs);
                bool        iremove(size_t idx, size_t n);
                uint8_t    *iremove(size_t idx, size_t n, void *dst);
                uint8_t    *iremove(size_t idx, size_t n, raw_darray *cs);
        };

        /**
         * Data array template
         */
        template <class T>
            class darray
            {
                private:
                    darray(const darray<T> &src);                                   // Disable copying
                    darray<T> & operator = (const darray<T> & src);                 // Disable copying

                private:
                    mutable raw_darray    v;

                    inline static T *cast(void *ptr)                                { return static_cast<T *>(ptr);         }
                    inline static const T *ccast(const void *ptr)                   { return static_cast<const T *>(ptr);   }

                public:
                    explicit inline darray()                                        { v.init(sizeof(T));                }
                    ~darray() {};

                public:
                    // Size and capacity
                    size_t size() const                                             { return v.nItems;                  }
                    size_t capacity() const                                         { return v.nCapacity;               }

                public:
                    // Whole collection manipulations
                    inline void clear()                                             { v.nItems  = 0;                    }
                    inline void flush()                                             { v.flush();                        }
                    inline void truncate()                                          { v.flush();                        }
                    inline void truncate(size_t size)                               { v.truncate(size);                 }
                    inline void reserve(size_t capacity)                            { v.grow(capacity);                 }
                    inline void swap(darray<T> &src)                                { v.swap(&src.v);                   }
                    inline void swap(darray<T> *src)                                { v.swap(&src->v);                  }

                public:
                    // Accessing elements (non-const)
                    inline T *get(size_t idx)                                       { return (idx < v.nItems) ? cast(&v.vItems[idx * v.nSizeOf]) : NULL; }
                    inline T *get_n(size_t idx, size_t n, T *x)                     { return cast(v.get_n(idx, n, x));  }
                    inline T *uget(size_t idx)                                      { return cast(&v.vItems[idx * v.nSizeOf]);  }
                    inline T *first()                                               { return cast(v.vItems); }
                    inline T *last()                                                { return (v.nItems > 0) ? cast(&v.vItems[(v.nItems - 1) * v.nSizeOf]) : NULL;   }
                    inline T *array()                                               { return cast(v.vItems); }
                    inline T *slice(size_t idx, size_t size)                        { return cast(v.slice(idx, size));  }
                    inline ssize_t index_of(const T *p) const                       { return v.index_of(p);             }

                public:
                    // Accessing elements (const)
                    inline const T *get(size_t idx) const                           { return (idx < v.nItems) ? ccast(&v.vItems[idx * v.nSizeOf]) : NULL; }
                    inline T *get_n(size_t idx, size_t n, T *x) const               { return cast(v.get_n(idx, n, x));  }
                    inline const T *uget(size_t idx) const                          { return ccast(&v.vItems[idx * v.nSizeOf]); }
                    inline const T *first() const                                   { return ccast(v.vItems); }
                    inline const T *last() const                                    { return (v.nItems > 0) ? ccast(&v.vItems[(v.nItems - 1) * v.nSizeOf]) : NULL;  }
                    inline const T *array() const                                   { return ccast(v.vItems); }
                    inline const T *slice(size_t idx, size_t size) const            { return ccast(v.slice(idx, size));  }

                public:
                    // Single modifications
                    inline T *append()                                              { return cast(v.append(1));         }
                    inline T *add()                                                 { return cast(v.append(1));         }
                    inline T *push()                                                { return cast(v.append(1));         }
                    inline T *unshift()                                             { return cast(v.insert(0, 1));      }
                    inline T *prepend()                                             { return cast(v.insert(0, 1));      }
                    inline T *insert(size_t idx)                                    { return cast(v.insert(idx, 1));    }

                    inline T *pop()                                                 { return cast(v.pop(1));            }
                    inline bool shift()                                             { return v.iremove(0, 1);           }
                    inline bool remove(size_t idx)                                  { return v.iremove(idx, 1);         }
                    inline bool premove(const T *ptr)                               { return v.premove(ptr, 1);         }

                    inline bool xswap(size_t i1, size_t i2)                         { return v.xswap(i1, i2);           }
                    inline void uswap(size_t i1, size_t i2)                         { v.uswap(i1, i2);                  }

                public:
                    // Single modifications with data copying (pointer argument)
                    inline T *append(const T *x)                                    { return cast(v.append(1, x));      }
                    inline T *add(const T *x)                                       { return cast(v.append(1, x));      }
                    inline T *push(const T *x)                                      { return cast(v.append(1, x));      }
                    inline T *unshift(const T *x)                                   { return cast(v.insert(0, 1, x));   }
                    inline T *prepend(const T *x)                                   { return cast(v.insert(0, 1, x));   }
                    inline T *insert(size_t idx, const T *x)                        { return cast(v.insert(idx, 1, x)); }

                    inline T *pop(T *x)                                             { return cast(v.pop(1, x));         }
                    inline T *shift(T *x)                                           { return cast(v.iremove(0, 1, x));  }
                    inline T *remove(size_t idx, T *x)                              { return cast(v.iremove(idx, 1, x));}
                    inline T *premove(const T *ptr, T *x)                           { return cast(v.premove(ptr, 1, x));}

                public:
                    // Single modifications with data copying (reference argument)
                    inline T *append(const T &x)                                    { return append(&x);                }
                    inline T *add(const T &x)                                       { return add(&x);                   }
                    inline T *push(const T &x)                                      { return push(&x);                  }
                    inline T *unshift(const T &x)                                   { return prepend(&x);               }
                    inline T *prepend(const T &x)                                   { return prepend(&x);               }
                    inline T *insert(size_t idx, const T &x)                        { return insert(idx, &x);           }

                    inline T *pop(T &x)                                             { return pop(&x);                   }
                    inline T *shift(T &x)                                           { return shift(&x);                 }
                    inline T *remove(size_t idx, T &x)                              { return remove(idx, &x);           }
                    inline T *premove(const T *ptr, T &x)                           { return premove(ptr, &x);          }

                public:
                    // Multiple modifications
                    inline T *append_n(size_t n)                                    { return cast(v.append(n));         }
                    inline T *add_n(size_t n)                                       { return cast(v.append(n));         }
                    inline T *push_n(size_t n)                                      { return cast(v.append(n));         }
                    inline T *unshift_n(size_t n)                                   { return cast(v.insert(0, n));      }
                    inline T *prepend_n(size_t n)                                   { return cast(v.insert(0, n));      }
                    inline T *insert_n(size_t idx, size_t n)                        { return cast(v.insert(idx, n));    }

                    inline T *pop_n(size_t n)                                       { return cast(v.pop(n));            }
                    inline bool shift_n(size_t n)                                   { return v.iremove(0, n);           }
                    inline bool remove_n(size_t idx, size_t n)                      { return v.iremove(idx, n);         }
                    inline bool premove_n(const T *ptr, size_t n)                   { return v.premove(ptr, n);         }

                public:
                    // Multiple modifications with data copying
                    inline T *set_n(size_t n, const T *x)                           { return cast(v.set(n, x));         }
                    inline T *append_n(size_t n, const T *x)                        { return cast(v.append(n, x));      }
                    inline T *add_n(size_t n, const T *x)                           { return cast(v.append(n, x));      }
                    inline T *push_n(size_t n, const T *x)                          { return cast(v.append(n, x));      }
                    inline T *unshift_n(size_t n, const T *x)                       { return cast(v.insert(0, n, x));   }
                    inline T *prepend_n(size_t n, const T *x)                       { return cast(v.insert(0, n, x));   }
                    inline T *insert_n(size_t idx, size_t n, const T *x)            { return cast(v.insert(idx, n, x)); }

                    inline T *pop_n(size_t n, T *x)                                 { return cast(v.pop(n, x));         }
                    inline T *shift_n(size_t n, T *x)                               { return cast(v.iremove(0, n, x));  }
                    inline T *remove_n(size_t idx, size_t n, T *x)                  { return cast(v.iremove(idx, n, x));}
                    inline T *premove_n(const T *ptr, size_t n, T *x)               { return cast(v.premove(ptr, n, x));}

                public:
                    // Collection-based modifications (pointer argument)
                    inline T *set(const darray<T> *x)                               { return cast(v.set(x->v.nItems, x->v.vItems));             }
                    inline T *append(const darray<T> *x)                            { return cast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T *add(const darray<T> *x)                               { return cast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T *push(const darray<T> *x)                              { return cast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T *unshift(const darray<T> *x)                           { return cast(v.insert(0, x->v.nItems, x->v.vItems));       }
                    inline T *prepend(const darray<T> *x)                           { return cast(v.insert(0, x->v.nItems, x->v.vItems));       }
                    inline T *insert(size_t idx, const darray<T> *x)                { return cast(v.insert(idx, x->v.nItems, x->v.vItems));     }

                    inline T *pop(darray<T> *x)                                     { return cast(v.pop(1, &x->v));                             }
                    inline T *shift(darray<T> *x)                                   { return cast(v.iremove(0, 1, &x->v));                      }
                    inline T *remove(size_t idx, darray<T> *x)                      { return cast(v.iremove(idx, 1, &x->v));                    }
                    inline T *premove(const T *ptr, darray<T> *x)                   { return cast(v.premove(ptr, 1, &x->v));                    }

                    inline T *pop_n(size_t n, darray<T> *x)                         { return cast(v.pop(n, &x->v));                             }
                    inline T *shift_n(size_t n, darray<T> *x)                       { return cast(v.iremove(0, n, &x->v));                      }
                    inline T *remove_n(size_t idx, size_t n, darray<T> *x)          { return cast(v.iremove(idx, n, &x->v));                    }
                    inline T *premove_n(const T *ptr, size_t n, darray<T> *x)       { return cast(v.premove(ptr, n, &x->v));                    }

                public:
                    // Collection-based modifications (reference argument)
                    inline T *set(const darray<T> &x)                               { return set(&x);                                           }
                    inline T *append(const darray<T> &x)                            { return append(&x);                                        }
                    inline T *add(const darray<T> &x)                               { return add(&x);                                           }
                    inline T *push(const darray<T> &x)                              { return push(&x);                                          }
                    inline T *unshift(const darray<T> &x)                           { return prepend(&x);                                       }
                    inline T *prepend(const darray<T> &x)                           { return prepend(&x);                                       }
                    inline T *insert(size_t idx, const darray<T> &x)                { return insert(idx, &x);                                   }

                    inline T *pop(darray<T> &x)                                     { return pop(&x);                                           }
                    inline T *shift(darray<T> &x)                                   { return shift(&x);                                         }
                    inline T *remove(size_t idx, darray<T> &x)                      { return remove(idx, &x);                                   }
                    inline T *premove(const T *ptr, darray<T> &x)                   { return premove(ptr, &x);                                  }

                    inline T *pop_n(size_t n, darray<T> &x)                         { return pop_n(n, &x);                                      }
                    inline T *shift_n(size_t n, darray<T> &x)                       { return shift_n(n, &x);                                    }
                    inline T *remove_n(size_t idx, size_t n, darray<T> &x)          { return remove_n(idx, n, &x);                              }
                    inline T *premove_n(const T *ptr, size_t n, darray<T> &x)       { return premove_n(ptr, n, &x);                             }

                public:
                    // Operators
                    inline T *operator[](size_t idx)                                { return get(idx);                  }
                    inline const T *operator[](size_t idx) const                    { return get(idx);                  }
            };
    }
}

#endif /* LSP_PLUG_IN_LLTL_DARRAY_H_ */
