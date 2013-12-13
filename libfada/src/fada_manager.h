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

#ifndef _FADA_MANAGER_H
#define _FADA_MANAGER_H

#include <fada/fada_def.h>
#include "fada_chunk.h"


struct fada_Manager
{
	fada_Chunk* current_chunk;
	fada_Chunk* first_chunk;
	fada_Chunk* last_chunk;
	
	fada_Pos current_sample;
	fada_Pos sample_count;
	fada_TSample sample_type;

	unsigned int channels;
	unsigned int sample_rate;

	struct
	{
		void* buffer;
		fada_Pos size;
		fada_Boolean filled;
	} window;

	struct
	{
		fada_FFTBuffer* buffer;
		fada_Boolean internal;
	} fft;

	fada_Boolean ready;
};

#endif