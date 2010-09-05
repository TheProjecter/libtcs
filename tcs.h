/*
 *  tcs.h -- interface of the 'libtcs' library
 *
 *  Copyright (C) 2009-2010 milkyjing <milkyjing@gmail.com>
 *
 *  Note: for more detailed information about TCS file format,
 *  please refer to the TCS Specification.
 *  http://www.tcsub.com/tcs/spec.htm
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 *
 *  milkyjing
 *
 */

#ifndef LIBTCS_TCS_H
#define LIBTCS_TCS_H
#pragma once

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#define LIBTCS_VERSION 0x00006000    /**< High word of LIBTCS_VERSION indicates major libtcs version, and low word indicates minor version */
#define LIBTCS_VER_STR "0.6"

#define TCS_VERSION 0x10000000
#define TCS_VER_STR "1.0"

#define TCS_SIGNATURE 0x5343542E    /**< The signature of TCS file is 0x2E 0x54 0x43 0x53, which means `.TCS' in ASCII */

#define TCS_INIT_MIN_TIME 0xffffffff
#define TCS_INIT_MAX_TIME 0x00000000

#define TCS_RESERVED_POSX 0xffff
#define TCS_RESERVED_POSY 0xffff
#define TCS_RESERVED_POS 0xffffffff
#define TCS_INVALID_POSX TCS_RESERVED_POSX
#define TCS_INVALID_POSY TCS_RESERVED_POSY
#define TCS_INVALID_POS TCS_RESERVED_POS
#define TCS_MAX_POSX 0xfffe
#define TCS_MAX_POSY 0xfffe
#define TCS_MIN_POSX 0
#define TCS_MIN_POSY 0
#define TCS_INIT_MIN_POSX TCS_MAX_POSX
#define TCS_INIT_MIN_POSY TCS_MAX_POSY
#define TCS_INIT_MAX_POSX TCS_MIN_POSX
#define TCS_INIT_MAX_POSY TCS_MIN_POSY

#define TCS_FLAG_RAW 0
#define TCS_FLAG_COMPRESSED 1
#define TCS_FLAG_PARSED_HIGHEST_LV 2
#define TCS_FLAG_PARSED TCS_FLAG_PARSED_HIGHEST_LV
#define TCS_FLAG_PARSED_HIGHEST_LV_NONCOMPRESSED 3

#define TCS_ERROR -1
#define TCS_FAIL TCS_ERROR
#define TCS_FALSE 0
#define TCS_TRUE 1
#define tcs_error TCS_ERROR
#define tcs_fail TCS_FAIL
#define tcs_false TCS_FALSE
#define tcs_true TCS_TRUE

typedef int tcs_bool;    /**< -1 - error/fail, 0 - false, 1 - true */

/* These types are used to describe TCS file data */
typedef signed __int64 tcs_s64;
typedef unsigned long tcs_u32;
typedef unsigned short tcs_u16;
typedef unsigned char tcs_u8;
typedef tcs_u32 tcs_unit;
typedef tcs_u16 tcs_word;
typedef tcs_u8 tcs_byte;

/* These macros help to convert different data types */
#define MAKE16B16B(a, b) ((tcs_u32)(((tcs_u16)(a)) | (((tcs_u32)(b)) << 16)))
#define MAKEPOS MAKE16B16B
#define GETLOW16B(x) ((tcs_u16)(x))
#define GETPOSX GETLOW16B
#define GETHI16B(x) ((tcs_u16)(((tcs_u32)(x)) >> 16))
#define GETPOSY GETHI16B
#define MAKE8B8B8B8B(a, b, c, d) ((tcs_u32)(((tcs_u8)(a)) | (((tcs_u32)((tcs_u8)(b))) << 8) | (((tcs_u32)((tcs_u8)(c))) << 16) | (((tcs_u32)((tcs_u8)(d))) << 24)))
#define MAKERGBA MAKE8B8B8B8B
#define GETR(a) ((tcs_u8)(a))
#define GETG(a) ((tcs_u8)(((tcs_u32)(a)) >> 8))
#define GETB(a) ((tcs_u8)(((tcs_u32)(a)) >> 16))
#define GETA(a) ((tcs_u8)(((tcs_u32)(a)) >> 24))
#define MAKE24B8B(l24b, h8b) ((0x00FFFFFF & (tcs_u32)(l24b)) | ((0x000000FF & (tcs_u32)(h8b)) << 24))
#define MAKECL MAKE24B8B
#define GETHI8B(x) ((tcs_u8)(((tcs_u32)(x)) >> 24))
#define GETLAYER GETHI8B
#define GETLOW24B(x) (0x00FFFFFF & (tcs_u32)(x))
#define GETCOUNT GETLOW24B

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

#define COUNT_RAW_CHUNKS(filesize) (((filesize) - sizeof(TCS_Header)) / sizeof(TCS_RawChunk))    /**< count chunks of raw TCS file */


/**
 * TCS_File structure is used for handling TCS file I/O.
 */
typedef struct _tcs_file {
    FILE *fp;            /**< a pointer to FILE structure */
    tcs_unit minTime;    /**< use as a temp variable */
    tcs_unit maxTime;    /**< use as a temp variable */
    tcs_unit chunks;     /**< use as a temp variable */
    tcs_unit temp;       /**< use as a temp variable */
} TCS_File, *TCS_pFile;

/**
 * TCS_Header structure is used to represent TCS file's header part.
 */
typedef struct _tcs_header {
    /**
     * The Signature of TCS file is 0x2E 0x54 0x43 0x53, which means .TCS in ASCII.
     */
    tcs_unit signature;
    /**
     * Low word of Version indicates minor TCS file version, the initial minor TCS file version is 0. 
     * High word of Version indicates major TCS file version, the initial major TCS file version is 1.
     */
    tcs_unit version;
    /**
     * If Flag is 0, then it means the TCS file contains raw TCS FX data which is made up of dozens of raw DIPs, 
     * if Flag is 1, then it means the TCS file has been compressed, and contains packed DIPs. 
     * Otherwise (Flag >= 2), it means the TCS file has been parsed, and contains reorganized TCS FX data. 
     * There may be different kinds of parsed TCS files in the future. 
     * Raw TCS file is usually used as temp TCS file (used between softwares), 
     * and compressed TCS file is the most commonly used one (as permanent storage).
     */
    tcs_unit flag;
    /**
     * Low word of Resolution indicates the horizonal resolution that the TCS FX is dealing with. 
     * High word of Resolution indicates the vertical resolution that the TCS FX is dealing with. 
     * The unit of Resolution is pixel. Note that the maximum width and height are both 65535, 
     * which is much enough as video resolution.
     */
    tcs_unit resolution;
    /**
     * MinTime indicates the first time that the TCS FX has taken effects. 
     * The unit of MinTime is millisecond, so the range of  MinTime is [0, 4294967295], more than 1 thousand hours.
     */
    tcs_unit minTime;
    /**
     * MaxTime indicates the first time that the TCS FX has ended its effects. 
     * The unit of MaxTime is millisecond, so the range of  MaxTime is [0, 4294967295], more than 1 thousand hours.
     */
    tcs_unit maxTime;
    /**
     * Chunks indicates the number of data chunks in the body part of TCS file.
     */
    tcs_unit chunks;
    /**
     * fpsNumerator indicates the numerator of target video's FPS
     */
    tcs_unit fpsNumerator;
    /**
     * fpsDenominator indicates the denominator of target video's FPS
     */
    tcs_unit fpsDenominator;
    /**
     * Reserved, will be used in later versions.
     */
    tcs_unit reserved[7];
} TCS_Header, *TCS_pHeader;

/**
 * TCS_RawChunk structure is used to describe raw TCS file's minimal unit. 
 * if we use tcs_unit to define the whole position, then it has form of 0xposYposX, 
 * if we use tcs_unit to define the whole color, then it has form of 0xaabbggrr.
 */
typedef struct _tcs_raw_chunk {
    tcs_unit startTime; /**< starting time of the chunk */
    tcs_unit endTime;   /**< end time of the chunk */
    tcs_unit layer;  /**< layer of the chunk */
    tcs_u16  posX;   /**< x-axis position of the DIP in the chunk */
    tcs_u16  posY;   /**< y-axis position of the DIP in the chunk */
    tcs_u8   r;    /**< red channel of the DIP color */
    tcs_u8   g;    /**< green channel of the DIP color */
    tcs_u8   b;    /**< blue channel of the DIP color */
    tcs_u8   a;    /**< alpha channel of the DIP color */
} TCS_RawChunk, *TCS_pRawChunk;

/**
 * TCS_Chunk structure is used to describe compressed/parsed TCS file's minimal unit.
 */
typedef struct _tcs_chunk {
    tcs_unit startTime; /**< starting time of the chunk */
    tcs_unit endTime;   /**< end time of the chunk */
    tcs_unit layer_and_count;    /**< layer occupies the high 8 bits, and count occupies the low 24 bits */
    tcs_unit *pos_and_color;     /**< every pos_and_color occupies 2 tcs_unit */
} TCS_Chunk, *TCS_pChunk;

/**
 * TCS_Index structure is used for parsing compressed TCS file. 
 * Remark: because offset is a 32-bit value, so the max size of compressed TCS file supported by libtcs is 
 * limited to sizeof(tcs_unit) * 4GB = 16GB, thus the max TCS file size supported by libtcs is also limited to 16GB.
 */
typedef struct _tcs_index {
    tcs_u32 first;               /**< may be startTime or firstFrame depending on which function fills this structure */
    tcs_u32 last;                /**< may be endTime or lastFrame depending on which function fills this structure */
    tcs_unit layer_and_count;    /**< layer occupies the high 8 bits, and count occupies the low 24 bits */
    tcs_u32 offset;              /**< chunk offset in the TCS file, file position indicator = offset * sizeof(tcs_unit) */
} TCS_Index, *TCS_pIndex;

/**
 * TCS_Error_Code enumeration defines some common errors.
 */
typedef enum _tcs_error_code {
    tcs_error_success = 0,    /**< indicates a successful process */
    tcs_error_null_pointer,  /**< receive a null pointer which is not desired */
    tcs_error_file_not_found,  /**< can not find the target TCS file */
    tcs_error_file_cant_open,   /**< can not open the target TCS file */
    tcs_error_file_cant_create,  /**< can not create the target TCS file */
    tcs_error_file_while_reading,  /**< some error occurred while reading a TCS file */
    tcs_error_file_while_writing,    /**< some error occurred while writing to a TCS file */
    tcs_error_file_type_not_match,    /**< the target TCS file's type does not match the one which is required */
    tcs_error_file_type_not_support    /**< the target TCS file's type has not been supported yet */
} TCS_Error_Code;

/**
 * TCS_File_Open_Type enumeration defines two kinds of file opening type. 
 * This is only used in libtcs_open_file function.
 */
typedef enum _tcs_file_open_type {
    tcs_file_open_existing,    /**< open an existing TCS file */
    tcs_file_create_new,    /**< create a new TCS file */
    tcs_file_read_write     /**< open a TCS file with `read-write' permission */
} TCS_File_Open_Type;

/**
 * TCS_File_Position_Indicator enumeration defines three positions of file position indicator.
 */
typedef enum _tcs_file_position_indicator {
    tcs_fpi_set,      /**< position at the beginning of the file */
    tcs_fpi_header,   /**< position right after the TCS header */
    tcs_fpi_end       /**< position at the end of the file */
} TCS_File_Position_Indicator;


/* Inhibit C++ name-mangling for libtcs functions but not for system calls. */
#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

/**
 * Open an existing file or create a new file.
 * @param pFile a pointer to TCS_File structure
 * @param filename specify the file name which is to be opened
 * @param type specify file opening type
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_open_file(TCS_pFile pFile, const char *filename, TCS_File_Open_Type type);

/**
 * Close the tcs file handler associated with TCS_File structure.
 * @param pFile a pointer to TCS_File structure
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_close_file(TCS_pFile pFile);

/**
 * Can read any kinds of data from TCS file, but we do not recommend you using this function.
 * @see libtcs_read_header()
 * @see libtcs_read_raw_chunks()
 * @see libtcs_read_chunk()
 * @see libtcs_read_specified_chunk()
 * @see libtcs_read_chunks()
 * @param pFile a pointer to TCS_File structure
 * @param buf memory buffer that is going to hold the data read from the TCS file
 * @param count indicates the amount of tcs_unit that is going to read
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_read(TCS_pFile pFile, tcs_unit *buf, tcs_u32 count);

/**
 * Can write any kinds of data to TCS file, but we do not recommend you using this function.
 * @see libtcs_write_header()
 * @see libtcs_write_raw_chunks()
 * @see libtcs_write_chunk()
 * @see libtcs_write_chunks()
 * @param pFile a pointer to TCS_File structure
 * @param buf memory buffer that is going to be written to the TCS file
 * @param count indicates the amount of tcs_unit that is going to write
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_write(TCS_pFile pFile, const tcs_unit *buf, tcs_u32 count);

/**
 * Check the file's signature to investigate whether it's a TCS file or not. 
 * Remark: this function will NOT change the file position indicator.
 *
 * @param pFile a pointer to TCS_File structure
 * @return tcs_bool
 */
extern tcs_bool libtcs_check_signature(const TCS_pFile pFile);

/**
 * Set file position indicator.
 * @param pFile a pointer to TCS_File structure
 * @param position tcs_fpi_set - set the file position indicator to the beginning of the file
 *                 tcs_fpi_header - set the file position indicator to right after the TCS header
 *                 tcs_fpi_end - set the file position indicator to the end of the file
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_set_file_position_indicator(TCS_pFile pFile, TCS_File_Position_Indicator position);

/**
 * Use values specified by the params to fill the TCS_Header structure.
 * @param flag TCS_FLAG_RAW - raw TCS file, TCS_FLAG_COMPRESSED - compressed TCS file, TCS_FLAG_PARSED - parsed TCS file
 * @param width horizonal resolution of TCS FX
 * @param height vertical resolution of TCS FX
 * @param minTime indicates the first time that the TCS FX has taken effects
 * @param maxTime indicates the first time that the TCS FX has ended its effects
 * @param chunks indicates the number of data chunks in the body part of the TCS file
 * @param fpsNumerator indicates the numerator of target video's FPS
 * @param fpsDenominator indicates the denominator of target video's FPS
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_set_header(TCS_pHeader pHeader, tcs_unit flag, tcs_u16 width, tcs_u16 height, tcs_unit minTime, tcs_unit maxTime, tcs_unit chunks, tcs_unit fpsNumerator, tcs_unit fpsDenominator);

/**
 * Read the header part of TCS file and assign values to TCS_Header structure.
 * @param pFile a pointer to TCS_File structure
 * @param pHeader a pointer to TCS_Header structure
 * @param keepPosition TCS_TRUE - keep the file position indicator, TCS_FALSE do not keep the file position indicator
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_read_header(TCS_pFile pFile, TCS_pHeader pHeader, tcs_bool keepPosition);

/**
 * Write TCS_Header strucutre to file.
 * @param pFile a pointer to TCS_File structure
 * @param pHeader a pointer to TCS_Header structure which is going to be written to the TCS file
 * @param keepPosition TCS_TRUE - keep the file position indicator, TCS_FALSE do not keep file the position indicator
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_write_header(TCS_pFile pFile, const TCS_pHeader pHeader, tcs_bool keepPosition);

/**
 * Read raw chunks from file. 
 * Remark: make sure that the file position indicator is just at the beginning of a chunk.
 *
 * @param pFile a pointer to TCS_File structure
 * @param pRawChunk the address of TCS_RawChunk array that is going to hold chunk data
 * @param count indicates the amount of raw chunks that is going to read
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_read_raw_chunks(TCS_pFile pFile, TCS_pRawChunk pRawChunk, tcs_u32 count);

/**
 * Write raw chunks to file. 
 * Remark: make sure that the file position indicator is just at the beginning of a chunk.
 *
 * @param pFile a pointer to TCS_File structure
 * @param pRawChunk the address of TCS_RawChunk array that is going to be written to file
 * @param count indicates the amount of TCS_RawChunk that is going to write
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_write_raw_chunks(TCS_pFile pFile, const TCS_pRawChunk pRawChunk, tcs_u32 count);

/**
 * Allocate memory for one compressed/parsed chunk.
 * @param pChunk a pointer to TCS_Chunk structure
 * @param count indicates the amount of packed DIPs
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_alloc_chunk(TCS_pChunk pChunk, tcs_u32 count);

/**
 * Free memory of a compressed/parsed chunk.
 * @param pChunk a pointer to TCS_Chunk structure
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_free_chunk(TCS_pChunk pChunk);

/**
 * Read a compressed/parsed chunk from file. 
 * Remark: you should allocate memory for pChunk, but not for pChunk->pos_and_color. 
 * Make sure that the file position indicator is just at the beginning of a chunk.
 *
 * @param pFile a pointer to TCS_File structure
 * @param pChunk a pointer to TCS_Chunk structure that is going to hold chunk data
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_read_chunk(TCS_pFile pFile, TCS_pChunk pChunk);

/**
 * Read the compressed/parsed chunk specified by the offset from file. 
 * Remark: you should allocate memory for pChunk, but not for pChunk->pos_and_color. 
 * Make sure that the file position indicator is just at the beginning of a chunk.
 * The file position indicator is set by offset from the beginning of the file.
 *
 * @param pFile a pointer to TCS_File structure
 * @param offset the offset of the chunk from the beginning of the file
 * @param pChunk a pointer to TCS_Chunk structure that is going to hold chunk data
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_read_specified_chunk(TCS_pFile pFile, tcs_s64 offset, TCS_pChunk pChunk);

/**
 * Read compressed/parsed chunks from file. 
 * Remark: you should allocate memory for pChunk, but not for pChunk->pos_and_color. 
 * Make sure that the file position indicator is just at the beginning of a chunk.
 *
 * @see libtcs_read_chunk()
 * @param pFile a pointer to TCS_File structure
 * @param pChunk the address of TCS_Chunk array that is going to hold chunk data
 * @param count indicates the amount of compressed/parsed chunks that is going to read
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_read_chunks(TCS_pFile pFile, TCS_pChunk pChunk, tcs_u32 count);

/**
 * Write one compressed/parsed chunk to file. 
 * Remark: make sure that the file position indicator is just at the beginning of a chunk.
 *
 * @param pFile a pointer to TCS_File structure
 * @param pChunk a pointer to TCS_Chunk structure that is going to be written to file
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_write_chunk(TCS_pFile pFile, const TCS_pChunk pChunk);

/**
 * Write compressed/parsed chunks to file. 
 * Remark: make sure that the file position indicator is just at the beginning of a chunk.
 *
 * @see libtcs_write_chunk()
 * @param pFile a pointer to TCS_File structure
 * @param pChunk the address of TCS_Chunk array that is going to be written to file
 * @param count indicates the amount of TCS_Chunk that is going to write
 * @param freeChunks TCS_TRUE - free chunks after writing them to file, TCS_FALSE - keep chunks
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_write_chunks(TCS_pFile pFile, const TCS_pChunk pChunk, tcs_u32 count, tcs_bool freeChunks);

/**
 * Compress raw chunks (in stream), the result of compression largely depends on the 
 * order that the raw chunks were sent to the function, i.e., locality is a big issue 
 * when using this function. Remark: In the compressing process, adjacent raw DIPs who have 
 * the same StartT EndT and Layer can share the same chunk.
 *
 * @param pRawChunk the address of TCS_RawChunk array that is going to be compressed
 * @param rawChunks the amount of input raw chunks
 * @param pBuf a pointer to a block of memory buffer that is going to hold the compressed data
 * @param pChunkCount pointer to the amount of output compressed chunks
 * @param pUnitCount pointer to the amount of tcs_unit in the buffer
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_compress_raw_chunks(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, tcs_unit **pBuf, tcs_u32 *pChunkCount, tcs_u32 *pUnitCount);

/**
 * Convert raw chunks to compressed chunks, the result of compression largely depends on the 
 * order that the raw chunks were sent to the function, i.e., locality is a big issue 
 * when using this function. Remark: In the compressing process, adjacent raw DIPs who have 
 * the same StartT EndT and Layer can share the same chunk.
 *
 * @see libtcs_compress_raw_chunks()
 * @param pRawChunk the address of TCS_RawChunk array that is going to be compressed
 * @param rawChunks the amount of input raw chunks
 * @param ppChunk a pointer to TCS_pChunk that is going to hold the compressed data
 * @param pChunkCount pointer to the amount of output compressed chunks
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_raw_chunks(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, TCS_pChunk *ppChunk, tcs_u32 *pChunkCount);

/**
 * Uncompress a compressed chunk (in stream).
 * @param pChunk a pointer to a compressed TCS chunk that is going to be uncompressed
 * @param pBuf a pointer to a block of memory buffer that is going to hold the uncompressed data
 * @param pRawChunkCount pointer to the amount of output raw chunks
 * @param pUnitCount pointer to the amount of tcs_unit in the buffer
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_uncompress_chunk(const TCS_pChunk pChunk, tcs_unit **pBuf, tcs_u32 *pRawChunkCount, tcs_u32 *pUnitCount);

/**
 * Convert a compressed chunk to raw chunks.
 * @see libtcs_uncompress_chunk()
 * @param pChunk a pointer to a compressed TCS chunk that is going to be uncompressed
 * @param ppRawChunk a pointer to TCS_pRawChunk that is going to hold the uncompressed data
 * @param pRawChunkCount pointer to the amount of output raw chunks
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_chunk(const TCS_pChunk pChunk, TCS_pRawChunk *ppRawChunk, tcs_u32 *pRawChunkCount);

/**
 * Uncompress compressed chunks (in stream).
 * @param pChunk the address of TCS_Chunk array that is going to be uncompressed
 * @param chunks the amount of input compressed chunks
 * @param pBuf a pointer to a block of memory buffer that is going to hold the uncompressed data
 * @param pRawChunkCount pointer to the amount of output raw chunks
 * @param pUnitCount pointer to the amount of tcs_unit in the buffer
 * @param freeChunks TCS_TRUE - free chunks after writing them to file, TCS_FALSE - keep chunks
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_uncompress_chunks(TCS_pChunk pChunk, tcs_u32 chunks, tcs_unit **pBuf, tcs_u32 *pRawChunkCount, tcs_u32 *pUnitCount, tcs_bool freeChunks);

/**
 * Convert compressed chunks to raw chunks.
 * @see libtcs_uncompress_chunks()
 * @param pChunk the address of TCS_Chunk array that is going to be uncompressed
 * @param chunks the amount of input compressed chunks
 * @param ppRawChunk a pointer to TCS_pRawChunk that is going to hold the uncompressed data
 * @param pRawChunkCount pointer to the amount of output raw chunks
 * @param freeChunks TCS_TRUE - free chunks after writing them to file, TCS_FALSE - keep chunks
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_chunks(TCS_pChunk pChunk, tcs_u32 chunks, TCS_pRawChunk *ppRawChunk, tcs_u32 *pRawChunkCount, tcs_bool freeChunks);

/**
 * Convert a RGBA array to compressed chunk. 
 * Remark: the function will not change the value of startTime, endTime and layer byte in the output chunk.
 *
 * @see libtcs_convert_chunk_to_rgba()
 * @see libtcs_convert_chunks_to_rgba()
 * @param rgba the address of a RGBA array
 * @param width width of RGBA array
 * @param height height of RGBA array
 * @param pChunk a pointer to TCS_Chunk that is going to hold the converted data
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_rgba_to_chunk(const tcs_byte *rgba, tcs_u16 width, tcs_u16 height, TCS_pChunk pChunk);

/**
 * Blend two colors.
 *
 * @param back color of the background
 * @param over color of the overlay
 * @return the blended color
 */
extern tcs_u32 libtcs_blend_color(tcs_u32 back, tcs_u32 over);

/**
 * Convert a chunk to RGBA array. 
 * Remark: the function will just ignore the value of startTime, endTime and layer byte.
 *
 * @see libtcs_convert_rgba_to_chunk()
 * @see libtcs_convert_chunks_to_rgba()
 * @param pChunk a pointer to TCS_Chunk that is going to be converted
 * @param width width of target RGBA array
 * @param height height of target RGBA array
 * @param pRGBA pointer to the address of a RGBA array
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_chunk_to_rgba(const TCS_pChunk pChunk, tcs_u16 width, tcs_u16 height, tcs_byte **pRGBA);

/**
 * Convert chunks to RGBA array. 
 * Remark: the function will just ignore the value of startTime, endTime and layer byte. 
 * You should allocate memory for the target RGBA array. Note that this function only converts one chunk a time, 
 * if you want to convert more than one chunks, just use the function more than once.
 *
 * @see libtcs_convert_rgba_to_chunk()
 * @see libtcs_convert_chunk_to_rgba()
 * @param pChunk a pointer to TCS_Chunk that is going to be converted
 * @param width width of target RGBA array
 * @param height height of target RGBA array
 * @param rgba pointer to the RGBA array that is going to hold the converted data
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_chunks_to_rgba(const TCS_pChunk pChunk, tcs_u16 width, tcs_u16 height, tcs_byte *rgba);

/**
 * Resample RGBA array.
 * @param src the address of source RGBA array
 * @param width width of the source RGBA array
 * @param height height of the source RGBA array
 * @param pRGBA pointer to the address of a RGBA array
 * @param targetWidth width of target RGBA array
 * @param targetHeight height of target RGBA array
 */
extern TCS_Error_Code libtcs_resample_rgba(const tcs_byte *src, tcs_u16 width, tcs_u16 height, tcs_byte **pRGBA, tcs_u16 targetWidth, tcs_u16 targetHeight);

/**
 * Get minPosX minPosY maxPosX maxPosY of a chunk.
 * @param pChunk a pointer to TCS_Chunk structure
 * @param pMinPosX pointer to the minimal x-axis value of the target chunk
 * @param pMinPosY pointer to the minimal y-axis value of the target chunk
 * @param pMaxPosX pointer to the maximal x-axis value of the target chunk
 * @param pMaxPosY pointer to the maximal y-axis value of the target chunk
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_get_chunk_min_max_pos(const TCS_pChunk pChunk, tcs_u16 *pMinPosX, tcs_u16 *pMinPosY, tcs_u16 *pMaxPosX, tcs_u16 *pMaxPosY);

/**
 * Get minimal and maximal time of raw chunks.
 * @param pRawChunk a pointer to TCS_RawChunk structure
 * @param rawChunks the amount of input raw chunks
 * @param pMinTime pointer to the minimal start time value of the target chunks
 * @param pMaxTime pointer to the maximal end time value of the target chunks
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_get_raw_chunks_min_max_time(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, tcs_u32 *pMinTime, tcs_u32 *pMaxTime);

/**
 * Get minimal and maximal time of chunks.
 * @param pChunk a pointer to TCS_Chunk structure
 * @param chunks the amount of input compressed chunks
 * @param pMinTime pointer to the minimal start time value of the target chunks
 * @param pMaxTime pointer to the maximal end time value of the target chunks
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_get_chunks_min_max_time(const TCS_pChunk pChunk, tcs_u32 chunks, tcs_u32 *pMinTime, tcs_u32 *pMaxTime);

/**
 * Count the chunks of TCS FX data in compressed/parsed TCS file. 
 * Remark: this function will NOT change the file position indicator. 
 * Chunks in raw TCS file can be calculate by macros COUNT_RAW_CHUNKS.
 *
 * @see libtcs_get_min_max_time_and_chunks()
 * @param pFile a pointer to TCS_File structure
 * @param pChunkCount pointer to the amount of chunks in TCS file
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_count_chunks(const TCS_pFile pFile, tcs_unit *pChunkCount);

/**
 * Get minTime and maxTime of TCS FX data in TCS file and also count the chunks. 
 * Remark: this function will NOT change the file position indicator.
 *
 * @see libtcs_count_chunks()
 * @param pFile a pointer to TCS_File structure
 * @param pMinTime pointer to the first time that the TCS FX has taken effects
 * @param pMaxTime pointer to the first time that the TCS FX has ended its effects
 * @param pChunkCount pointer to the amount of chunks in TCS file
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_get_min_max_time_and_chunks(const TCS_pFile pFile, tcs_unit *pMinTime, tcs_unit *pMaxTime, tcs_unit *pChunkCount);

/**
 * Parse a compressed TCS file. 
 * Remark: this function will NOT change the file position indicator. 
 * In this function, TCS_Index.first means startTime and TCS_Index.last means endTime.
 *
 * @see libtcs_parse_compressed_tcs_file_with_fps()
 * @param pFile a pointer to TCS_File structure
 * @param ppIndex a pointer to TCS_pIndex that is going to hold the parsed TCS Index
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_parse_compressed_tcs_file(const TCS_pFile pFile, TCS_pIndex *ppIndex);

/**
 * Parse a compressed TCS file using specified FPS. 
 * Remark: this function will NOT change the file position indicator. 
 * In this function, TCS_Index.first means firstFrame and TCS_Index.last means lastFrame.
 *
 * @see libtcs_parse_compressed_tcs_file()
 * @param pFile a pointer to TCS_File structure
 * @param fpsNumerator FramePerSecond = fpsNumerator / (double)fpsDenominator
 * @param fpsDenominator FramePerSecond = fpsNumerator / (double)fpsDenominator
 * @param ppIndex a pointer to TCS_pIndex that is going to hold the parsed TCS Index
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_parse_compressed_tcs_file_with_fps(const TCS_pFile pFile, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, TCS_pIndex *ppIndex);

/**
 * Free the memory owned by TCS_pIndex structure.
 * @param pIndex the address of TCS_Index array that is going to be freed
 */
extern TCS_Error_Code libtcs_destroy_index(TCS_pIndex pIndex);

/* high level functions */

/**
 * Create a TCS frame. 
 * Remark: the function supports for compressed TCS file and highest level parsed TCS file (both compressed and non-compressed). 
 * If the source TCS file is a compressed TCS file param fpsNumerator and fpsDenominator will be ignored. 
 * If the source TCS file is a parsed TCS file param pIndex will be ignored.
 *
 * @param pFile a pointer to TCS_File structure
 * @param pHeader a pointer to TCS_Header structure
 * @param pIndex the address of TCS_Index array which holds the parsed index of compressed TCS file, TCS_Index.first means firstFrame and TCS_Index.last means lastFrame
 * @param n specify which frame is going to create
 * @param fpsNumerator numerator of the target video frame rate
 * @param fpsDenominator denominator of the target video frame rate
 * @param targetWidth width of the target video
 * @param targetHeight height of the target video
 * @param pBuf a pointer to a block of memory which is going to hold the TCS frame
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_create_tcs_frame(TCS_pFile pFile, const TCS_pHeader pHeader, const TCS_pIndex pIndex, tcs_u32 n, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, tcs_u16 targetWidth, tcs_u16 targetHeight, tcs_byte **pBuf);

/**
 * Parse a compressed TCS file to its (compressed) highest level parsed TCS file with user specified param - milliseconds. 
 * Remark: this function will NOT change the file position indicator. 
 * Flag = 1 - compressed TCS file, Flag = 2 - (compressed) highest level parsed TCS file. 
 *
 * @see libtcs_parse_compressed_tcs_file()
 * @see libtcs_convert_flag_1_to_2_with_fps()
 * @param pFile a pointer to TCS_File structure
 * @param filename file name of the output file
 * @param milliseconds timeline scaner incremental unit
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_flag_1_to_2_with_ms(const TCS_pFile pFile, const char *filename, tcs_u8 milliseconds);

/**
 * Parse a compressed TCS file to its (compressed) highest level parsed TCS file with specified fps - fpsNumerator and fpsDenominator. 
 * Remark: this function will NOT change the file position indicator. 
 * Flag = 1 - compressed TCS file, Flag = 2 - (compressed) highest level parsed TCS file. 
 *
 * @see libtcs_parse_compressed_tcs_file()
 * @see libtcs_convert_flag_1_to_2_with_ms()
 * @param pFile a pointer to TCS_File structure
 * @param filename file name of the output file
 * @param fpsNumerator FramePerSecond = fpsNumerator / (double)fpsDenominator
 * @param fpsDenominator FramePerSecond = fpsNumerator / (double)fpsDenominator
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_flag_1_to_2_with_fps(const TCS_pFile pFile, const char *filename, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator);

/**
 * Parse a compressed TCS file to its non-compressed highest level parsed TCS file with user specified param - milliseconds. 
 * Remark: this function will NOT change the file position indicator. 
 * Flag = 1 - compressed TCS file, Flag = 3 - non-compressed highest level parsed TCS file.
 *
 * @see libtcs_parse_compressed_tcs_file()
 * @see libtcs_convert_flag_1_to_3_with_fps()
 * @param pFile a pointer to TCS_File structure
 * @param filename file name of the output file
 * @param milliseconds timeline scaner incremental unit
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_flag_1_to_3_with_ms(const TCS_pFile pFile, const char *filename, tcs_u8 milliseconds);

/**
 * Parse a compressed TCS file to its non-compressed highest level parsed TCS file with specified fps - fpsNumerator and fpsDenominator. 
 * Remark: this function will NOT change the file position indicator. 
 * Flag = 1 - compressed TCS file, Flag = 3 - non-compressed highest level parsed TCS file.
 *
 * @see libtcs_parse_compressed_tcs_file()
 * @see libtcs_convert_flag_1_to_3_with_ms()
 * @param pFile a pointer to TCS_File structure
 * @param filename file name of the output file
 * @param fpsNumerator FramePerSecond = fpsNumerator / (double)fpsDenominator
 * @param fpsDenominator FramePerSecond = fpsNumerator / (double)fpsDenominator
 * @return TCS_Error_Code
 */
extern TCS_Error_Code libtcs_convert_flag_1_to_3_with_fps(const TCS_pFile pFile, const char *filename, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator);

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* LIBTCS_TCS_H */

