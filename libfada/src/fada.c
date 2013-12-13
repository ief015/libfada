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


//////////////////////////////////////////////////
fada_Error fada_getsample(fada_Manager* m, fada_Pos pos, unsigned int channel, fada_Res* out_result)
{
	if (!out_result)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (channel >= m->channels)
		return FADA_ERROR_INVALID_CHANNEL;

	if (!m->current_chunk)
	{
		(*out_result) = 0.;
		return FADA_ERROR_SUCCESS;
	}

	pos *= m->channels;

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    (*out_result) = fada_getsample_i8(m, pos + channel);  break;
		case FADA_TSAMPLE_INT16:   (*out_result) = fada_getsample_i16(m, pos + channel); break;
		case FADA_TSAMPLE_INT32:   (*out_result) = fada_getsample_i32(m, pos + channel); break;
		case FADA_TSAMPLE_INT64:   (*out_result) = fada_getsample_i64(m, pos + channel); break;
		case FADA_TSAMPLE_FLOAT32: (*out_result) = fada_getsample_f32(m, pos + channel); break;
		case FADA_TSAMPLE_FLOAT64: (*out_result) = fada_getsample_f64(m, pos + channel); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_getsamples(fada_Manager* m, unsigned int channel, fada_Res* out_results)
{
	if (!out_results)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (channel >= m->channels)
		return FADA_ERROR_INVALID_CHANNEL;

	if (!m->current_chunk)
	{
		(*out_results) = 0.;
		return FADA_ERROR_SUCCESS;
	}
	
	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    fada_getsamples_i8(m, channel, out_results);  break;
		case FADA_TSAMPLE_INT16:   fada_getsamples_i16(m, channel, out_results); break;
		case FADA_TSAMPLE_INT32:   fada_getsamples_i32(m, channel, out_results); break;
		case FADA_TSAMPLE_INT64:   fada_getsamples_i64(m, channel, out_results); break;
		case FADA_TSAMPLE_FLOAT32: fada_getsamples_f32(m, channel, out_results); break;
		case FADA_TSAMPLE_FLOAT64: fada_getsamples_f64(m, channel, out_results); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_getframe(fada_Manager* m, fada_Pos pos, fada_Res* out_result)
{
	if (!out_result)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (!m->current_chunk)
	{
		(*out_result) = 0.;
		return FADA_ERROR_SUCCESS;
	}

	pos *= m->channels;

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    (*out_result) = fada_getframe_i8(m, pos);  break;
		case FADA_TSAMPLE_INT16:   (*out_result) = fada_getframe_i16(m, pos); break;
		case FADA_TSAMPLE_INT32:   (*out_result) = fada_getframe_i32(m, pos); break;
		case FADA_TSAMPLE_INT64:   (*out_result) = fada_getframe_i64(m, pos); break;
		case FADA_TSAMPLE_FLOAT32: (*out_result) = fada_getframe_f32(m, pos); break;
		case FADA_TSAMPLE_FLOAT64: (*out_result) = fada_getframe_f64(m, pos); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_getframes(fada_Manager* m, fada_Res* out_results)
{
	if (!out_results)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (!m->current_chunk)
	{
		(*out_results) = 0.;
		return FADA_ERROR_SUCCESS;
	}
	
	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    fada_getframes_i8(m, out_results);  break;
		case FADA_TSAMPLE_INT16:   fada_getframes_i16(m, out_results); break;
		case FADA_TSAMPLE_INT32:   fada_getframes_i32(m, out_results); break;
		case FADA_TSAMPLE_INT64:   fada_getframes_i64(m, out_results); break;
		case FADA_TSAMPLE_FLOAT32: fada_getframes_f32(m, out_results); break;
		case FADA_TSAMPLE_FLOAT64: fada_getframes_f64(m, out_results); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_calcbeat(fada_Manager* m, fada_Res* out_result)
{
	if (!out_result)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (!m->current_chunk)
	{
		(*out_result) = 0.;
		return FADA_ERROR_SUCCESS;
	}

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    (*out_result) = fada_calcbeat_i8(m);  break;
		case FADA_TSAMPLE_INT16:   (*out_result) = fada_calcbeat_i16(m); break;
		case FADA_TSAMPLE_INT32:   (*out_result) = fada_calcbeat_i32(m); break;
		case FADA_TSAMPLE_INT64:   (*out_result) = fada_calcbeat_i64(m); break;
		case FADA_TSAMPLE_FLOAT32: (*out_result) = fada_calcbeat_f32(m); break;
		case FADA_TSAMPLE_FLOAT64: (*out_result) = fada_calcbeat_f64(m); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_calcbeat_channel(fada_Manager* m, unsigned int channel, fada_Res* out_result)
{
	if (!out_result)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (channel >= m->channels)
		return FADA_ERROR_INVALID_CHANNEL;

	if (!m->current_chunk)
	{
		(*out_result) = 0.;
		return FADA_ERROR_SUCCESS;
	}

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    (*out_result) = fada_calcbeat_channel_i8(m, channel);  break;
		case FADA_TSAMPLE_INT16:   (*out_result) = fada_calcbeat_channel_i16(m, channel); break;
		case FADA_TSAMPLE_INT32:   (*out_result) = fada_calcbeat_channel_i32(m, channel); break;
		case FADA_TSAMPLE_INT64:   (*out_result) = fada_calcbeat_channel_i64(m, channel); break;
		case FADA_TSAMPLE_FLOAT32: (*out_result) = fada_calcbeat_channel_f32(m, channel); break;
		case FADA_TSAMPLE_FLOAT64: (*out_result) = fada_calcbeat_channel_f64(m, channel); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_calcbass(fada_Manager* m, fada_Res* out_result)
{
	if (!out_result)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (!m->current_chunk)
	{
		(*out_result) = 0.;
		return FADA_ERROR_SUCCESS;
	}

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    (*out_result) = fada_calcbass_i8(m);  break;
		case FADA_TSAMPLE_INT16:   (*out_result) = fada_calcbass_i16(m); break;
		case FADA_TSAMPLE_INT32:   (*out_result) = fada_calcbass_i32(m); break;
		case FADA_TSAMPLE_INT64:   (*out_result) = fada_calcbass_i64(m); break;
		case FADA_TSAMPLE_FLOAT32: (*out_result) = fada_calcbass_f32(m); break;
		case FADA_TSAMPLE_FLOAT64: (*out_result) = fada_calcbass_f64(m); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_calcbass_channel(fada_Manager* m, unsigned int channel, fada_Res* out_result)
{
	if (!out_result)
		return FADA_ERROR_INVALID_PARAMETER;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (channel >= m->channels)
		return FADA_ERROR_INVALID_CHANNEL;

	if (!m->current_chunk)
	{
		(*out_result) = 0.;
		return FADA_ERROR_SUCCESS;
	}

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    (*out_result) = fada_calcbass_channel_i8(m, channel);  break;
		case FADA_TSAMPLE_INT16:   (*out_result) = fada_calcbass_channel_i16(m, channel); break;
		case FADA_TSAMPLE_INT32:   (*out_result) = fada_calcbass_channel_i32(m, channel); break;
		case FADA_TSAMPLE_INT64:   (*out_result) = fada_calcbass_channel_i64(m, channel); break;
		case FADA_TSAMPLE_FLOAT32: (*out_result) = fada_calcbass_channel_f32(m, channel); break;
		case FADA_TSAMPLE_FLOAT64: (*out_result) = fada_calcbass_channel_f64(m, channel); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_calcfft(fada_Manager* m)
{
	fada_Error err = fada_preloadfftbuffer(m);
	if (err != FADA_ERROR_SUCCESS)
		return err;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (!m->current_chunk)
		return FADA_ERROR_SUCCESS;

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    fada_calcfft_i8(m);  break;
		case FADA_TSAMPLE_INT16:   fada_calcfft_i16(m); break;
		case FADA_TSAMPLE_INT32:   fada_calcfft_i32(m); break;
		case FADA_TSAMPLE_INT64:   fada_calcfft_i64(m); break;
		case FADA_TSAMPLE_FLOAT32: fada_calcfft_f32(m); break;
		case FADA_TSAMPLE_FLOAT64: fada_calcfft_f64(m); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}


//////////////////////////////////////////////////
fada_Error fada_calcfft_channel(fada_Manager* m, unsigned int channel)
{
	fada_Error err;

	err = fada_preloadfftbuffer(m);
	if (err != FADA_ERROR_SUCCESS)
		return err;
	
	if (!m->window.buffer)
		return FADA_ERROR_WINDOW_NOT_CREATED;

	if (channel >= m->channels)
		return FADA_ERROR_INVALID_CHANNEL;

	if (!m->current_chunk)
		return FADA_ERROR_SUCCESS;

	switch (m->sample_type)
	{
		case FADA_TSAMPLE_INT8:    fada_calcfft_channel_i8(m, channel);  break;
		case FADA_TSAMPLE_INT16:   fada_calcfft_channel_i16(m, channel); break;
		case FADA_TSAMPLE_INT32:   fada_calcfft_channel_i32(m, channel); break;
		case FADA_TSAMPLE_INT64:   fada_calcfft_channel_i64(m, channel); break;
		case FADA_TSAMPLE_FLOAT32: fada_calcfft_channel_f32(m, channel); break;
		case FADA_TSAMPLE_FLOAT64: fada_calcfft_channel_f64(m, channel); break;
		default: return FADA_ERROR_INVALID_TYPE;
	}

	return FADA_ERROR_SUCCESS;
}