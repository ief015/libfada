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

//////////////////////////////////////////////////
/// \file fada_def.h
/// \brief Definitions header.
/// 

#ifndef _FADA_DEF_H
#define _FADA_DEF_H

/// \cond
#ifdef FADA_STATIC
  #define FADA_API
#else
  #ifdef FADA_BUILD
  	#define FADA_API __declspec(dllexport)
  #else
  	#define FADA_API __declspec(dllimport)
  #endif
#endif

#ifndef NULL
#define NULL 0
#endif
/// \endcond

//////////////////////////////////////////////////
/// \def FADA_VERSION_MAJOR
/// \brief Major libfada version number.
#define FADA_VERSION_MAJOR 0

//////////////////////////////////////////////////
/// \def FADA_VERSION_MINOR
/// \brief Minor libfada version number.
#define FADA_VERSION_MINOR 1

//////////////////////////////////////////////////
/// \typedef fada_Boolean
/// \brief Boolean true/false type used by libfada.
/// 
/// Uses only \ref FADA_TRUE or \ref FADA_FALSE.
typedef char fada_Boolean;
#define FADA_FALSE  0 /**< \brief \c fada_Boolean: False */
#define FADA_TRUE   1 /**< \brief \c fada_Boolean: True */

//////////////////////////////////////////////////
/// \typedef fada_TSample
/// \brief Sample type identifier.
/// 
/// Uses the enumeration type \c FADA_TSAMPLE_*
typedef int fada_TSample;
#define FADA_TSAMPLE_NOTSET   0 /**< \brief \c FADA_TSAMPLE: Sample type not set. */
#define FADA_TSAMPLE_INT8     1 /**< \brief \c FADA_TSAMPLE: Integer 8-bit sample type. */
#define FADA_TSAMPLE_INT16    2 /**< \brief \c FADA_TSAMPLE: Integer 16-bit sample type. */
#define FADA_TSAMPLE_INT32    3 /**< \brief \c FADA_TSAMPLE: Integer 32-bit sample type. */
#define FADA_TSAMPLE_INT64    4 /**< \brief \c FADA_TSAMPLE: Integer 64-bit sample type. */
#define FADA_TSAMPLE_FLOAT32  5 /**< \brief \c FADA_TSAMPLE: Floating-point 32-bit sample type. */
#define FADA_TSAMPLE_FLOAT64  6 /**< \brief \c FADA_TSAMPLE: Floating-point 64-bit sample type. */

//////////////////////////////////////////////////
/// \typedef fada_Error
/// \brief Error type related to libfada.
/// 
/// Uses the enumeration type \c FADA_ERROR_*
typedef int fada_Error;
#define FADA_ERROR_SUCCESS                   0 /**< \brief \c FADA_ERROR: Operation was successful. Not actually an error. */
#define FADA_ERROR_INVALID_MANAGER           1 /**< \brief \c FADA_ERROR: Passed an invalid manager. */
#define FADA_ERROR_INVALID_PARAMETER         2 /**< \brief \c FADA_ERROR: Passed an invalid parameter. Typically returned if an output parameter was NULL. */
#define FADA_ERROR_INVALID_TYPE              3 /**< \brief \c FADA_ERROR: Manager does not have a valid sample type bound to it. */
#define FADA_ERROR_INVALID_SIZE              4 /**< \brief \c FADA_ERROR: Passed an invalid size. */
#define FADA_ERROR_INVALID_SAMPLE_RATE       5 /**< \brief \c FADA_ERROR: Passed an invalid sample rate. */
#define FADA_ERROR_INVALID_CHANNEL           6 /**< \brief \c FADA_ERROR: Passed an invalid channel index. */
#define FADA_ERROR_INVALID_FFT_BUFFER        7 /**< \brief \c FADA_ERROR: Passed an invalid FFT buffer. */
#define FADA_ERROR_MANAGER_NOT_READY         8 /**< \brief \c FADA_ERROR: Manager is not ready yet. Did you bind sample/stream information to it? */
#define FADA_ERROR_NO_DATA                   9 /**< \brief \c FADA_ERROR: No data was bound to the manager. */
#define FADA_ERROR_NOT_MULTIPLE_OF_CHANNELS  10 /**< \brief \c FADA_ERROR: Value was not a multiple of the channel count. */
#define FADA_ERROR_NOT_ENOUGH_MEMORY         11 /**< \brief \c FADA_ERROR: Not enough memory. */
#define FADA_ERROR_INDEX_OUT_OF_BOUNDS       12 /**< \brief \c FADA_ERROR: Index was out of bounds. */
#define FADA_ERROR_POSITION_OUT_OF_BOUNDS    13 /**< \brief \c FADA_ERROR: Position was out of bounds. */
#define FADA_ERROR_FREQUENCY_OUT_OF_BOUNDS   14 /**< \brief \c FADA_ERROR: Frequency was out of the valid frequency range. */
#define FADA_ERROR_WINDOW_NOT_CREATED        15 /**< \brief \c FADA_ERROR: Manager does not have a window buffer created. */

//////////////////////////////////////////////////
/// \typedef fada_Pos
/// \brief Unsigned integral type used for position and size related values.
typedef unsigned int fada_Pos;

//////////////////////////////////////////////////
/// \def FADA_DEFAULT_WINDOWSIZE
/// \brief Default analysis window size used if no window size was set.
#define FADA_DEFAULT_WINDOWSIZE 1024U

//////////////////////////////////////////////////
/// \def FADA_NEXT_WINDOW
/// \brief Used in fada_continue to instruct libfada to advance to the next window.
/// 
/// Can be multiplied to advance over multiple windows.
/// For example, <tt>\ref fada_continue(m, \ref FADA_NEXT_WINDOW * 2)</tt> will advance two window lengths.
/// 
/// \see fada_continue
#define FADA_NEXT_WINDOW -1L

//////////////////////////////////////////////////
/// \typedef fada_Res
/// \brief Floating-point result type.
typedef double fada_Res;

//////////////////////////////////////////////////
/// \typedef fada_Manager
/// \brief Manages audio information and audio data to allow analyzation.
/// 
/// This is basically the most important type in libfada.
typedef struct fada_Manager fada_Manager;

//////////////////////////////////////////////////
/// \typedef fada_FFTBuffer
/// \brief Stores generated FFT data.
typedef struct fada_FFTBuffer fada_FFTBuffer;

#endif