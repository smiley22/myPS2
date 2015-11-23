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
 * $Id: sample.h,v 1.13 2002/06/20 10:35:56 samc Exp $
 */
#ifndef SAMPLE_H
#define SAMPLE_H

#include "sampletype.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Type used my libsample to measure length and size (e.g. number
 * of bytes or number of samples).
 */
typedef uint32_t ls_size_t;
#define LS_SIZE_MAX UINT32_MAX
#define LS_SIZE_MIN UINT32_MIN

/**
 * Internal type for fixed point numbers.
 */
typedef uint32_t ls_fixed_t;

/**
 * Possible flags to pass to ls_get_resampler.
 * For the moment each implementation should add an entry to this enum.
 */
enum
{
  LS_BEST,
  LS_FASTEST,
  LS_LINEAR,
  LS_BANDLIMITED
};

/**
 * Prototype for resampler initialiation routine.
 * @param from sample type to convert from. 
 * @param to sample type to convert to.
 * @returns a void pointer to some internal state or null.
 */
typedef void *(*ls_resampler_init_t) (ls_sample_type_t from, ls_sample_type_t to);

/**
 * Prototype for resmapler termination routine.
 * @param state the current state of the resampler.
 * @returns the new state of the resampler.
 */
typedef void (*ls_resampler_term_t) (void *state);

/**
 * Prototype for resampling routines.  
 * It is up to the caller to ensure that out_length is big enough by calling
 * ls_resampled_size.
 * @param in_data audio data to be resampled.
 * @param out_data memory for resampled audio data.
 * @param in_len length in bytes of in_data.
 * @param out_len maximum length in bytes of out_data.
 * @return number of bytes written to out_data.
 */
typedef ls_size_t (*ls_resampler_run_t) (void *state, void *in_data, void *out_data, 
                                         ls_size_t in_len, ls_size_t out_len);

/**
 * Resampler struct.
 */
typedef struct
{
  ls_resampler_init_t init;    /**< initialize the resampler */
  ls_resampler_term_t term;    /**< terminate the resampler (free internal memory) */
  ls_resampler_run_t resample; /**< run the resmapler */
} 
ls_resampler_t;

/**
 * A mixing buffer structure, contains information about one of the to be mixed
 * buffers
 */
typedef struct 
{
  void *data;        /**< The audio buffer */
  ls_size_t length;  /**< The length of that audio buffer */
  float factor;      /**< Pre-mixing multiplier */
} 
ls_mix_buffer_t;

/**
 * Prototype for mixing routines.  "result" must be at least
 * as long as the longest of the buffer to be mixed.
 * @param mb array of buffers to be mixed.
 * @param count the number of buffers to be mixed.
 * @param result memory for mixed samples to be written to.
 * @return the length of the resulting mixed buffer.
 */
typedef ls_size_t (*ls_mix_proc_t) (ls_sample_type_t format, 
                                    ls_mix_buffer_t *mb, int count, 
                                    void *result);

/**
 * Prototype for multiplication routines. 
 * "in_data" and "out_data" may point to the same memory.
 * "out_data" must be at least as long as "in_data"
 * @param data the audio data to be multiplied.
 * @param target the resulting multiplied data.
 * @param factor the multiplying factor
 * @param length the lenght the data, and target buffers.
 */
typedef ls_size_t (*ls_multiply_proc_t) (ls_sample_type_t format, float factor,
                                         void *in_data, void *out_data,
                                         ls_size_t length);

/**
 * Find a suitable resampling routine for the specified conversion.
 * @return an appropriate resampler or NULL if none was found.
 */
ls_resampler_t *ls_get_resampler (ls_sample_type_t from, ls_sample_type_t to, 
                                  int flags);

/**
 * Calculate the converted size of a data block with "length" bytes in
 * sample format "from" to sample format "to".
 * @return the size of the buffer that would be required.
 */
ls_size_t ls_resampled_size(ls_sample_type_t from, ls_sample_type_t to, 
                            ls_size_t length);

/**
 * Find a suitable mixing routine for the specified format. 
 * If "multiply" is nonzero the function must not ignore the "factor" 
 * field of the lsMixBuffer. If "multiply" is zero, it may or may not ignore
 * it. 
 * @return the appropriate function or NULL if none was found
 */
ls_mix_proc_t ls_get_mixer (ls_sample_type_t format, int multiply);

/**
 * Find a suitable multiplying routine for the specified format. 
 * @returns the appropriate function or NULL if none was found.
 */
ls_multiply_proc_t ls_get_multiplyer (ls_sample_type_t format);

/**
 * Sets the memory in "data" with length "length" to the normalized
 * silence of the given format.  For most formats this will work like a
 * bzero() (but not, for example, unsigned formats).
 * @returns 0 on sucess, non-zero on failure.
 */
int ls_silence (ls_sample_type_t format, void *data, ls_size_t length);

/**
 * Idiot-proof resampling routine. Converts the data in "data" from
 * "from" to "to". The original length is read from "*l", the
 * resulting length will be written to "*l" as well. Returns a pointer
 * to (thread specific) memory, which will be freed by the
 * library. The next call to ls_resample() may overwrite the
 * buffer. 
 * This is essentially a wrapper function for ls_get_resampler followed by a
 * call to resulting resampler.
 * @returns NULL on failure. 
 */
void *ls_resample (ls_sample_type_t from, ls_sample_type_t to, void *data, ls_size_t *l);

/**
 * Idiot-proof mixing routine. This is essentially a wrapper function 
 * for ls_get_mixer followed by a call to resulting mixer.
 */
void *ls_mix (ls_sample_type_t format, ls_mix_buffer_t *mb, int count, 
              ls_size_t *result_length);

/**
 * Idiot-proof multiplying routine. This is essenitally a wrapper for
 * ls_get_multiplyer followed by a call the resulting multiplier.
 */
void *ls_multiply (ls_sample_type_t format, float factor, void *data, 
                   ls_size_t length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SAMPLE_H */
