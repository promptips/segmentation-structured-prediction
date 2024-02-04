/* zconf.h -- configuration of the zlib compression library
 * Copyright (C) 1995-2005 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#ifndef ZCONF_H
#define ZCONF_H

/*
 * If you *really* need a unique prefix for all types and library functions,
 * compile with -DZ_PREFIX. The "standard" zlib should be compiled without it.
 */
#ifdef Z_PREFIX
#  define deflateInit_          z_deflateInit_
#  define deflate               z_deflate
#  define deflateEnd            z_deflateEnd
#  define inflateInit_          z_inflateInit_
#  define inflate               z_inflate
#  define inflateEnd            z_inflateEnd
#  define deflateInit2_         z_deflateInit2_
#  define deflateSetDictionary  z_deflateSetDictionary
#  define deflateCopy           z_deflateCopy
#  define deflateReset          z_deflateReset
#  define deflateParams         z_deflateParams
#  define deflateBound          z_deflateBound
#  define deflatePrime          z_deflatePrime
#  define inflateInit2_         z_inflateInit2_
#  define inflateSetDictionary  z_inflateSetDictionary
#  define inflateSync           z_inflateSync
#  define inflateSyncPoint      z_inflateSyncPoint
#  define inflateCopy           z_inflateCopy
#  define inflateReset          z_inflateReset
#  define inflateBack           z_inflateBack
#  define inflateBackEnd        z_inflateBackEnd
#  define compress              z_compress
#  define compress2             z_compress2
#  define compressBound         z_compressBound
#  define uncompress            z_uncompress
#  define adler32               z_adler32
#  define crc32                 z_crc32
#  define get_crc_table         z_get_crc_table
#  define zError                z_zError

#  define alloc_func            z_alloc_func
#  define free_func             z_free_func
#  define in_func               z_in_func
#  define out_func              z_out_func
#  define Byte                  z_Byte
#  define uInt                  z_uInt
#  define uLong                 z_uLong
#  define Bytef                 z_Bytef
#  define charf                 z_charf
#  define intf                  z_intf
#  define uIntf                 z_uIntf
#  define uLongf                z_uLongf
#  define voidpf                z_voidpf
#  define voidp                 z_voidp
#