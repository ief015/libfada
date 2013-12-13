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

#ifndef _FADA_CALC_H
#define _FADA_CALC_H

#include <fada/fada_def.h>


void fada_fillwindowbuffer_i8(fada_Manager* m);
void fada_fillwindowbuffer_i16(fada_Manager* m);
void fada_fillwindowbuffer_i32(fada_Manager* m);
void fada_fillwindowbuffer_i64(fada_Manager* m);
void fada_fillwindowbuffer_f32(fada_Manager* m);
void fada_fillwindowbuffer_f64(fada_Manager* m);

fada_Res fada_getsample_i8(fada_Manager* m, fada_Pos pos);
fada_Res fada_getsample_i16(fada_Manager* m, fada_Pos pos);
fada_Res fada_getsample_i32(fada_Manager* m, fada_Pos pos);
fada_Res fada_getsample_i64(fada_Manager* m, fada_Pos pos);
fada_Res fada_getsample_f32(fada_Manager* m, fada_Pos pos);
fada_Res fada_getsample_f64(fada_Manager* m, fada_Pos pos);

void fada_getsamples_i8(fada_Manager* m, unsigned int channel, fada_Res* out_results);
void fada_getsamples_i16(fada_Manager* m, unsigned int channel, fada_Res* out_results);
void fada_getsamples_i32(fada_Manager* m, unsigned int channel, fada_Res* out_results);
void fada_getsamples_i64(fada_Manager* m, unsigned int channel, fada_Res* out_results);
void fada_getsamples_f32(fada_Manager* m, unsigned int channel, fada_Res* out_results);
void fada_getsamples_f64(fada_Manager* m, unsigned int channel, fada_Res* out_results);

fada_Res fada_getframe_i8(fada_Manager* m, fada_Pos pos);
fada_Res fada_getframe_i16(fada_Manager* m, fada_Pos pos);
fada_Res fada_getframe_i32(fada_Manager* m, fada_Pos pos);
fada_Res fada_getframe_i64(fada_Manager* m, fada_Pos pos);
fada_Res fada_getframe_f32(fada_Manager* m, fada_Pos pos);
fada_Res fada_getframe_f64(fada_Manager* m, fada_Pos pos);

void fada_getframes_i8(fada_Manager* m, fada_Res* out_results);
void fada_getframes_i16(fada_Manager* m, fada_Res* out_results);
void fada_getframes_i32(fada_Manager* m, fada_Res* out_results);
void fada_getframes_i64(fada_Manager* m, fada_Res* out_results);
void fada_getframes_f32(fada_Manager* m, fada_Res* out_results);
void fada_getframes_f64(fada_Manager* m, fada_Res* out_results);

fada_Res fada_calcbeat_i8(fada_Manager* m);
fada_Res fada_calcbeat_i16(fada_Manager* m);
fada_Res fada_calcbeat_i32(fada_Manager* m);
fada_Res fada_calcbeat_i64(fada_Manager* m);
fada_Res fada_calcbeat_f32(fada_Manager* m);
fada_Res fada_calcbeat_f64(fada_Manager* m);

fada_Res fada_calcbeat_channel_i8(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbeat_channel_i16(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbeat_channel_i32(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbeat_channel_i64(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbeat_channel_f32(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbeat_channel_f64(fada_Manager* m, unsigned int chan);

fada_Res fada_calcbass_i8(fada_Manager* m);
fada_Res fada_calcbass_i16(fada_Manager* m);
fada_Res fada_calcbass_i32(fada_Manager* m);
fada_Res fada_calcbass_i64(fada_Manager* m);
fada_Res fada_calcbass_f32(fada_Manager* m);
fada_Res fada_calcbass_f64(fada_Manager* m);

fada_Res fada_calcbass_channel_i8(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbass_channel_i16(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbass_channel_i32(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbass_channel_i64(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbass_channel_f32(fada_Manager* m, unsigned int chan);
fada_Res fada_calcbass_channel_f64(fada_Manager* m, unsigned int chan);

void fada_calcfft_i8(fada_Manager* m);
void fada_calcfft_i16(fada_Manager* m);
void fada_calcfft_i32(fada_Manager* m);
void fada_calcfft_i64(fada_Manager* m);
void fada_calcfft_f32(fada_Manager* m);
void fada_calcfft_f64(fada_Manager* m);

void fada_calcfft_channel_i8(fada_Manager* m, unsigned int chan);
void fada_calcfft_channel_i16(fada_Manager* m, unsigned int chan);
void fada_calcfft_channel_i32(fada_Manager* m, unsigned int chan);
void fada_calcfft_channel_i64(fada_Manager* m, unsigned int chan);
void fada_calcfft_channel_f32(fada_Manager* m, unsigned int chan);
void fada_calcfft_channel_f64(fada_Manager* m, unsigned int chan);

void fada_calcfft_master(fada_Res* fft, unsigned int rate);

#endif