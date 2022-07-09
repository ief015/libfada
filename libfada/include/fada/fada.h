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
/// \file fada.h
/// \brief Main API header for libfada.
/// 
 
#ifndef _FADA_H
#define _FADA_H

#include "fada_def.h"


//////////////////////////////////////////////////
// Audio Management
//////////////////////////////////////////////////


//////////////////////////////////////////////////
/// \brief Create a new libfada manager.
/// Returns NULL if manager could not be created.
/// 
/// \return Returns a new libfada audio manager.
/// 
/// \see fada_closemanager
FADA_API fada_Manager* fada_newmanager();

//////////////////////////////////////////////////
/// \brief Close an existing libfada audio manager, freeing its resources.
/// 
/// \param m The manager to close.
/// 
/// \see fada_newmanager
FADA_API void fada_closemanager(fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Bind initial audio data and audio information to the manager.
/// 
/// If successful, the manager will be ready to use for analysis.
/// 
/// \param m The manager to bind.
/// \param data Pointer to the audio data.
/// \param sample_count The number of samples stored in data.
/// \param copy_data Copy the data being bound to the manager. Set to \ref FADA_FALSE to reference the data instead.
/// \param sample_type The sample type of audio data.
/// \param sample_rate The sample rate of the audio data in Hertz. For example, \c 44100.
/// \param channels The number of audio channels. For example, \c 1 would be mono audio, \c 2 would be stereo audio, etc.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_CHANNEL
///         \li \ref FADA_ERROR_INVALID_SAMPLE_RATE
///         \li \ref FADA_ERROR_INVALID_SIZE
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_NO_DATA
///         \li \ref FADA_ERROR_NOT_ENOUGH_MEMORY
///         \li \ref FADA_ERROR_NOT_MULTIPLE_OF_CHANNELS
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_bindstream
/// \see fada_pushsamples
FADA_API fada_Error fada_bindsamples(fada_Manager* m, void* data, fada_Pos sample_count, fada_Boolean copy_data, fada_TSample sample_type, unsigned int sample_rate, unsigned int channels);

//////////////////////////////////////////////////
/// \brief Bind only audio information to the manager.
/// 
/// Similar to fada_bindsamples, except no samples are bound to the manager until \ref fada_pushsamples is called.
/// If successful, the manager will be ready to use for analysis.
/// 
/// \param m The manager to bind.
/// \param sample_type The sample type of audio data.
/// \param sample_rate The sample rate of the audio data in Hertz. For example, \c 44100.
/// \param channels The number of audio channels. For example, \c 1 would be mono audio, \c 2 would be stereo audio, etc.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_CHANNEL
///         \li \ref FADA_ERROR_INVALID_SAMPLE_RATE
///         \li \ref FADA_ERROR_INVALID_SIZE
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_NOT_ENOUGH_MEMORY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_bindsamples
/// \see fada_pushsamples
FADA_API fada_Error fada_bindstream(fada_Manager* m, fada_TSample sample_type, unsigned int sample_rate, unsigned int channels);

//////////////////////////////////////////////////
/// \brief Push a new chunk of samples to the end of the audio manager.
/// 
/// For the sake of performance, it is recommended to keep chunks at least the size of the analyzation window, but even larger is more preferable.
/// The optimal size of streaming chunks is 1 second of audio data or larger.
/// 
/// \param m The manager to give samples.
/// \param data Pointer to the audio data.
/// \param sample_count The number of samples stored in data.
/// \param copy_data Copy the data being bound to the manager. Set to \ref FADA_FALSE to reference the data instead.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_SIZE
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_NO_DATA
///         \li \ref FADA_ERROR_NOT_ENOUGH_MEMORY
///         \li \ref FADA_ERROR_NOT_MULTIPLE_OF_CHANNELS
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_bindsamples
/// \see fada_bindstream
FADA_API fada_Error fada_pushsamples(fada_Manager* m, void* data, fada_Pos sample_count, fada_Boolean copy_data);

//////////////////////////////////////////////////
/// \brief Free audio chunk data before the current "in-use" chunk from the manager.
/// 
/// This will make the current chunk become the "first" chunk.
/// 
/// \param m The manager to trim.
/// 
/// \see fada_freechunks
FADA_API void fada_trimchunks(fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Clears all audio chunk data from the manager.
/// 
/// \param m The manager to clear.
/// 
/// \see fada_trimchunks
FADA_API void fada_freechunks(fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the sample type bound to the manager.
/// 
/// \param m The manager.
/// 
/// \return Returns sample type.
FADA_API fada_TSample fada_getsampletype(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the sample rate bound to the manager.
/// 
/// \param m The manager.
/// 
/// \return Returns sample rate in Hertz.
FADA_API unsigned int fada_getsamplerate(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the total number of samples bound to the manager.
/// 
/// \param m The manager.
/// 
/// \return Returns sample count.
FADA_API unsigned int fada_getsamplecount(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the total number of frames bound to the manager.
/// 
/// This is the same as <tt>sample count / channel count</tt>.
/// 
/// \param m The manager.
/// 
/// \return Returns frame count.
FADA_API unsigned int fada_getframecount(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the number of audio channels bound to the manager.
/// 
/// \param m The manager.
/// 
/// \return Returns channel count.
FADA_API unsigned int fada_getchannels(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the position of the analysis window (in frames) relative to the beginning of sample data.
/// 
/// \param m The manager.
/// 
/// \return Returns analysis window position in frames.
/// 
/// \see fada_setposition
FADA_API fada_Pos fada_getposition(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the window size (in samples) that is analyzed during calculations.
/// 
/// \param m The manager.
/// 
/// \return Returns analysis window size in samples.
/// 
/// \see fada_getwindowframes
/// \see fada_setwindowframes
FADA_API fada_Pos fada_getwindowsize(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the window size (in frames) that is analyzed during calculations.
/// 
/// \param m The manager.
/// 
/// \return Returns analysis window size in frames.
/// 
/// \see fada_getwindowsize
/// \see fada_setwindowframes
FADA_API fada_Pos fada_getwindowframes(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Set the position of the analysis window (in frames) relative to the beginning of sample data.
/// 
/// \param m The manager.
/// \param pos The position in frames. Must be less than the manager's <tt>frame count</tt>.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_POSITION_OUT_OF_BOUNDS
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getposition
FADA_API fada_Error fada_setposition(fada_Manager* m, fada_Pos pos);

//////////////////////////////////////////////////
/// \brief Set the window size (in frames) that is analyzed during calculations.
/// 
/// Default is 1024, see \ref FADA_DEFAULT_WINDOWSIZE.
/// Typically, this is set to a power of 2 (Ex: 1024, 2048, etc.), but is not required to be. See \ref fada_preloadfftbuffer for more information.
/// 
/// \param m The manager.
/// \param frames The size in frames.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_SIZE
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_NOT_ENOUGH_MEMORY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getwindowsize
/// \see fada_getwindowframes
FADA_API fada_Error fada_setwindowframes(fada_Manager* m, fada_Pos frames);

//////////////////////////////////////////////////
/// \brief Advance the analysis window position ahead.
/// 
/// If \c offset_frames is below \c 0 or \ref FADA_NEXT_WINDOW, it will automatically continue to the next window of samples.
/// You may also multiply \ref FADA_NEXT_WINDOW to skip windows. For example, <tt>\ref fada_continue(m, \ref FADA_NEXT_WINDOW * 2)</tt> will jump two windows ahead, stopping at the window after the next.
/// 
/// \param m The manager to advance.
/// \param offset_frames Number of frames to advance. Can be negative to skip over whole windows.
/// 
/// \return Boolean indicating whether the end of audio data has not been reached.
///         \ref FADA_FALSE if end of audio has been reached.
/// 
/// \see fada_continuetolast
/// \see fada_endofaudio
FADA_API fada_Boolean fada_continue(fada_Manager* m, long offset_frames);

//////////////////////////////////////////////////
/// \brief Advance the analysis window to the last full available window of audio samples bound to the manager.
/// 
/// \param m The manager to advance.
/// 
/// \return Boolean indicating whether the operation was successful.
/// 
/// \see fada_continue
/// \see fada_endofaudio
FADA_API fada_Boolean fada_continuetolast(fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Check if the analysis window position has surpassed the end of audio samples bound to the manager.
/// 
/// \param m The manager.
/// 
/// \return Boolean indicating whether the end of audio has been reached.
/// 
/// \see fada_continue
/// \see fada_continuetolast
FADA_API fada_Boolean fada_endofaudio(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the factor for normalizing results so that results can be uniform among different sample data types.
/// 
/// \param m The manager.
/// 
/// \return Boolean indicating whether the end of audio has been reached.
FADA_API fada_Res fada_getnormalizer(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Preallocate and use the internal FFT buffer.
/// 
/// If the manager doesn't have an FFT buffer already assigned to it, this function is automatically called when an FFT buffer is needed.
/// You can manually call this function to use the internal FFT buffer during application initialization, so libfada won't need to call this automatically.
/// Internal FFT buffer size will be the frame count of the manager's analysis window. If the window's frame count is not a power of 2, it will use the closest power of 2 less than the window's frame count.
/// 
/// \note It is important to remember that this function will not work as intended if an FFT buffer is still assigned to the manager (via \ref fada_usefftbuffer). You must unassign the buffer first: <tt>\ref fada_usefftbuffer (m, NULL)</tt>
/// 
/// \param m The manager.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_NOT_ENOUGH_MEMORY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_usefftbuffer
FADA_API fada_Error fada_preloadfftbuffer(fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Assign an external FFT buffer to the manager.
/// 
/// If \c b is \c NULL, it will unassign an FFT buffer. This is useful if you wish to use the internal FFT buffer. See \ref fada_preloadfftbuffer.
/// 
/// \param m The manager.
/// \param b The FFT buffer to use.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_newfftbuffer
/// \see fada_preloadfftbuffer
FADA_API fada_Error fada_usefftbuffer(fada_Manager* m, fada_FFTBuffer* b);

//////////////////////////////////////////////////
/// \brief Retrieve the raw FFT data from the FFT buffer in use.
/// 
/// Returned FFT data is an array of fada_Res, with a length of <tt>\ref fada_getfftsize * 2</tt>.
/// 
/// \param m The manager.
/// 
/// \return Returns array with raw FFT data.
/// 
/// \see fada_getfftsize
/// \see fada_getfftvalue
/// \see fada_getfftvalues
/// \see fada_getfftvaluesrange
/// \see fada_getfftvaluefromfrequency
FADA_API const fada_Res* fada_getfft(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the size of the FFT buffer in use.
/// 
/// \param m The manager.
/// 
/// \return Returns length of the FFT buffer.
/// 
/// \see fada_getfft
/// \see fada_getfftvalue
/// \see fada_getfftvalues
/// \see fada_getfftvaluesrange
/// \see fada_getfftvaluefromfrequency
FADA_API fada_Pos fada_getfftsize(const fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Retrieve the value at the position from the FFT buffer in use.
/// 
/// The FFT must be calculated before getting values from it. See \ref fada_calcfft.
/// 
/// \param m The manager.
/// \param pos The position on the FFT buffer in use. Must be less than \ref fada_getfftsize.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INDEX_OUT_OF_BOUNDS
///         \li \ref FADA_ERROR_INVALID_FFT_BUFFER
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getfft
/// \see fada_getfftsize
/// \see fada_getfftvalues
/// \see fada_getfftvaluesrange
/// \see fada_getfftvaluefromfrequency
/// \see fada_calcfft
/// \see fada_calcfft_channel
FADA_API fada_Error fada_getfftvalue(const fada_Manager* m, fada_Pos pos, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Retrieve all values from the FFT buffer in use.
/// 
/// The FFT must be calculated before getting values from it. See \ref fada_calcfft.
/// 
/// \param m The manager.
/// \param out_results Destination to write the results. Destination is an array with a length of at least \ref fada_getfftsize.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_FFT_BUFFER
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getfft
/// \see fada_getfftsize
/// \see fada_getfftvalue
/// \see fada_getfftvaluesrange
/// \see fada_getfftvaluefromfrequency
/// \see fada_calcfft
/// \see fada_calcfft_channel
FADA_API fada_Error fada_getfftvalues(const fada_Manager* m, fada_Res* out_results);

//////////////////////////////////////////////////
/// \brief Retrieve all values in range from the FFT buffer in use.
/// 
/// The FFT must be calculated before getting values from it. See \ref fada_calcfft.
/// \c len should not be any greater than <tt>\ref fada_getfftsize - offset</tt>. If \c len is \c 0, it will grab all values from \c offset to the end of the FFT.
/// 
/// \param m The manager.
/// \param out_results Destination to write the results. Destination is an array with a length of at least \c len.
/// \param offset Where on the FFT buffer in use to begin retrieving values.
/// \param len How many values to retrieve.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INDEX_OUT_OF_BOUNDS
///         \li \ref FADA_ERROR_INVALID_FFT_BUFFER
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getfft
/// \see fada_getfftsize
/// \see fada_getfftvalue
/// \see fada_getfftvalues
/// \see fada_getfftvaluefromfrequency
/// \see fada_calcfft
/// \see fada_calcfft_channel
FADA_API fada_Error fada_getfftvaluesrange(const fada_Manager* m, fada_Res* out_results, fada_Pos offset, fada_Pos len);

//////////////////////////////////////////////////
/// \brief Retrieve the closest value at a specified frequency (in Hertz) from the FFT buffer in use.
/// 
/// The FFT must be calculated before getting values from it. See \ref fada_calcfft.
/// 
/// \param m The manager.
/// \param freq The frequency in Hertz (Hz), and must be less than half of the audio sample rate.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_FREQUENCY_OUT_OF_BOUNDS
///         \li \ref FADA_ERROR_INDEX_OUT_OF_BOUNDS
///         \li \ref FADA_ERROR_INVALID_FFT_BUFFER
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getfft
/// \see fada_getfftsize
/// \see fada_getfftvalue
/// \see fada_getfftvalues
/// \see fada_getfftvaluesrange
/// \see fada_calcfft
/// \see fada_calcfft_channel
FADA_API fada_Error fada_getfftvaluefromfrequency(const fada_Manager* m, fada_Res freq, fada_Res* out_result);


//////////////////////////////////////////////////
// FFT buffers
//////////////////////////////////////////////////


//////////////////////////////////////////////////
/// \brief Create a new external buffer.
/// 
/// \c size_po2 must be a power of 2. If it is not a power of 2, it will use the closest power of 2 less than \c size_po2.
/// FFT buffers allocate <tt>sizeof(\ref fada_Res) * size_po2 * 2</tt> of memory, which is twice more memory than one may expect. \c size_po2 only defines the size of the "useful" half of the FFT.
/// 
/// \param size_po2 The size of the new FFT buffer.
/// 
/// \return Returns a new FFT buffer.
/// 
/// \see fada_closefftbuffer
FADA_API fada_FFTBuffer* fada_newfftbuffer(fada_Pos size_po2);

//////////////////////////////////////////////////
/// \brief Close an existing FFT buffer, freeing its resources.
/// 
/// If a manager is still using the buffer, make sure you call <tt>\ref fada_usefftbuffer (m, NULL or anotherBuffer)</tt> to unassign the buffer from that manager.
/// Optionally, you may also call \ref fada_preloadfftbuffer after assigning NULL to the manager.
/// 
/// \param b The FFT buffer to close.
/// 
/// \see fada_newfftbuffer
FADA_API void fada_closefftbuffer(fada_FFTBuffer* b);

//////////////////////////////////////////////////
/// \brief Retrieve the raw FFT data from the FFT buffer.
/// 
/// Returned FFT data is an array of \ref fada_Res, with a length of <tt>\ref fada_getfftsize_buffer * 2</tt>.
/// 
/// \param b The FFT buffer.
/// 
/// \return Returns array with raw FFT data.
/// 
/// \see fada_getfftsize_buffer
/// \see fada_getfftvalue_buffer
/// \see fada_getfftvalues_buffer
/// \see fada_getfftvaluesrange_buffer
FADA_API const fada_Res* fada_getfft_buffer(const fada_FFTBuffer* b);

//////////////////////////////////////////////////
/// \brief Retrieve the size of the FFT buffer.
/// 
/// \param b The FFT buffer.
/// 
/// \return Returns length of the FFT buffer.
/// 
/// \see fada_getfft_buffer
/// \see fada_getfftvalue_buffer
/// \see fada_getfftvalues_buffer
/// \see fada_getfftvaluesrange_buffer
FADA_API fada_Pos fada_getfftsize_buffer(const fada_FFTBuffer* b);

//////////////////////////////////////////////////
/// \brief Retrieve the value at the position from the FFT buffer.
/// 
/// The FFT must be calculated before getting values from it. See \ref fada_calcfft.
/// 
/// \param b The FFT buffer.
/// \param pos The position on the FFT buffer. Must be less than \ref fada_getfftsize_buffer.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INDEX_OUT_OF_BOUNDS
///         \li \ref FADA_ERROR_INVALID_FFT_BUFFER
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getfft_buffer
/// \see fada_getfftsize_buffer
/// \see fada_getfftvalues_buffer
/// \see fada_getfftvaluesrange_buffer
/// \see fada_calcfft
/// \see fada_calcfft_channel
FADA_API fada_Error fada_getfftvalue_buffer(const fada_FFTBuffer* b, fada_Pos pos, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Retrieve all values from the FFT buffer.
/// 
/// The FFT must be calculated before getting values from it. See \ref fada_calcfft.
/// 
/// \param b The FFT buffer.
/// \param out_results Destination to write the results. Destination is an array with a length of at least \ref fada_getfftsize_buffer.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_FFT_BUFFER
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getfft_buffer
/// \see fada_getfftsize_buffer
/// \see fada_getfftvalue_buffer
/// \see fada_getfftvaluesrange_buffer
/// \see fada_calcfft
/// \see fada_calcfft_channel
FADA_API fada_Error fada_getfftvalues_buffer(const fada_FFTBuffer* b, fada_Res* out_results);

//////////////////////////////////////////////////
/// \brief Retrieve all values in range from the FFT buffer.
/// 
/// The FFT must be calculated before getting values from it. See \ref fada_calcfft.
/// \c len' should not be any greater than <tt>\ref fada_getfftsize_buffer - offset</tt>. If \c len is \c 0, it will grab all values from \c offset to the end of the FFT.
/// 
/// \param b The FFT buffer.
/// \param out_results Destination to write the results. Destination is an array with a length of at least \c len.
/// \param offset Where on the FFT buffer to begin retrieving values.
/// \param len How many values to retrieve.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INDEX_OUT_OF_BOUNDS
///         \li \ref FADA_ERROR_INVALID_FFT_BUFFER
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_SUCCESS
/// 
/// \see fada_getfft_buffer
/// \see fada_getfftsize_buffer
/// \see fada_getfftvalue_buffer
/// \see fada_getfftvalues_buffer
/// \see fada_calcfft
/// \see fada_calcfft_channel
FADA_API fada_Error fada_getfftvaluesrange_buffer(const fada_FFTBuffer* b, fada_Res* out_results, fada_Pos offset, fada_Pos len);


//////////////////////////////////////////////////
// Analyzation
//////////////////////////////////////////////////


//////////////////////////////////////////////////
/// \brief Retrieve a channel sample value in the analysis window.
/// 
/// Channels here are zero-based.
/// 
/// \param m The manager.
/// \param pos Position (in frames) where the sample is.
/// \param channel The specified channel.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_CHANNEL
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_getsamples
/// \see fada_getframe
/// \see fada_getframes
FADA_API fada_Error fada_getsample(fada_Manager* m, fada_Pos pos, unsigned int channel, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Retrieve all of the channel's sample values in the analysis window.
/// 
/// Channels here are zero-based.
/// 
/// \param m The manager.
/// \param channel The specified channel.
/// \param out_results Destination to write the results. Destination is an array with a length of at least \ref fada_getwindowframes.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_CHANNEL
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_getsample
/// \see fada_getframe
/// \see fada_getframes
FADA_API fada_Error fada_getsamples(fada_Manager* m, unsigned int channel, fada_Res* out_results);

//////////////////////////////////////////////////
/// \brief Retrieve a mixed sample (frame) value in the analysis window.
/// 
/// Channels will be mixed.
/// 
/// \param m The manager.
/// \param pos Position (in frames) where the frame is.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_getsample
/// \see fada_getsamples
/// \see fada_getframes
FADA_API fada_Error fada_getframe(fada_Manager* m, fada_Pos pos, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Retrieve all mixed sample values (frames) in the analysis window.
/// 
/// Channels will be mixed.
/// 
/// \param m The manager.
/// \param out_results Destination to write the results. Destination is an array with a length of at least \ref fada_getwindowframes.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_getsample
/// \see fada_getsamples
/// \see fada_getframe
FADA_API fada_Error fada_getframes(fada_Manager* m, fada_Res* out_results);

//////////////////////////////////////////////////
/// \brief Calculate the "beat" using the current analysis window.
/// 
/// Channels will be mixed.
/// "Beat" is calculated by measuring the change in samples. Lots of spikes and high frequencies means a lot of "beat."
/// 
/// \param m The manager.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_calcbeat_channel
FADA_API fada_Error fada_calcbeat(fada_Manager* m, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Calculate the "beat" of a single channel using the current analysis window.
/// 
/// Channels here are zero-based.
/// "Beat" is calculated by measuring the change in samples. Lots of spikes and high frequencies means a lot of "beat."
/// 
/// \param m The manager.
/// \param channel The specified channel.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_CHANNEL
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_calcbeat
FADA_API fada_Error fada_calcbeat_channel(fada_Manager* m, unsigned int channel, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Calculate the audible "bass" using the current analysis window.
/// 
/// Channels will be mixed.
/// "Bass" is calculated by measuring the amplitude of long wavelengths.
/// 
/// \param m The manager.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_calcbass_channel
FADA_API fada_Error fada_calcbass(fada_Manager* m, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Calculate the audible "bass" of a single channel using the current analysis window.
/// 
/// Channels here are zero-based.
/// "Bass" is calculated by measuring the amplitude of long wavelengths.
/// 
/// \param m The manager.
/// \param channel The specified channel.
/// \param out_result Destination to write the result.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_CHANNEL
///         \li \ref FADA_ERROR_INVALID_PARAMETER
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_calcbass
FADA_API fada_Error fada_calcbass_channel(fada_Manager* m, unsigned int channel, fada_Res* out_result);

//////////////////////////////////////////////////
/// \brief Calculate the Fast Fourier Transform using the current analysis window.
/// 
/// Channels will be mixed.
/// The FFT is generated on the assigned FFT buffer. If no FFT buffer was assigned, the manager's internal FFT buffer will be used.
/// 
/// \param m The manager.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_NOT_ENOUGH_MEMORY
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_calcfft_channel
FADA_API fada_Error fada_calcfft(fada_Manager* m);

//////////////////////////////////////////////////
/// \brief Calculate the Fast Fourier Transform for a single channel using the current analysis window.
/// 
/// Channels here are zero-based.
/// The FFT is generated on the assigned FFT buffer. If no FFT buffer was assigned, the manager's internal FFT buffer will be used.
/// 
/// \param m The manager.
/// \param channel The specified channel.
/// 
/// \return Returns one of the following error signals:
///         \li \ref FADA_ERROR_INVALID_CHANNEL
///         \li \ref FADA_ERROR_INVALID_TYPE
///         \li \ref FADA_ERROR_MANAGER_NOT_READY
///         \li \ref FADA_ERROR_NOT_ENOUGH_MEMORY
///         \li \ref FADA_ERROR_SUCCESS
///         \li \ref FADA_ERROR_WINDOW_NOT_CREATED
/// 
/// \see fada_calcfft
FADA_API fada_Error fada_calcfft_channel(fada_Manager* m, unsigned int channel);

#endif