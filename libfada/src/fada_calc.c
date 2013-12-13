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
#include "fada_calc.h"
#include "fada_manager.h"
#include "fada_fftbuffer.h"
#include "fada_mem.h"

#include <math.h>
#include <stdio.h>

#define _FADA_FFT_SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define _FADA_MIN(a,b) ((a)>(b)?(b):(a))
#define _FADA_PI 3.1415926535897932384626433832795028842


//////////////////////////////////////////////////
void fada_fillwindowbuffer_i8(fada_Manager* m)
{
	char* buf = (char*)m->window.buffer;
	fada_Chunk* chunk = m->current_chunk;
	unsigned int i, o, n;

	if (m->window.filled)
		return;

	o = m->current_sample;

	for (i = 0; i < m->window.size; i += n)
	{
		if (chunk)
		{
			n = _FADA_MIN(chunk->sample_count - o, m->window.size - i);
			fada_memcopy(&buf[i], &((char*)chunk->samples)[o], n);
			
			chunk = chunk->next;
			o = 0;
		}
		else
		{
			fada_memzero(&buf[i], (m->window.size - i));
			break;
		}
	}
	
	m->window.filled = FADA_TRUE;
}


//////////////////////////////////////////////////
void fada_fillwindowbuffer_i16(fada_Manager* m)
{
	short* buf = (short*)m->window.buffer;
	fada_Chunk* chunk = m->current_chunk;
	unsigned int i, o, n;

	if (m->window.filled)
		return;

	o = m->current_sample;

	for (i = 0; i < m->window.size; i += n)
	{
		if (chunk)
		{
			n = _FADA_MIN(chunk->sample_count - o, m->window.size - i);
			fada_memcopy(&buf[i], &((short*)chunk->samples)[o], n * sizeof(short));
			
			chunk = chunk->next;
			o = 0;
		}
		else
		{
			fada_memzero(&buf[i], (m->window.size - i) * sizeof(short));
			break;
		}
	}
	
	m->window.filled = FADA_TRUE;
}


//////////////////////////////////////////////////
void fada_fillwindowbuffer_i32(fada_Manager* m)
{
	int* buf = (int*)m->window.buffer;
	fada_Chunk* chunk = m->current_chunk;
	unsigned int i, o, n;

	if (m->window.filled)
		return;

	o = m->current_sample;

	for (i = 0; i < m->window.size; i += n)
	{
		if (chunk)
		{
			n = _FADA_MIN(chunk->sample_count - o, m->window.size - i);
			fada_memcopy(&buf[i], &((int*)chunk->samples)[o], n * sizeof(int));
			
			chunk = chunk->next;
			o = 0;
		}
		else
		{
			fada_memzero(&buf[i], (m->window.size - i) * sizeof(int));
			break;
		}
	}
	
	m->window.filled = FADA_TRUE;
}


//////////////////////////////////////////////////
void fada_fillwindowbuffer_i64(fada_Manager* m)
{
	long long* buf = (long long*)m->window.buffer;
	fada_Chunk* chunk = m->current_chunk;
	unsigned int i, o, n;

	if (m->window.filled)
		return;

	o = m->current_sample;

	for (i = 0; i < m->window.size; i += n)
	{
		if (chunk)
		{
			n = _FADA_MIN(chunk->sample_count - o, m->window.size - i);
			fada_memcopy(&buf[i], &((long long*)chunk->samples)[o], n * sizeof(long long));
			
			chunk = chunk->next;
			o = 0;
		}
		else
		{
			fada_memzero(&buf[i], (m->window.size - i) * sizeof(long long));
			break;
		}
	}
	
	m->window.filled = FADA_TRUE;
}


//////////////////////////////////////////////////
void fada_fillwindowbuffer_f32(fada_Manager* m)
{
	float* buf = (float*)m->window.buffer;
	fada_Chunk* chunk = m->current_chunk;
	unsigned int i, o, n;

	if (m->window.filled)
		return;

	o = m->current_sample;

	for (i = 0; i < m->window.size; i += n)
	{
		if (chunk)
		{
			n = _FADA_MIN(chunk->sample_count - o, m->window.size - i);
			fada_memcopy(&buf[i], &((float*)chunk->samples)[o], n * sizeof(float));
			
			chunk = chunk->next;
			o = 0;
		}
		else
		{
			fada_memzero(&buf[i], (m->window.size - i) * sizeof(float));
			break;
		}
	}
	
	m->window.filled = FADA_TRUE;
}


//////////////////////////////////////////////////
void fada_fillwindowbuffer_f64(fada_Manager* m)
{
	double* buf = (double*)m->window.buffer;
	fada_Chunk* chunk = m->current_chunk;
	unsigned int i, o, n;

	if (m->window.filled)
		return;

	o = m->current_sample;

	for (i = 0; i < m->window.size; i += n)
	{
		if (chunk)
		{
			n = _FADA_MIN(chunk->sample_count - o, m->window.size - i);
			fada_memcopy(&buf[i], &((double*)chunk->samples)[o], n * sizeof(double));
			
			chunk = chunk->next;
			o = 0;
		}
		else
		{
			fada_memzero(&buf[i], (m->window.size - i) * sizeof(double));
			break;
		}
	}
	
	m->window.filled = FADA_TRUE;
}


//////////////////////////////////////////////////
fada_Res fada_getsample_i8(fada_Manager* m, fada_Pos pos)
{
	const char* buf = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	return (fada_Res)buf[pos];
}


//////////////////////////////////////////////////
fada_Res fada_getsample_i16(fada_Manager* m, fada_Pos pos)
{
	const short* buf = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	return (fada_Res)buf[pos];
}


//////////////////////////////////////////////////
fada_Res fada_getsample_i32(fada_Manager* m, fada_Pos pos)
{
	const int* buf = (int*)m->window.buffer;
	fada_fillwindowbuffer_i32(m);

	return (fada_Res)buf[pos];
}


//////////////////////////////////////////////////
fada_Res fada_getsample_i64(fada_Manager* m, fada_Pos pos)
{
	const long long* buf = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	return (fada_Res)buf[pos];
}


//////////////////////////////////////////////////
fada_Res fada_getsample_f32(fada_Manager* m, fada_Pos pos)
{
	const float* buf = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	return (fada_Res)buf[pos];
}


//////////////////////////////////////////////////
fada_Res fada_getsample_f64(fada_Manager* m, fada_Pos pos)
{
	const double* buf = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	return (fada_Res)buf[pos];
}


//////////////////////////////////////////////////
void fada_getsamples_i8(fada_Manager* m, unsigned int channel, fada_Res* out_results)
{
	unsigned int i, sz;
	const char* buf = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		out_results[i] = (fada_Res)buf[i * m->channels + channel];
	}
}


//////////////////////////////////////////////////
void fada_getsamples_i16(fada_Manager* m, unsigned int channel, fada_Res* out_results)
{
	unsigned int i, sz;
	const short* buf = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		out_results[i] = (fada_Res)buf[i * m->channels + channel];
	}
}


//////////////////////////////////////////////////
void fada_getsamples_i32(fada_Manager* m, unsigned int channel, fada_Res* out_results)
{
	unsigned int i, sz;
	const int* buf = (int*)m->window.buffer;
	fada_fillwindowbuffer_i32(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		out_results[i] = (fada_Res)buf[i * m->channels + channel];
	}
}


//////////////////////////////////////////////////
void fada_getsamples_i64(fada_Manager* m, unsigned int channel, fada_Res* out_results)
{
	unsigned int i, sz;
	const long long* buf = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		out_results[i] = (fada_Res)buf[i * m->channels + channel];
	}
}


//////////////////////////////////////////////////
void fada_getsamples_f32(fada_Manager* m, unsigned int channel, fada_Res* out_results)
{
	unsigned int i, sz;
	const float* buf = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		out_results[i] = (fada_Res)buf[i * m->channels + channel];
	}
}


//////////////////////////////////////////////////
void fada_getsamples_f64(fada_Manager* m, unsigned int channel, fada_Res* out_results)
{
	unsigned int i, sz;
	const double* buf = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		out_results[i] = (fada_Res)buf[i * m->channels + channel];
	}
}


//////////////////////////////////////////////////
fada_Res fada_getframe_i8(fada_Manager* m, fada_Pos pos)
{
	unsigned int i;
	fada_Res res = 0.;

	const char* buf = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	for (i = 0; i < m->channels; ++i)
		res += (fada_Res)buf[pos + i];

	return res / m->channels;
}


//////////////////////////////////////////////////
fada_Res fada_getframe_i16(fada_Manager* m, fada_Pos pos)
{
	unsigned int i;
	fada_Res res = 0.;

	const short* buf = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	for (i = 0; i < m->channels; ++i)
		res += (fada_Res)buf[pos + i];

	return res / m->channels;
}


//////////////////////////////////////////////////
fada_Res fada_getframe_i32(fada_Manager* m, fada_Pos pos)
{
	unsigned int i;
	fada_Res res = 0.;

	const int* buf = (int*)m->window.buffer;
	fada_fillwindowbuffer_i32(m);

	for (i = 0; i < m->channels; ++i)
		res += (fada_Res)buf[pos + i];

	return res / m->channels;
}


//////////////////////////////////////////////////
fada_Res fada_getframe_i64(fada_Manager* m, fada_Pos pos)
{
	unsigned int i;
	fada_Res res = 0.;

	const long long* buf = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	for (i = 0; i < m->channels; ++i)
		res += (fada_Res)buf[pos + i];

	return res / m->channels;
}


//////////////////////////////////////////////////
fada_Res fada_getframe_f32(fada_Manager* m, fada_Pos pos)
{
	unsigned int i;
	fada_Res res = 0.;

	const float* buf = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	for (i = 0; i < m->channels; ++i)
		res += (fada_Res)buf[pos + i];

	return res / m->channels;
}


//////////////////////////////////////////////////
fada_Res fada_getframe_f64(fada_Manager* m, fada_Pos pos)
{
	unsigned int i;
	fada_Res res = 0.;

	const double* buf = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	for (i = 0; i < m->channels; ++i)
		res += (fada_Res)buf[pos + i];

	return res / m->channels;
}


//////////////////////////////////////////////////
void fada_getframes_i8(fada_Manager* m, fada_Res* out_results)
{
	unsigned int i, j, sz;
	const char* buf = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		for (j = 0; j < m->channels; ++j)
			out_results[i] = (fada_Res)buf[i * m->channels + j];

		out_results[i] /= m->channels;
	}
}


//////////////////////////////////////////////////
void fada_getframes_i16(fada_Manager* m, fada_Res* out_results)
{
	unsigned int i, j, sz;
	const short* buf = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		for (j = 0; j < m->channels; ++j)
			out_results[i] = (fada_Res)buf[i * m->channels + j];

		out_results[i] /= m->channels;
	}
}


//////////////////////////////////////////////////
void fada_getframes_i32(fada_Manager* m, fada_Res* out_results)
{
	unsigned int i, j, sz;
	const int* buf = (int*)m->window.buffer;
	fada_fillwindowbuffer_i32(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		for (j = 0; j < m->channels; ++j)
			out_results[i] = (fada_Res)buf[i * m->channels + j];

		out_results[i] /= m->channels;
	}
}


//////////////////////////////////////////////////
void fada_getframes_i64(fada_Manager* m, fada_Res* out_results)
{
	unsigned int i, j, sz;
	const long long* buf = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		for (j = 0; j < m->channels; ++j)
			out_results[i] = (fada_Res)buf[i * m->channels + j];

		out_results[i] /= m->channels;
	}
}


//////////////////////////////////////////////////
void fada_getframes_f32(fada_Manager* m, fada_Res* out_results)
{
	unsigned int i, j, sz;
	const float* buf = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		for (j = 0; j < m->channels; ++j)
			out_results[i] = (fada_Res)buf[i * m->channels + j];

		out_results[i] /= m->channels;
	}
}


//////////////////////////////////////////////////
void fada_getframes_f64(fada_Manager* m, fada_Res* out_results)
{
	unsigned int i, j, sz;
	const double* buf = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	for (i = 0, sz = m->window.size / m->channels; i < sz; ++i)
	{
		for (j = 0; j < m->channels; ++j)
			out_results[i] = (fada_Res)buf[i * m->channels + j];

		out_results[i] /= m->channels;
	}
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_i8(fada_Manager* m)
{
	unsigned int i, chan;
	fada_Res beat, avg;
	
	const char* samples = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		avg = 0.;

		for (chan = 0; chan < m->channels; ++chan)
			avg += abs(samples[i + chan] - samples[i + chan + m->channels]);

		beat += avg / m->channels;
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_i16(fada_Manager* m)
{
	unsigned int i, chan;
	fada_Res beat, avg;
	
	const short* samples = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		avg = 0.;

		for (chan = 0; chan < m->channels; ++chan)
			avg += abs(samples[i + chan] - samples[i + chan + m->channels]);

		beat += avg / m->channels;
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_i32(fada_Manager* m)
{
	unsigned int i, chan;
	fada_Res beat, avg;
	
	const int* samples = (int*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		avg = 0.;

		for (chan = 0; chan < m->channels; ++chan)
			avg += abs(samples[i + chan] - samples[i + chan + m->channels]);

		beat += avg / m->channels;
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_i64(fada_Manager* m)
{
	unsigned int i, chan;
	fada_Res beat, avg;
	
	const long long* samples = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		avg = 0.;

		for (chan = 0; chan < m->channels; ++chan)
			avg += llabs(samples[i + chan] - samples[i + chan + m->channels]);

		beat += avg / m->channels;
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_f32(fada_Manager* m)
{
	unsigned int i, chan;
	fada_Res beat, avg;
	
	const float* samples = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		avg = 0.;

		for (chan = 0; chan < m->channels; ++chan)
			avg += fabs(samples[i + chan] - samples[i + chan + m->channels]);

		beat += avg / m->channels;
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_f64(fada_Manager* m)
{
	unsigned int i, chan;
	fada_Res beat, avg;
	
	const double* samples = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		avg = 0.;

		for (chan = 0; chan < m->channels; ++chan)
			avg += fabs(samples[i + chan] - samples[i + chan + m->channels]);

		beat += avg / m->channels;
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_channel_i8(fada_Manager* m, unsigned int chan)
{
	unsigned int i;
	fada_Res beat;
	
	const char* samples = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		beat += abs(samples[i + chan] - samples[i + chan + m->channels]);
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_channel_i16(fada_Manager* m, unsigned int chan)
{
	unsigned int i;
	fada_Res beat;
	
	const short* samples = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		beat += abs(samples[i + chan] - samples[i + chan + m->channels]);
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_channel_i32(fada_Manager* m, unsigned int chan)
{
	unsigned int i;
	fada_Res beat;
	
	const int* samples = (int*)m->window.buffer;
	fada_fillwindowbuffer_i32(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		beat += abs(samples[i + chan] - samples[i + chan + m->channels]);
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_channel_i64(fada_Manager* m, unsigned int chan)
{
	unsigned int i;
	fada_Res beat;
	
	const long long* samples = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		beat += llabs(samples[i + chan] - samples[i + chan + m->channels]);
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_channel_f32(fada_Manager* m, unsigned int chan)
{
	unsigned int i;
	fada_Res beat;
	
	const float* samples = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		beat += fabs(samples[i + chan] - samples[i + chan + m->channels]);
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbeat_channel_f64(fada_Manager* m, unsigned int chan)
{
	unsigned int i;
	fada_Res beat;
	
	const double* samples = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	beat = 0.;
	for (i = 0; i < m->window.size; i += m->channels)
	{
		beat += fabs(samples[i + chan] - samples[i + chan + m->channels]);
	}

	return beat / (m->window.size / m->channels);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_i8(fada_Manager* m)
{
	unsigned int i, chan, subi;
	fada_Res bass, chan_avg, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const char* samples = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;
		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
		{
			chan_avg = 0.;

			for (chan = 0; chan < m->channels; ++chan)
				chan_avg += samples[subi + chan];

			sub_avg += chan_avg / m->channels;
		}
		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
/*
fada_Res fada_calcbass_i16(fada_Manager* m)
{
	unsigned int i, chan;
	fada_Res bass, chan_avg;
	
	const short* samples = &((short*)m->samples)[m->current_sample];
	fada_Pos last_sample = (m->current_sample < m->samples_size) ? m->samples_size - m->current_sample : 0;

	bass = 0.;
	for (i = 0; i < m->window_size && i < last_sample; i += m->channels)
	{
		chan_avg = 0.;
		for (chan = 0; chan < m->channels; ++chan)
		{
			chan_avg += abs(samples[i + chan]);
		}
		chan_avg /= m->channels;
		bass += chan_avg;
	}

	return bass / (m->window_size / m->channels);
}
*/
fada_Res fada_calcbass_i16(fada_Manager* m)
{
	unsigned int i, chan, subi;
	fada_Res bass, chan_avg, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const short* samples = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;
		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
		{
			chan_avg = 0.;

			for (chan = 0; chan < m->channels; ++chan)
				chan_avg += samples[subi + chan];

			sub_avg += chan_avg / m->channels;
		}
		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}



//////////////////////////////////////////////////
fada_Res fada_calcbass_i32(fada_Manager* m)
{
	unsigned int i, chan, subi;
	fada_Res bass, chan_avg, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const int* samples = (int*)m->window.buffer;
	fada_fillwindowbuffer_i32(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;
		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
		{
			chan_avg = 0.;

			for (chan = 0; chan < m->channels; ++chan)
				chan_avg += samples[subi + chan];

			sub_avg += chan_avg / m->channels;
		}
		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_i64(fada_Manager* m)
{
	unsigned int i, chan, subi;
	fada_Res bass, chan_avg, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const long long* samples = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;
		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
		{
			chan_avg = 0.;

			for (chan = 0; chan < m->channels; ++chan)
				chan_avg += samples[subi + chan];

			sub_avg += chan_avg / m->channels;
		}
		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_f32(fada_Manager* m)
{
	unsigned int i, chan, subi;
	fada_Res bass, chan_avg, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const float* samples = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;
		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
		{
			chan_avg = 0.;

			for (chan = 0; chan < m->channels; ++chan)
				chan_avg += samples[subi + chan];

			sub_avg += chan_avg / m->channels;
		}
		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_f64(fada_Manager* m)
{
	unsigned int i, chan, subi;
	fada_Res bass, chan_avg, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const double* samples = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;
		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
		{
			chan_avg = 0.;

			for (chan = 0; chan < m->channels; ++chan)
				chan_avg += samples[subi + chan];

			sub_avg += chan_avg / m->channels;
		}
		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_channel_i8(fada_Manager* m, unsigned int chan)
{
	unsigned int i, subi;
	fada_Res bass, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const char* samples = (char*)m->window.buffer;
	fada_fillwindowbuffer_i8(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;

		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
			sub_avg += samples[i + chan];

		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
/*
fada_Res fada_calcbass_channel_i16(fada_Manager* m, unsigned int chan)
{
	unsigned int i;
	fada_Res bass;
	
	const short* samples = &((short*)m->samples)[m->current_sample];
	fada_Pos last_sample = (m->current_sample < m->samples_size) ? m->samples_size - m->current_sample : 0;

	bass = 0.;
	for (i = 0; i < m->window_size && i < last_sample; i += m->channels)
	{
		bass += abs(samples[i + chan]);
	}

	return bass / (m->window_size / m->channels);
}
*/
fada_Res fada_calcbass_channel_i16(fada_Manager* m, unsigned int chan)
{
	unsigned int i, subi;
	fada_Res bass, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const short* samples = (short*)m->window.buffer;
	fada_fillwindowbuffer_i16(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;

		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
			sub_avg += samples[i + chan];

		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_channel_i32(fada_Manager* m, unsigned int chan)
{
	unsigned int i, subi;
	fada_Res bass, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const int* samples = (int*)m->window.buffer;
	fada_fillwindowbuffer_i32(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;

		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
			sub_avg += samples[i + chan];

		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_channel_i64(fada_Manager* m, unsigned int chan)
{
	unsigned int i, subi;
	fada_Res bass, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const long long* samples = (long long*)m->window.buffer;
	fada_fillwindowbuffer_i64(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;

		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
			sub_avg += samples[i + chan];

		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_channel_f32(fada_Manager* m, unsigned int chan)
{
	unsigned int i, subi;
	fada_Res bass, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const float* samples = (float*)m->window.buffer;
	fada_fillwindowbuffer_f32(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;

		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
			sub_avg += samples[i + chan];

		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
fada_Res fada_calcbass_channel_f64(fada_Manager* m, unsigned int chan)
{
	unsigned int i, subi;
	fada_Res bass, sub_avg;
	const unsigned int SUB_PERIOD = 32*m->channels;
	
	const double* samples = (double*)m->window.buffer;
	fada_fillwindowbuffer_f64(m);

	bass = 0.;
	for (i = 0; i < m->window.size; i += SUB_PERIOD)
	{
		sub_avg = 0.;

		for (subi = i; subi < i+SUB_PERIOD && subi < m->window.size; subi += m->channels)
			sub_avg += samples[i + chan];

		bass += fabs(sub_avg / (SUB_PERIOD / m->channels));
	}

	return bass / ((m->window.size / m->channels) / SUB_PERIOD);
}


//////////////////////////////////////////////////
void fada_calcfft_i8(fada_Manager* m)
{
	unsigned int i, j, c, rate;
	fada_Res normal, avg;
	
	const char* samples = (char*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i8(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < rate; ++i, j += m->channels)
	{
		avg = 0.;
		for (c = 0; c < m->channels; ++c)
			avg += (j + c < m->window.size) ? (samples[j + c] / normal) : 0.;
		fft[2*i] = avg / m->channels;
		fft[2*i+1] = 0.;
	}

	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_i16(fada_Manager* m)
{
	unsigned int i, j, c, rate;
	fada_Res normal, avg;
	
	const short* samples = (short*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i16(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < rate; ++i, j += m->channels)
	{
		avg = 0.;
		for (c = 0; c < m->channels; ++c)
			avg += (j + c < m->window.size) ? (samples[j + c] / normal) : 0.;
		fft[2*i] = avg / m->channels;
		fft[2*i+1] = 0.;
	}

	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_i32(fada_Manager* m)
{
	unsigned int i, j, c, rate;
	fada_Res normal, avg;
	
	const int* samples = (int*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i32(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < rate; ++i, j += m->channels)
	{
		avg = 0.;
		for (c = 0; c < m->channels; ++c)
			avg += (j + c < m->window.size) ? (samples[j + c] / normal) : 0.;
		fft[2*i] = avg / m->channels;
		fft[2*i+1] = 0.;
	}

	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_i64(fada_Manager* m)
{
	unsigned int i, j, c, rate;
	fada_Res normal, avg;
	
	const long long* samples = (long long*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i64(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < rate; ++i, j += m->channels)
	{
		avg = 0.;
		for (c = 0; c < m->channels; ++c)
			avg += (j + c < m->window.size) ? (samples[j + c] / normal) : 0.;
		fft[2*i] = avg / m->channels;
		fft[2*i+1] = 0.;
	}

	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_f32(fada_Manager* m)
{
	unsigned int i, j, c, rate;
	fada_Res avg;
	
	const float* samples = (float*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_f32(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < rate; ++i, j += m->channels)
	{
		avg = 0.;
		for (c = 0; c < m->channels; ++c)
			avg += (j + c < m->window.size) ? samples[j + c] : 0.;
		fft[2*i] = avg / m->channels;
		fft[2*i+1] = 0.;
	}

	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_f64(fada_Manager* m)
{
	unsigned int i, j, c, rate;
	fada_Res avg;
	
	const double* samples = (double*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_f64(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < rate; ++i, j += m->channels)
	{
		avg = 0.;
		for (c = 0; c < m->channels; ++c)
			avg += (j + c < m->window.size) ? samples[j + c] : 0.;
		fft[2*i] = avg / m->channels;
		fft[2*i+1] = 0.;
	}

	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_channel_i8(fada_Manager* m, unsigned int chan)
{
	unsigned int i, j, rate;
	fada_Res normal;

	const char* samples = (char*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i8(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < rate; ++i, j += m->channels)
	{
		fft[2*i] = (j + chan < m->window.size) ? (samples[j + chan] / normal) : 0.;
		fft[2*i+1] = 0.;
	}
	
	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_channel_i16(fada_Manager* m, unsigned int chan)
{
	unsigned int i, j, rate;
	fada_Res normal;

	const short* samples = (short*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i16(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < m->fft.buffer->size; ++i, j += m->channels)
	{
		fft[2*i] = (j + chan < m->window.size) ? (samples[j + chan] / normal) : 0.;
		fft[2*i+1] = 0.;
	}
	
	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_channel_i32(fada_Manager* m, unsigned int chan)
{
	unsigned int i, j, rate;
	fada_Res normal;

	const int* samples = (int*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i32(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < m->fft.buffer->size; ++i, j += m->channels)
	{
		fft[2*i] = (j + chan < m->window.size) ? (samples[j + chan] / normal) : 0.;
		fft[2*i+1] = 0.;
	}
	
	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_channel_i64(fada_Manager* m, unsigned int chan)
{
	unsigned int i, j, rate;
	fada_Res normal;

	const long long* samples = (long long*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_i64(m);
	normal = fada_getnormalizer(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < m->fft.buffer->size; ++i, j += m->channels)
	{
		fft[2*i] = (j + chan < m->window.size) ? (samples[j + chan] / normal) : 0.;
		fft[2*i+1] = 0.;
	}
	
	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_channel_f32(fada_Manager* m, unsigned int chan)
{
	unsigned int i, j, rate;

	const float* samples = (float*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_f32(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < m->fft.buffer->size; ++i, j += m->channels)
	{
		fft[2*i] = (j + chan < m->window.size) ? samples[j + chan] : 0.;
		fft[2*i+1] = 0.;
	}
	
	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_channel_f64(fada_Manager* m, unsigned int chan)
{
	unsigned int i, j, rate;

	const double* samples = (double*)m->window.buffer;
	fada_Res* fft = m->fft.buffer->buffer;

	fada_fillwindowbuffer_f64(m);

	rate = m->fft.buffer->size;

	for (i = 0, j = 0; i < m->fft.buffer->size; ++i, j += m->channels)
	{
		fft[2*i] = (j + chan < m->window.size) ? samples[j + chan] : 0.;
		fft[2*i+1] = 0.;
	}
	
	fada_calcfft_master(fft, rate);
}


//////////////////////////////////////////////////
void fada_calcfft_master(fada_Res* fft, unsigned int rate)
{
	unsigned int n, mmax, mm;
	unsigned int i, j, is;
	double wtemp, wr, wpr, wpi, wi, theta;
	fada_Res tempi, tempr;

	n = rate << 1;
	j = 0;
	mmax = n / 2;
	for (i = 0; i < mmax; i += 2)
	{
		if (j > i)
		{
			_FADA_FFT_SWAP(fft[j], fft[i]);
			_FADA_FFT_SWAP(fft[j+1], fft[i+1]);
			if ((j/2) < (n/4))
			{
				_FADA_FFT_SWAP(fft[(n-(i+2))], fft[(n-(j+2))]);
				_FADA_FFT_SWAP(fft[(n-(i+2))+1], fft[(n-(j+2))+1]);
			}
		}
		mm = n >> 1;
		while (mm >= 2 && j >= mm)
		{
			j -= mm;
			mm >>= 1;
		}
		j += mm;
	}
	
	mmax = 2;
	while (n > mmax)
	{
		is = mmax << 1;
		theta = (2.*_FADA_PI/mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.;
		wi = 0.;
		for (mm = 1; mm < mmax; mm += 2)
		{
			for (i = mm; i <= n; i += is)
			{
				j = i + mmax;

				tempr = wr*fft[j-1]-wi*fft[j];
				tempi = wr*fft[j]+wi*fft[j-1];

				fft[j-1] = fft[i-1]-tempr;
				fft[j] = fft[i]-tempi;
				fft[i-1] += tempr;
				fft[i] += tempi;
			}
			wr = (wtemp=wr)*wpr - wi*wpi + wr;
			wi = wi*wpr + wtemp*wpi + wi;
		}
		mmax = is;
	}
}