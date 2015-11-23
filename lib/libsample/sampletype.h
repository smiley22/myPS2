/*
 * libsample - audio sample convertion/mixing library
 * Copyright (C) 2002 Lennart Poettering
 *
 * libsample is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * libsample is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsample; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * $Id: sampletype.h,v 1.13 2002/09/08 17:24:50 poettering Exp $
 */
#ifndef SAMPLETYPE_H
#define SAMPLETYPE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include <tamtypes.h>

typedef u8 uint8_t;
typedef u32 uint32_t;
    
/**
 * Encoding types supported by libsample.
 */
typedef enum {
    LS_SIGNED   = 0, 
    LS_UNSIGNED = 1, 
    LS_FLOAT    = 2, 
    LS_ALAW     = 3, 
    LS_ULAW     = 4
} ls_encoding;

#define LS_ENCODING_MAX 4

/**
 * Structure defining 'type' for audio samples.
 */
typedef struct {
    uint32_t rate:32;     /**< sample rate (samples/sec) */
    uint8_t bits:8;       /**< number of bits (bits/sample) */
    uint8_t channels:4;   /**< number of channels */
    ls_encoding enc:3;   /**< encoding type */
    int be:1;             /**< big endian or little endian? */
} ls_sample_type_t;

/**
 * Sample type used for comparing with SAMPLE_TYPE_UNSPEC.
 */
extern const ls_sample_type_t ls_sample_type_unspec;

/**
 * Special sample type describing an null/empty/unspecified state
 */
#define LS_SAMPLE_TYPE_UNSPEC ((ls_sample_type_t) { 0, 0, 0, 0, 0 })

/**
 * Test whether the given sample type is valid
 */
#define ls_sample_type_valid(t) ((t).channels \
                                 && (t).bits  \
                                 && (t).enc <= LS_ENCODING_MAX  \
                                 && (t).rate)

/**
 * Sample size of a given sample type
 */
#define ls_sample_type_size(st) (((st).channels)*((st).bits/8))

/**
 * Bytes per second of a given sample type
 */
#define ls_sample_type_bps(st) (sample_type_size(st)*(st).rate)

/**
 * Compare two sample types, returns true when equal
 */
#define ls_sample_type_equals(st1, st2)(((st1).channels == (st2).channels) && \
                                        ((st1).bits     == (st2).bits) && \
                                        ((st1).enc      == (st2).enc) && \
                                        ((st1).be       == (st2).be) && \
                                        ((st1).rate     == (st2).rate))

/**
 * Convert the sample type to string.
 * @param t the sample_type to convert
 * @param s the string to write into
 * @param l then length of the s
 * @return s
 */
char *ls_sample_type_to_string (ls_sample_type_t t, char *s, int l);

/**
 * Convert the sample type to a compact string.
 * @param t the sample_type to convert
 * @param s the string to write into
 * @param l then length of the s
 * @return s
 */
char *ls_sample_type_to_string_compact (ls_sample_type_t t, char *s, int l);

/**
 * Convert a string with a representation of a sample type into a sample type
 * @param s the string to convert
 * @return the sample type
 */
ls_sample_type_t ls_sample_type_parse_compact(char *s);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SAMPLETYPE_H */
