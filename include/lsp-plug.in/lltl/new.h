/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 8 янв. 2023 г.
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

#ifndef LSP_PLUG_IN_LLTL_NEW_H_
#define LSP_PLUG_IN_LLTL_NEW_H_

#include <lsp-plug.in/lltl/version.h>
#include <lsp-plug.in/common/types.h>

#include <stddef.h>

namespace lsp
{
    namespace lltl
    {
        typedef struct allocator_tag_t
        {
        } allocator_tag_t;
    } /* namespace lltl */
} /* namespace lsp */

inline void *operator new(size_t size, void *ptr, const lsp::lltl::allocator_tag_t & tag)
{
    return ptr;
}

#endif /* LSP_PLUG_IN_LLTL_NEW_H_ */
