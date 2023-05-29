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

#include <lsp-plug.in/lltl/iterator.h>

namespace lsp
{
    namespace lltl
    {
        void invalid_iter_move(raw_iterator *i, ssize_t n)
        {
        }

        void *invalid_iter_get(raw_iterator *i)
        {
            return NULL;
        }

        ssize_t invalid_iter_compare(const raw_iterator *a, const raw_iterator *b)
        {
            return ssize_t(b->container == NULL);
        }

        const iter_vtbl_t raw_iterator::invalid_vtbl =
        {
            invalid_iter_move,
            invalid_iter_get,
            invalid_iter_compare
        };

        const raw_iterator raw_iterator::invalid =
        {
            &invalid_vtbl,
            NULL,
            NULL,
            0
        };

    } /* namespace lltl */
} /* namespace lsp */
