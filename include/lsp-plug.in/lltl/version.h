/*
 * version.h
 *
 *  Created on: 8 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_VERSION_H_
#define LSP_PLUG_IN_LLTL_VERSION_H_

#define LSP_LLTL_LIB_MAJOR      0
#define LSP_LLTL_LIB_MINOR      5
#define LSP_LLTL_LIB_MICRO      2

#ifdef LSP_LLTL_LIB_BUILTIN
    #define LSP_LLTL_LIB_EXPORT
    #define LSP_LLTL_LIB_CEXPORT
    #define LSP_LLTL_LIB_IMPORT
    #define LSP_LLTL_LIB_CIMPORT
#else
    #define LSP_LLTL_LIB_EXPORT         LSP_SYMBOL_EXPORT
    #define LSP_LLTL_LIB_CEXPORT        LSP_CSYMBOL_EXPORT
    #define LSP_LLTL_LIB_IMPORT         LSP_SYMBOL_IMPORT
    #define LSP_LLTL_LIB_CIMPORT        LSP_CSYMBOL_IMPORT
#endif

#endif /* LSP_PLUG_IN_LLTL_VERSION_H_ */
