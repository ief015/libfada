/***********************************************************************************
 * libfada - Free Audio Detection and Analyzation Library                          *
 *                                                                                 *
 * The zlib/libpng License                                                         *
 * Copyright (c) 2013 Nathan Cousins                                               *
 *                                                                                 *
 * This software is provided 'as-is', without any express or implied warranty.     *
 * In no event will the authors be held liable for any damages arising from the    *
 * use of this software.                                                           *
 *                                                                                 *
 * Permission is granted to anyone to use this software for any purpose,           *
 * including commercial applications, and to alter it and redistribute it freely,  *
 * subject to the following restrictions:                                          *
 *                                                                                 *
 * 1. The origin of this software must not be misrepresented; you must not claim   *
 *    that you wrote the original software. If you use this software in a product, *
 *    an acknowledgment in the product documentation would be appreciated but is   *
 *    not required.                                                                *
 *                                                                                 *
 * 2. Altered source versions must be plainly marked as such, and must not be      *
 *    misrepresented as being the original software.                               *
 *                                                                                 *
 * 3. This notice may not be removed or altered from any source distribution.      *
 ***********************************************************************************/

#include "fada_fftbuffer.h"
#include "fada_mem.h"

#include <math.h>

#define _FADA_ISPOW2(a) (((a)&((a)-1))==0)


//////////////////////////////////////////////////
FADA_API fada_FFTBuffer* fada_newfftbuffer(fada_Pos size_po2)
{
	fada_FFTBuffer* buf;
	fada_Pos nearest = size_po2;
	size_t len;

	//Get the nearest power of two equal or under the window size (x2).
	if (!_FADA_ISPOW2(nearest))
	{
		nearest |= nearest >> 1;
		nearest |= nearest >> 2;
		nearest |= nearest >> 4;
		nearest |= nearest >> 8;
		nearest |= nearest >> 16;
		nearest  = (nearest + 1) >> 1;
	}

	// Create buffer object.
	buf = (fada_FFTBuffer*)fada_memalloc(sizeof(fada_FFTBuffer));
	if (!buf)
		return NULL;

	// FFT buffer size is doubled to store both imaginary + complex data per value.
	len = sizeof(fada_Res) * nearest * 2;

	// Allocate the buffer itself.
	buf->buffer = (fada_Res*)fada_memalloc(len);
	if (!buf->buffer)
	{
		fada_memfree(buf);
		return NULL;
	}

	// Zero the buffer.
	buf->size = nearest;

	return buf;
}


//////////////////////////////////////////////////
FADA_API void fada_closefftbuffer(fada_FFTBuffer* b)
{
	if (b->buffer)
		fada_memfree(b->buffer);

	fada_memfree(b);
}


//////////////////////////////////////////////////
FADA_API const fada_Res* fada_getfft_buffer(const fada_FFTBuffer* b)
{
	if (!b)
		return NULL;

	return b->buffer;
}


//////////////////////////////////////////////////
FADA_API fada_Pos fada_getfftsize_buffer(const fada_FFTBuffer* b)
{
	if (!b)
		return 0;

	return b->size;
}


//////////////////////////////////////////////////
FADA_API fada_Error fada_getfftvalue_buffer(const fada_FFTBuffer* b, fada_Pos pos, fada_Res* out_result)
{
	if (!out_result) return FADA_ERROR_INVALID_PARAMETER;
	if (!b) return FADA_ERROR_INVALID_FFT_BUFFER;
	if (pos >= b->size) return FADA_ERROR_INDEX_OUT_OF_BOUNDS;

	(*out_result) = (fabs(b->buffer[2*pos]) + fabs(b->buffer[2*pos+1])) / b->size;

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
FADA_API fada_Error fada_getfftvalues_buffer(const fada_FFTBuffer* b, fada_Res* out_results)
{
	unsigned int i;

	if (!out_results) return FADA_ERROR_INVALID_PARAMETER;
	if (!b) return FADA_ERROR_INVALID_FFT_BUFFER;

	for (i = 0; i < b->size; ++i)
	{
		out_results[i] = (fabs(b->buffer[2*i]) + fabs(b->buffer[2*i+1])) / b->size;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
FADA_API fada_Error fada_getfftvaluesrange_buffer(const fada_FFTBuffer* b, fada_Res* out_results, fada_Pos offset, fada_Pos len)
{
	unsigned int i;

	if (!out_results) return FADA_ERROR_INVALID_PARAMETER;
	if (!b) return FADA_ERROR_INVALID_FFT_BUFFER;

	if (!len)
		len = b->size - offset;

	if (offset + len > b->size) return FADA_ERROR_INDEX_OUT_OF_BOUNDS;

	for (i = 0; i < len; ++i)
	{
		out_results[i] = (fabs(b->buffer[2*(i+offset)]) + fabs(b->buffer[2*(i+offset)+1])) / b->size;
	}

	return FADA_ERROR_SUCCESS;
}