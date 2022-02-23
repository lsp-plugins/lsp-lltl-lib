/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 8 апр. 2020 г.
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

#ifndef LSP_PLUG_IN_LLTL_VERSION_H_
#define LSP_PLUG_IN_LLTL_VERSION_H_

#define LSP_LLTL_LIB_MAJOR      1
#define LSP_LLTL_LIB_MINOR      0
#define LSP_LLTL_LIB_MICRO      0

#ifdef LSP_LLTL_LIB_BUILTIN
    #define LSP_LLTL_LIB_EXPORT
    #define LSP_LLTL_LIB_CEXPORT
    #define LSP_LLTL_LIB_IMPORT         LSP_SYMBOL_IMPORT
    #define LSP_LLTL_LIB_CIMPORT        LSP_CSYMBOL_IMPORT
#else
    #define LSP_LLTL_LIB_EXPORT         LSP_SYMBOL_EXPORT
    #define LSP_LLTL_LIB_CEXPORT        LSP_CSYMBOL_EXPORT
    #define LSP_LLTL_LIB_IMPORT         LSP_SYMBOL_IMPORT
    #define LSP_LLTL_LIB_CIMPORT        LSP_CSYMBOL_IMPORT
#endif

#endif /* LSP_PLUG_IN_LLTL_VERSION_H_ */
