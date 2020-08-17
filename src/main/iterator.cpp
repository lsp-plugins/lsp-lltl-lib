/*
 * iterator.cpp
 *
 *  Created on: 17 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/lltl/iterator.h>

namespace lsp
{
    namespace lltl
    {
        raw_iterator *raw_iterator::reference(raw_iterator *src)
        {
            if (src == NULL)
                return NULL;
            ++src->refs;
            return src;
        }

        raw_iterator *raw_iterator::dereference(raw_iterator *src)
        {
            if (src != NULL)
            {
                if ((--src->refs) <= 0)
                    ::free(src);
            }

            return NULL;
        }

        raw_iterator *raw_iterator::replace(raw_iterator *xold, raw_iterator *xnew)
        {
            if (xold == xnew)
                return xold;

            raw_iterator *ref = reference(xnew);
            dereference(xold);
            return ref;
        }
    }
}
