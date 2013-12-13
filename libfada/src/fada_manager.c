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

#include <fada/fada.h>
#include "fada_manager.h"
#include "fada_fftbuffer.h"
#include "fada_mem.h"

#include <limits.h>


//////////////////////////////////////////////////
fada_Manager* fada_newmanager()
{
	fada_Manager* m;
	m = (fada_Manager*)fada_memalloc(sizeof(fada_Manager));

	if (!m)
		return NULL;

	m->current_chunk = NULL;
	m->first_chunk = NULL;
	m->last_chunk = NULL;

	m->sample_count = 0;
	m->current_sample = 0;

	m->sample_type = FADA_TSAMPLE_NOTSET;
	m->channels = 0;
	m->sample_rate = 0;

	m->fft.buffer = NULL;
	m->fft.internal = FADA_FALSE;

	m->window.buffer = NULL;
	m->window.size = 0;
	m->window.filled = FADA_FALSE;

	m->ready = FADA_FALSE;

	return m;
}


//////////////////////////////////////////////////
void fada_closemanager(fada_Manager* m)
{
	fada_freechunks(m);

	if (m->window.buffer)
		fada_memfree(m->window.buffer);

	if (m->fft.buffer && m->fft.internal)
		fada_closefftbuffer(m->fft.buffer);

	fada_memfree(m);
}


//////////////////////////////////////////////////
fada_Error fada_bindsamples(fada_Manager* m, void* data, fada_Pos sample_count, fada_Boolean copy_data, fada_TSample sample_type, unsigned int sample_rate, unsigned int channels)
{
	fada_Error err;

	if (!data) return FADA_ERROR_NO_DATA;
	if (!channels) return FADA_ERROR_INVALID_CHANNEL;
	if (!sample_count) return FADA_ERROR_INVALID_SIZE;
	if (sample_count % channels) return FADA_ERROR_NOT_MULTIPLE_OF_CHANNELS;
	if (!sample_rate) return FADA_ERROR_INVALID_SAMPLE_RATE;

	// Assign audio information to manager.
	m->sample_type = sample_type;
	m->channels = channels;
	m->sample_rate = sample_rate;

	m->ready = FADA_TRUE;

	// Create default window buffer.
	err = fada_setwindowframes(m, FADA_DEFAULT_WINDOWSIZE);
	if (err != FADA_ERROR_SUCCESS)
	{
		m->ready = FADA_FALSE;
		return err;
	}

	// Make way for our new sample data.
	fada_freechunks(m);

	// Push sample data as one chunk.
	return fada_pushsamples(m, data, sample_count, copy_data);
}


//////////////////////////////////////////////////
fada_Error fada_bindstream(fada_Manager* m, fada_TSample sample_type, unsigned int sample_rate, unsigned int channels)
{
	fada_Error err;

	if (!channels) return FADA_ERROR_INVALID_CHANNEL;
	if (!sample_rate) return FADA_ERROR_INVALID_SAMPLE_RATE;

	// Assign audio information to manager.
	m->sample_type = sample_type;
	m->channels = channels;
	m->sample_rate = sample_rate;

	m->ready = FADA_TRUE;

	// Create default window buffer.
	err = fada_setwindowframes(m, FADA_DEFAULT_WINDOWSIZE);
	if (err != FADA_ERROR_SUCCESS)
	{
		m->ready = FADA_FALSE;
		return err;
	}
	
	// Make way for new chunks.
	fada_freechunks(m);
	
	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_pushsamples(fada_Manager* m, void* data, fada_Pos sample_count, fada_Boolean copy_data)
{
	fada_Chunk* newchunk;

	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;
	if (!data) return FADA_ERROR_NO_DATA;
	if (!sample_count) return FADA_ERROR_INVALID_SIZE;
	if (sample_count % m->channels) return FADA_ERROR_NOT_MULTIPLE_OF_CHANNELS;

	newchunk = fada_newchunk();
	if (!newchunk)
		return FADA_ERROR_NOT_ENOUGH_MEMORY;

	// Assign sample data to this new chunk.
	if (copy_data)
	{
		int so = 1;
		switch (m->sample_type)
		{
			default: {
				fada_memfree(newchunk);
				return FADA_ERROR_INVALID_TYPE;
			}
			case FADA_TSAMPLE_INT8:    so = 1; break;
			case FADA_TSAMPLE_INT16:   so = 2; break;
			case FADA_TSAMPLE_INT32:   so = 4; break;
			case FADA_TSAMPLE_INT64:   so = 8; break;
			case FADA_TSAMPLE_FLOAT32: so = 4; break;
			case FADA_TSAMPLE_FLOAT64: so = 8; break;
		}

		newchunk->samples = fada_memalloc(sample_count * so);
		if (!newchunk->samples)
		{
			fada_memfree(newchunk);
			return FADA_ERROR_NOT_ENOUGH_MEMORY;
		}

		fada_memcopy(newchunk->samples, data, sample_count * so);

		newchunk->sample_count = sample_count;
		newchunk->samples_copied = FADA_TRUE;
	}
	else
	{
		newchunk->samples = data;
		newchunk->sample_count = sample_count;
		newchunk->samples_copied = FADA_FALSE;
	}

	// Update chunk links.
	if (m->first_chunk)
	{
		// Link new chunk as the next chunk of the current last chunk.
		m->last_chunk->next = newchunk;
		newchunk->prev = m->last_chunk;
		newchunk->position = m->last_chunk->position + m->last_chunk->sample_count;

		m->last_chunk = newchunk;
	}
	else
	{
		// This is our first chunk pushed to the manager.
		m->current_chunk = newchunk;
		m->first_chunk = newchunk;
		m->last_chunk = newchunk;
	}

	m->sample_count += newchunk->sample_count;
	
	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
void fada_trimchunks(fada_Manager* m)
{
	fada_Chunk* cur, *next;
	fada_Pos freed = 0;

	if (!m || !m->current_chunk)
		return;

	// Free all previous chunks.
	for (cur = m->current_chunk->prev; cur != NULL; cur = next)
	{
		next = cur->prev;
		freed += cur->sample_count;

		fada_closechunk(cur);
	}
	
	m->current_chunk->prev = NULL;

	// Update chunk positions.
	for (cur = m->current_chunk->next; cur != NULL; cur = next)
	{
		next = cur->next;
		cur->position -= freed;
	}
	
	m->sample_count -= freed;
	m->first_chunk = m->current_chunk;
}


//////////////////////////////////////////////////
void fada_freechunks(fada_Manager* m)
{
	fada_Chunk* cur, *next;

	if (!m)
		return;

	for (cur = m->first_chunk; cur != NULL; cur = next)
	{
		next = cur->next;
		
		fada_closechunk(cur);
	}

	m->first_chunk = NULL;
	m->current_chunk = NULL;
	m->last_chunk = NULL;

	m->current_sample = 0;
	m->sample_count = 0;
}


//////////////////////////////////////////////////
fada_TSample fata_getsampletype(const fada_Manager* m)
{
	return m->sample_type;
}


//////////////////////////////////////////////////
unsigned int fada_getsamplerate(const fada_Manager* m)
{
	return m->sample_rate;
}


//////////////////////////////////////////////////
unsigned int fada_getsamplecount(const fada_Manager* m)
{
	if (!m->ready)
		return 0;
	return m->sample_count;
}


//////////////////////////////////////////////////
unsigned int fada_getframecount(const fada_Manager* m)
{
	if (!m->ready)
		return 0;
	return m->sample_count / m->channels;
}


//////////////////////////////////////////////////
unsigned int fada_getchannels(const fada_Manager* m)
{
	return m->channels;
}


//////////////////////////////////////////////////
fada_Pos fada_getposition(const fada_Manager* m)
{
	return (m->current_chunk->position + m->current_sample) / m->channels;
}


//////////////////////////////////////////////////
fada_Pos fada_getwindowsize(const fada_Manager* m)
{
	return m->window.size;
}


//////////////////////////////////////////////////
fada_Pos fada_getwindowframes(const fada_Manager* m)
{
	return m->window.size / m->channels;
}


//////////////////////////////////////////////////
fada_Error fada_setposition(fada_Manager* m, fada_Pos pos)
{
	fada_Chunk* chunk;

	if (!m->ready)
		return FADA_ERROR_MANAGER_NOT_READY;

	// Internally we use samples.
	pos *= m->channels;

	if (pos >= m->sample_count)
		return FADA_ERROR_POSITION_OUT_OF_BOUNDS;
	
	// Find the appropriate chunk for the requested position.
	for (chunk = m->first_chunk; chunk != NULL; chunk = chunk->next)
	{
		if (chunk->position + chunk->sample_count > pos)
		{
			m->current_sample = pos - chunk->position;
			m->current_chunk = chunk;
			break;
		}
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_setwindowframes(fada_Manager* m, fada_Pos frames)
{
	int so;
	void* buf;

	if (!frames) return FADA_ERROR_INVALID_SIZE;
	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;

	if (frames == m->window.size / m->channels)
		return FADA_ERROR_SUCCESS;

	switch (m->sample_type)
	{
		default: return FADA_ERROR_INVALID_TYPE;
		case FADA_TSAMPLE_INT8:    so = 1; break;
		case FADA_TSAMPLE_INT16:   so = 2; break;
		case FADA_TSAMPLE_INT32:   so = 4; break;
		case FADA_TSAMPLE_INT64:   so = 8; break;
		case FADA_TSAMPLE_FLOAT32: so = 4; break;
		case FADA_TSAMPLE_FLOAT64: so = 8; break;
	}

	buf = fada_memalloc(frames * m->channels * so);
	if (!buf)
		return FADA_ERROR_NOT_ENOUGH_MEMORY;

	if (m->window.buffer)
		fada_memfree(m->window.buffer);

	m->window.buffer = buf;
	m->window.size   = frames * m->channels;
	m->window.filled = FADA_FALSE;

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Boolean fada_continue(fada_Manager* m, long offset_frames)
{
	fada_Chunk* chunk;
	
	if (!m->ready)
		return FADA_FALSE;

	if (!m->current_chunk)
		return FADA_FALSE;

	if (fada_endofaudio(m))
		return FADA_FALSE;
	
	m->window.filled = FADA_FALSE;

	// Move current sample forward.
	if (offset_frames < 0)
		m->current_sample += -offset_frames * m->window.size;
	else
		m->current_sample += offset_frames * m->channels;

	// Continue to the next chunk if we've surpassed this one.
	for (chunk = m->current_chunk; m->current_sample >= chunk->sample_count; m->current_sample -= chunk->sample_count)
	{
		if (chunk->next)
		{
			chunk = chunk->next;
		}
		else
		{
			// Reached last chunk.
			m->current_sample = chunk->sample_count;
			break;
		}
	}
	m->current_chunk = chunk;
	
	return !fada_endofaudio(m);
}


//////////////////////////////////////////////////
fada_Boolean fada_continuetolast(fada_Manager* m)
{
	fada_Chunk* chunk;
	int n;
	
	if (!m->ready)
		return FADA_FALSE;

	if (!m->current_chunk)
		return FADA_FALSE;

	m->window.filled = FADA_FALSE;

	n = m->window.size;
	for (chunk = m->last_chunk; n > 0;)
	{
		n -= chunk->sample_count;

		if (n <= 0)
		{
			break;
		}
		else
		{
			if (chunk->prev)
			{
				chunk = chunk->prev;
			}
			else
			{
				m->current_chunk = chunk;
				m->current_sample = 0;

				return FADA_TRUE;
			}
		}
	}
	m->current_chunk = chunk;
	m->current_sample = chunk->sample_count + n;
	
	return FADA_TRUE;
}


//////////////////////////////////////////////////
fada_Boolean fada_endofaudio(const fada_Manager* m)
{
	if (!m->current_chunk)
		return FADA_TRUE;
	return ((m->current_chunk->position + m->current_sample) >= m->sample_count);
}


//////////////////////////////////////////////////
fada_Res fada_getnormalizer(const fada_Manager* m)
{
	if (!m->ready) return 1.;

	switch (m->sample_type)
	{
		//TODO: Should these be signed maxes?
		case FADA_TSAMPLE_INT8:    return (fada_Res)UCHAR_MAX / 2.;
		case FADA_TSAMPLE_INT16:   return (fada_Res)USHRT_MAX / 2.;
		case FADA_TSAMPLE_INT32:   return (fada_Res)UINT_MAX / 2.;
		case FADA_TSAMPLE_INT64:   return (fada_Res)ULLONG_MAX / 2.;
		case FADA_TSAMPLE_FLOAT32: return 1.;
		case FADA_TSAMPLE_FLOAT64: return 1.;
		default: return 1.;
	}
}


//////////////////////////////////////////////////
fada_Error fada_preloadfftbuffer(fada_Manager* m)
{
	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;

	if (!m->fft.buffer)
	{
		m->fft.buffer = fada_newfftbuffer(m->window.size / m->channels);

		if (!m->fft.buffer)
			return FADA_ERROR_NOT_ENOUGH_MEMORY;

		m->fft.internal = FADA_TRUE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_usefftbuffer(fada_Manager* m, fada_FFTBuffer* b)
{
	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;

	// Close internal buffer.
	if (m->fft.buffer && m->fft.internal)
		fada_closefftbuffer(m->fft.buffer);

	m->fft.buffer = b;
	m->fft.internal = FADA_FALSE;

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
const fada_Res* fada_getfft(const fada_Manager* m)
{
	if (!m->fft.buffer)
		return NULL;
	return m->fft.buffer->buffer;
}


//////////////////////////////////////////////////
fada_Pos fada_getfftsize(const fada_Manager* m)
{
	if (!m->fft.buffer)
		return 0;
	return m->fft.buffer->size;
}


//////////////////////////////////////////////////
fada_Error fada_getfftvalue(const fada_Manager* m, fada_Pos pos, fada_Res* out_result)
{
	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;
	return fada_getfftvalue_buffer(m->fft.buffer, pos, out_result);
}


//////////////////////////////////////////////////
fada_Error fada_getfftvalues(const fada_Manager* m, fada_Res* out_results)
{
	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;
	return fada_getfftvalues_buffer(m->fft.buffer, out_results);
}


//////////////////////////////////////////////////
fada_Error fada_getfftvaluesrange(const fada_Manager* m, fada_Res* out_results, fada_Pos offset, fada_Pos len)
{
	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;
	return fada_getfftvaluesrange_buffer(m->fft.buffer, out_results, offset, len);
}


//////////////////////////////////////////////////
fada_Error fada_getfftvaluefromfrequency(const fada_Manager* m, fada_Res freq, fada_Res* out_result)
{
	if (!m->ready) return FADA_ERROR_MANAGER_NOT_READY;
	if (freq >= m->sample_rate / 2) return FADA_ERROR_FREQUENCY_OUT_OF_BOUNDS;
	return fada_getfftvalue_buffer(m->fft.buffer, (fada_Pos)(freq * m->fft.buffer->size) / m->sample_rate, out_result);
}