/*
 *  Copyright (C) 2009-2010 milkyjing <milkyjing@gmail.com>
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

#include "tcs.h"
#include "vector.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)    /* disable warnings for fopen function() */
#endif    /* _MSC_VER */


TCS_Error_Code libtcs_open_file(TCS_pFile pFile, const char *filename, TCS_File_Open_Type type) {
    if (!pFile) return tcs_error_null_pointer;
    if (tcs_file_open_existing == type) {
        pFile->fp = fopen(filename, "rb");    /* file should open in binary mode */
        if (!pFile->fp) return tcs_error_file_cant_open;
    } else if (tcs_file_create_new == type) {
        pFile->fp = fopen(filename, "wb");    /* file should open in binary mode */
        if (!pFile->fp) return tcs_error_file_cant_create;
    } else if (tcs_file_read_write == type) {
        pFile->fp = fopen(filename, "r+b");    /* file should open in binary mode */
        if (!pFile->fp) return tcs_error_file_cant_open;
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_close_file(TCS_pFile pFile) {
    if (!pFile) return tcs_error_null_pointer;
    fclose(pFile->fp);
    return tcs_error_success;
}

TCS_Error_Code libtcs_read(TCS_pFile pFile, tcs_unit *buf, tcs_u32 count) {
    if (!pFile) return tcs_error_null_pointer;
    if (fread(buf, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_reading;
    return tcs_error_success;
}

TCS_Error_Code libtcs_write(TCS_pFile pFile, const tcs_unit *buf, tcs_u32 count) {
    if (!pFile) return tcs_error_null_pointer;
    if (fwrite(buf, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_writing;
    return tcs_error_success;
}

tcs_bool libtcs_check_signature(const TCS_pFile pFile) {
    fpos_t position;
    tcs_unit signature;
    if (!pFile) return TCS_FAIL;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    fseek(pFile->fp, 0, SEEK_SET);
    fread(&signature, sizeof(tcs_unit), 1, pFile->fp);
    fsetpos(pFile->fp, &position);    /* reset file position indicator */
    if (TCS_SIGNATURE == signature) return TCS_TRUE;
    return TCS_FALSE;
}

TCS_Error_Code libtcs_set_file_position_indicator(TCS_pFile pFile, TCS_File_Position_Indicator position) {
    if (!pFile) return tcs_error_null_pointer;
    if (tcs_fpi_set == position) {
        fseek(pFile->fp, 0, SEEK_SET);
    } else if (tcs_fpi_header == position) {
        fseek(pFile->fp, sizeof(TCS_Header), SEEK_SET);
    } else {
        fseek(pFile->fp, 0, SEEK_END);
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_set_header(TCS_pHeader pHeader, tcs_unit flag, tcs_u16 width, tcs_u16 height, tcs_unit minTime, tcs_unit maxTime, tcs_unit chunks, tcs_unit fpsNumerator, tcs_unit fpsDenominator) {
    if (!pHeader) return tcs_error_null_pointer;
    memset(pHeader, 0, sizeof(TCS_Header));
    pHeader->signature = TCS_SIGNATURE;
    pHeader->version = TCS_VERSION;
    pHeader->flag = flag;
    pHeader->resolution = MAKEPOS(width, height);
    pHeader->minTime = minTime;
    pHeader->maxTime = maxTime;
    pHeader->chunks = chunks;
    pHeader->fpsNumerator = fpsNumerator;
    pHeader->fpsDenominator = fpsDenominator;
    return tcs_error_success;
}

TCS_Error_Code libtcs_read_header(TCS_pFile pFile, TCS_pHeader pHeader, tcs_bool keepPosition) {
    int count;
    fpos_t position;
    if (!pFile || !pHeader) return tcs_error_null_pointer;
    count = sizeof(TCS_Header) >> 2;
    if (keepPosition) {
        fgetpos(pFile->fp, &position);    /* remember file position indicator */
        fseek(pFile->fp, 0, SEEK_SET);
        if (fread(pHeader, sizeof(tcs_unit), count, pFile->fp) != count) {
            fsetpos(pFile->fp, &position);    /* reset file position indicator */
            return tcs_error_file_while_reading;
        }
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
    } else {
        fseek(pFile->fp, 0, SEEK_SET);
        if (fread(pHeader, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_reading;
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_write_header(TCS_pFile pFile, const TCS_pHeader pHeader, tcs_bool keepPosition) {
    int count;
    fpos_t position;
    if (!pFile || !pHeader) return tcs_error_null_pointer;
    count = sizeof(TCS_Header) >> 2;
    if (keepPosition) {
        fgetpos(pFile->fp, &position);    /* remember file position indicator */
        fseek(pFile->fp, 0, SEEK_SET);
        if (fwrite(pHeader, sizeof(tcs_unit), count, pFile->fp) != count) {
            fsetpos(pFile->fp, &position);
            return tcs_error_file_while_writing;
        }
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
    } else {
        fseek(pFile->fp, 0, SEEK_SET);
        if (fwrite(pHeader, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_writing;
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_read_raw_chunks(TCS_pFile pFile, TCS_pRawChunk pRawChunk, tcs_u32 count) {
    if (!pFile || !pRawChunk) return tcs_error_null_pointer;
    if (fread(pRawChunk, sizeof(TCS_RawChunk), count, pFile->fp) != count) return tcs_error_file_while_reading;
    return tcs_error_success;
}

TCS_Error_Code libtcs_write_raw_chunks(TCS_pFile pFile, const TCS_pRawChunk pRawChunk, tcs_u32 count) {
    if (!pFile || !pRawChunk) return tcs_error_null_pointer;
    if (fwrite(pRawChunk, sizeof(TCS_RawChunk), count, pFile->fp) != count) return tcs_error_file_while_writing;
    return tcs_error_success;
}

TCS_Error_Code libtcs_alloc_chunk(TCS_pChunk pChunk, tcs_u32 count) {
    if (!pChunk) return tcs_error_null_pointer;
    pChunk->pos_and_color = (tcs_unit *)malloc(count * (sizeof(tcs_unit) << 1));    /* every pos_and_color takes up 2 tcs_unit */
    return tcs_error_success;
}

TCS_Error_Code libtcs_free_chunk(TCS_pChunk pChunk) {
    if (!pChunk) return tcs_error_null_pointer;
    free(pChunk->pos_and_color);
    memset(pChunk, 0, sizeof(TCS_Chunk));
    return tcs_error_success;
}

TCS_Error_Code libtcs_read_chunk(TCS_pFile pFile, TCS_pChunk pChunk) {
    tcs_u32 count;
    if (!pFile) return tcs_error_null_pointer;
    if (fread(pChunk, sizeof(tcs_unit), 3, pFile->fp) != 3) return tcs_error_file_while_reading;    /* startTime endTime layer_and_count takes up 3 tcs_unit */
    count = GETCOUNT(pChunk->layer_and_count) << 1;    /* every pos_and_color takes up 2 tcs_unit */
    pChunk->pos_and_color = (tcs_unit *)malloc(count * sizeof(tcs_unit));
    if (fread(pChunk->pos_and_color, sizeof(tcs_unit), count, pFile->fp) != count) {
        free(pChunk->pos_and_color);
        return tcs_error_file_while_reading;
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_read_specified_chunk(TCS_pFile pFile, tcs_s64 offset, TCS_pChunk pChunk) {
    tcs_u32 count;
    if (!pFile) return tcs_error_null_pointer;
#ifdef __GNUC__
        fseeko64(pFile->fp, offset, SEEK_SET);
#else
#   ifdef _MSC_VER
        _fseeki64(pFile->fp, offset, SEEK_SET);
#   else
        fseek(pFile->fp, offset, SEEK_SET);
#   endif
#endif
    if (fread(pChunk, sizeof(tcs_unit), 3, pFile->fp) != 3) return tcs_error_file_while_reading;    /* startTime endTime layer_and_count takes up 3 tcs_unit */
    count = GETCOUNT(pChunk->layer_and_count) << 1;    /* every pos_and_color takes up 2 tcs_unit */
    pChunk->pos_and_color = (tcs_unit *)malloc(count * sizeof(tcs_unit));
    if (fread(pChunk->pos_and_color, sizeof(tcs_unit), count, pFile->fp) != count) {
        free(pChunk->pos_and_color);
        return tcs_error_file_while_reading;
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_read_chunks(TCS_pFile pFile, TCS_pChunk pChunk, tcs_u32 count) {
    TCS_Error_Code error;
    tcs_u32 i = 0;
    do error = libtcs_read_chunk(pFile, &pChunk[i ++]);
    while (i < count && tcs_error_success == error);
    return error;
}

TCS_Error_Code libtcs_write_chunk(TCS_pFile pFile, const TCS_pChunk pChunk) {
    int count;
    if (!pFile || !pChunk) return tcs_error_null_pointer;
    count = GETCOUNT(pChunk->layer_and_count) << 1;    /* every pos_and_color takes up 2 tcs_unit */
    if (fwrite(pChunk, sizeof(tcs_unit), 3, pFile->fp) != 3) return tcs_error_file_while_writing;   /* startTime endTime layer_and_count takes up 3 tcs_unit */
    if (fwrite(pChunk->pos_and_color, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_writing;
    return tcs_error_success;
}

TCS_Error_Code libtcs_write_chunks(TCS_pFile pFile, const TCS_pChunk pChunk, tcs_u32 count, tcs_bool freeChunks) {
    TCS_Error_Code error;
    tcs_u32 i = 0;
    if (freeChunks) {
        do {
            error = libtcs_write_chunk(pFile, &pChunk[i]);
            libtcs_free_chunk(&pChunk[i]);
            i ++;
        } while (i < count && tcs_error_success == error);
    } else {
        do error = libtcs_write_chunk(pFile, &pChunk[i ++]);
        while (i < count && tcs_error_success == error);
    }
    return error;
}

TCS_Error_Code libtcs_compress_raw_chunks(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, tcs_unit **pBuf, tcs_u32 *pChunkCount, tcs_u32 *pUnitCount) {
    tcs_u32 i, count, compCount, singleCount;
    tcs_unit prevStartTime, prevEndTime, prevLayer;
    tcs_unit *compBuf;
    if (!pRawChunk) return tcs_error_null_pointer;
    compBuf = (tcs_unit *)malloc(rawChunks * sizeof(TCS_RawChunk));    /* allocate a block of large enough memory buffer */
    memset(compBuf, 0, rawChunks * sizeof(TCS_RawChunk));
    compBuf[0] = pRawChunk[0].startTime;
    compBuf[1] = pRawChunk[0].endTime;
    compBuf[2] = MAKECL(1, pRawChunk[0].layer);
    compBuf[3] = MAKEPOS(pRawChunk[0].posX, pRawChunk[0].posY);
    compBuf[4] = MAKERGBA(pRawChunk[0].r, pRawChunk[0].g, pRawChunk[0].b, pRawChunk[0].a);
    count       = 5;    /* amount of tcs_unit being used in compBuf, 5 == sizeof(TCS_RawChunk) / sizeof(tcs_unit) */
    compCount   = 0;    /* the compressing times */
    singleCount = 0;    /* counter in a compressed chunk */
    prevStartTime = pRawChunk[0].startTime;
    prevEndTime   = pRawChunk[0].endTime;
    prevLayer     = pRawChunk[0].layer;
    for (i = 1; i < rawChunks; i ++) {
        /* compress one time can save 3 * DWORDs space!
           In the compressing process, adjacent raw DIPs who have the same StartT EndT and Layer can share the same chunk */
        if (prevStartTime == pRawChunk[i].startTime && prevEndTime == pRawChunk[i].endTime && prevLayer == pRawChunk[i].layer) {
            compCount ++;
            singleCount ++;
            compBuf[5 * i - 3 * compCount + 3] = MAKEPOS(pRawChunk[i].posX, pRawChunk[i].posY);
            compBuf[5 * i - 3 * compCount + 4] = MAKERGBA(pRawChunk[i].r, pRawChunk[i].g, pRawChunk[i].b, pRawChunk[i].a);
            compBuf[5 * i - 3 * compCount - 2 * singleCount + 2] ++;
            count += 2;
        } else {
            compBuf[5 * i - 3 * compCount + 0] = pRawChunk[i].startTime;
            compBuf[5 * i - 3 * compCount + 1] = pRawChunk[i].endTime;
            compBuf[5 * i - 3 * compCount + 2] = MAKECL(1, pRawChunk[i].layer);
            compBuf[5 * i - 3 * compCount + 3] = MAKEPOS(pRawChunk[i].posX, pRawChunk[i].posY);
            compBuf[5 * i - 3 * compCount + 4] = MAKERGBA(pRawChunk[i].r, pRawChunk[i].g, pRawChunk[i].b, pRawChunk[i].a);
            prevStartTime = pRawChunk[i].startTime;
            prevEndTime   = pRawChunk[i].endTime;
            prevLayer     = pRawChunk[i].layer;
            singleCount   = 0;
            count += 5;
        }
    }
    compBuf = (tcs_unit *)realloc(compBuf, count * sizeof(tcs_unit));    /* sweep no-used memory */
    *pBuf = compBuf;
    *pChunkCount = rawChunks - compCount;
    *pUnitCount = count;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_raw_chunks(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, TCS_pChunk *ppChunk, tcs_u32 *pChunkCount) {
    tcs_unit *buf;
    tcs_u32 i, index, size, compChunks, units;
    TCS_Error_Code error;
    TCS_pChunk pChunk;
    error = libtcs_compress_raw_chunks(pRawChunk, rawChunks, &buf, &compChunks, &units);
    if (tcs_error_success != error) return error;
    pChunk = (TCS_pChunk)malloc(compChunks * sizeof(TCS_Chunk));
    index = 0;
    for (i = 0; i < compChunks; i ++) {
        pChunk[i].startTime = buf[index + 0];
        pChunk[i].endTime = buf[index + 1];
        pChunk[i].layer_and_count = buf[index + 2];
        size = GETCOUNT(pChunk[i].layer_and_count) * (sizeof(tcs_unit) << 1);
        pChunk[i].pos_and_color = (tcs_unit *)malloc(size);
        memcpy(pChunk[i].pos_and_color, &buf[index + 3], size);
        index += 3 + (GETCOUNT(pChunk[i].layer_and_count) << 1);
    }
    free(buf);
    *ppChunk = pChunk;
    *pChunkCount = compChunks;
    return tcs_error_success;
}

TCS_Error_Code libtcs_uncompress_chunk(const TCS_pChunk pChunk, tcs_unit **pBuf, tcs_u32 *pRawChunkCount, tcs_u32 *pUnitCount) {
    tcs_u32 i, count, size;    /* count equals *rawChunks */
    tcs_unit *buf;
    if (!pChunk) return tcs_error_null_pointer;
    count = GETCOUNT(pChunk->layer_and_count);
    size = count * sizeof(TCS_RawChunk);
    buf = (tcs_unit *)malloc(size);
    for (i = 0; i < count; i ++) {
        buf[5 * i + 0] = pChunk->startTime;
        buf[5 * i + 1] = pChunk->endTime;
        buf[5 * i + 2] = GETLAYER(pChunk->layer_and_count);
        buf[5 * i + 3] = pChunk->pos_and_color[2 * i + 0];
        buf[5 * i + 4] = pChunk->pos_and_color[2 * i + 1];
    }
    *pBuf = buf;
    *pRawChunkCount = count;
    *pUnitCount = size >> 2;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_chunk(const TCS_pChunk pChunk, TCS_pRawChunk *ppRawChunk, tcs_u32 *pRawChunkCount) {
    tcs_u32 units;
    tcs_unit *buf;
    TCS_Error_Code error;
    error = libtcs_uncompress_chunk(pChunk, &buf, pRawChunkCount, &units);
    if (tcs_error_success != error) return error;
    *ppRawChunk = (TCS_pRawChunk)buf;
    return tcs_error_success;
}

TCS_Error_Code libtcs_uncompress_chunks(TCS_pChunk pChunk, tcs_u32 chunks, tcs_unit **pBuf, tcs_u32 *pRawChunkCount, tcs_u32 *pUnitCount, tcs_bool freeChunks) {
    tcs_u32 i, j, index, count, size;    /* count equals *rawChunks */
    tcs_unit *buf;
    if (!pChunk) return tcs_error_null_pointer;
    count = 0;
    for (i = 0; i < chunks; i ++) {
        count += GETCOUNT(pChunk[i].layer_and_count);
    }
    size = count * sizeof(TCS_RawChunk);
    buf = (tcs_unit *)malloc(size);
    index = 0;
    if (freeChunks) {
        for (i = 0; i < chunks; i ++) {
            tcs_u32 n = GETCOUNT(pChunk[i].layer_and_count);
            for (j = 0; j < n; j ++) {
                buf[5 * index + 0] = pChunk[i].startTime;
                buf[5 * index + 1] = pChunk[i].endTime;
                buf[5 * index + 2] = GETLAYER(pChunk[i].layer_and_count);
                buf[5 * index + 3] = pChunk[i].pos_and_color[2 * j + 0];
                buf[5 * index + 4] = pChunk[i].pos_and_color[2 * j + 1];
                index ++;
            }
            libtcs_free_chunk(&pChunk[i]);
        }
    } else {
        for (i = 0; i < chunks; i ++) {
            tcs_u32 n = GETCOUNT(pChunk[i].layer_and_count);
            for (j = 0; j < n; j ++) {
                buf[5 * index + 0] = pChunk[i].startTime;
                buf[5 * index + 1] = pChunk[i].endTime;
                buf[5 * index + 2] = GETLAYER(pChunk[i].layer_and_count);
                buf[5 * index + 3] = pChunk[i].pos_and_color[2 * j + 0];
                buf[5 * index + 4] = pChunk[i].pos_and_color[2 * j + 1];
                index ++;
            }
        }
    }
    *pBuf = buf;
    *pRawChunkCount = count;
    *pUnitCount = size >> 2;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_chunks(TCS_pChunk pChunk, tcs_u32 chunks, TCS_pRawChunk *ppRawChunk, tcs_u32 *pRawChunkCount, tcs_bool freeChunks) {
    tcs_u32 units;
    tcs_unit *buf;
    TCS_Error_Code error;
    error = libtcs_uncompress_chunks(pChunk, chunks, &buf, pRawChunkCount, &units, freeChunks);
    if (tcs_error_success != error) return error;
    *ppRawChunk = (TCS_pRawChunk)buf;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_rgba_to_chunk(const tcs_byte *rgba, tcs_u16 width, tcs_u16 height, TCS_pChunk pChunk) {
    tcs_u16 h, w;
    tcs_u32 index, count, offset, size;
    if (!rgba || !pChunk) return tcs_error_null_pointer;
    pChunk->pos_and_color = (tcs_unit *)malloc(width * height * (sizeof(tcs_unit) << 1));    /* every pos_and_color occupies 2 tcs_unit */
    index = 0;
    for (h = 0; h < height; h ++) {
        for (w = 0; w < width; w ++) {
            offset = h * (width << 2) + (w << 2);
            if (0 != rgba[offset + 3]) {
                pChunk->pos_and_color[(index << 1)] = MAKEPOS(w, h);
                pChunk->pos_and_color[(index << 1) + 1] = *((const tcs_unit *)&rgba[offset]);
                index ++;
            }
        }
    }
    count = index;
    size = count * (sizeof(tcs_unit) << 1);
    pChunk->layer_and_count = MAKECL(count, GETLAYER(pChunk->layer_and_count));
    pChunk->pos_and_color = (tcs_unit *)realloc(pChunk->pos_and_color, size);
    return tcs_error_success;
}

tcs_u32 libtcs_blend_color(tcs_u32 back, tcs_u32 over) {
    tcs_u8 r, g, b, a, r1, g1, b1, a1, r2, g2, b2, a2;
    r1 = GETR(back);
    g1 = GETG(back);
    b1 = GETB(back);
    a1 = GETA(back);
    r2 = GETR(over);
    g2 = GETG(over);
    b2 = GETB(over);
    a2 = GETA(over);
    a = 255 - (255 - a1) * (255 - a2) / 255;
    if (0 != a) {
        r = (r2 * a2 + r1 * a1 * (255 - a2) / 255) / a;
        g = (g2 * a2 + g1 * a1 * (255 - a2) / 255) / a;
        b = (b2 * a2 + b1 * a1 * (255 - a2) / 255) / a;
    } else {
        r = 0;
        g = 0;
        b = 0;
    }
    return MAKERGBA(r, g, b, a);
}

TCS_Error_Code libtcs_convert_chunk_to_rgba(const TCS_pChunk pChunk, tcs_u16 width, tcs_u16 height, tcs_byte **pRGBA) {
    tcs_u16 x, y;
    tcs_u32 i, pitch, size, xx, yy;
    tcs_byte r, g, b, a, r0, g0, b0, a0, A;
    tcs_byte *rgba;
    if (!pChunk) return tcs_error_null_pointer;
    pitch = width << 2;
    size = height * pitch;
    rgba = (tcs_byte *)malloc(size);
    memset(rgba, 0, size);
    for (i = 0; i < GETCOUNT(pChunk->layer_and_count); i ++) {
        x = GETPOSX(pChunk->pos_and_color[i << 1]);
        y = GETPOSY(pChunk->pos_and_color[i << 1]);
        if (x >= width || y >= height) continue;
        r = GETR(pChunk->pos_and_color[(i << 1) + 1]);
        g = GETG(pChunk->pos_and_color[(i << 1) + 1]);
        b = GETB(pChunk->pos_and_color[(i << 1) + 1]);
        a = GETA(pChunk->pos_and_color[(i << 1) + 1]);
        yy = y * pitch;
        xx = x << 2;
        r0 = rgba[yy + xx];
        g0 = rgba[yy + xx + 1];
        b0 = rgba[yy + xx + 2];
        a0 = rgba[yy + xx + 3];
        A = 255 - (255 - a) * (255 - a0) / 255;
        if (0 == A) continue;
        rgba[yy + xx]     = (r * a + r0 * a0 * (255 - a) / 255) / A;
        rgba[yy + xx + 1] = (g * a + g0 * a0 * (255 - a) / 255) / A;
        rgba[yy + xx + 2] = (b * a + b0 * a0 * (255 - a) / 255) / A;
        rgba[yy + xx + 3] =  A;
    }
    *pRGBA = rgba;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_chunks_to_rgba(const TCS_pChunk pChunk, tcs_u16 width, tcs_u16 height, tcs_byte *rgba) {
    tcs_u16 x, y;
    tcs_u32 i, pitch, xx, yy;
    tcs_byte r, g, b, a, r0, g0, b0, a0, A;
    if (!pChunk || !rgba) return tcs_error_null_pointer;
    pitch = width << 2;
    for (i = 0; i < GETCOUNT(pChunk->layer_and_count); i ++) {
        x = GETPOSX(pChunk->pos_and_color[i << 1]);
        y = GETPOSY(pChunk->pos_and_color[i << 1]);
        if (x >= width || y >= height) continue;
        r = GETR(pChunk->pos_and_color[(i << 1) + 1]);
        g = GETG(pChunk->pos_and_color[(i << 1) + 1]);
        b = GETB(pChunk->pos_and_color[(i << 1) + 1]);
        a = GETA(pChunk->pos_and_color[(i << 1) + 1]);
        yy = y * pitch;
        xx = x << 2;
        r0 = rgba[yy + xx];
        g0 = rgba[yy + xx + 1];
        b0 = rgba[yy + xx + 2];
        a0 = rgba[yy + xx + 3];
        A = 255 - (255 - a) * (255 - a0) / 255;
        if (0 == A) continue;
        rgba[yy + xx]     = (r * a + r0 * a0 * (255 - a) / 255) / A;
        rgba[yy + xx + 1] = (g * a + g0 * a0 * (255 - a) / 255) / A;
        rgba[yy + xx + 2] = (b * a + b0 * a0 * (255 - a) / 255) / A;
        rgba[yy + xx + 3] =  A;
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_resample_rgba(const tcs_byte *src, tcs_u16 width, tcs_u16 height, tcs_byte **pRGBA, tcs_u16 targetWidth, tcs_u16 targetHeight) {
    tcs_u16 h, w;
    tcs_u32 pitch, targetPitch, targetSize, Sx, Sy, Dx, Dy;
    tcs_byte *dst;
    if (!src) return tcs_error_null_pointer;
    pitch = width << 2;
    targetPitch = targetWidth << 2;
    targetSize = targetHeight * targetPitch;
    dst = (tcs_byte *)malloc(targetSize);
    for (h = 0; h < targetHeight; h ++) {
        Dy = h * targetPitch;    /* Destination buffer postion-y */
        Sy = (h * height / targetHeight) * pitch;  /* Source buffer postion-y */
        for (w = 0; w < targetWidth; w ++) {
            Dx = w << 2;       /* Destination buffer postion-x */
            Sx = (w * width / targetWidth) << 2;    /* Source buffer postion-x */
            if (0 != src[Sy + Sx + 3]) {             /* we predict that there are a lot of transparent pixels */
                dst[Dy + Dx]     = src[Sy + Sx];
                dst[Dy + Dx + 1] = src[Sy + Sx + 1];
                dst[Dy + Dx + 2] = src[Sy + Sx + 2];
                dst[Dy + Dx + 3] = src[Sy + Sx + 3];
            }
        }
    }
    *pRGBA = dst;
    return tcs_error_success;
}

TCS_Error_Code libtcs_get_chunk_min_max_pos(const TCS_pChunk pChunk, tcs_u16 *pMinPosX, tcs_u16 *pMinPosY, tcs_u16 *pMaxPosX, tcs_u16 *pMaxPosY) {
    tcs_u32 i;
    tcs_u16 minX, minY, maxX, maxY;
    if (!pChunk) return tcs_error_null_pointer;
    minX = TCS_INIT_MIN_POSX;
    minY = TCS_INIT_MIN_POSY;
    maxX = TCS_INIT_MAX_POSX;
    maxY = TCS_INIT_MAX_POSY;
    for (i = 0; i < GETCOUNT(pChunk->layer_and_count); i ++) {
        if (TCS_INVALID_POS == pChunk->pos_and_color[i << 1]) continue;
        minX = __min(minX, GETPOSX(pChunk->pos_and_color[i << 1]));
        minY = __min(minY, GETPOSY(pChunk->pos_and_color[i << 1]));
        maxX = __max(maxX, GETPOSX(pChunk->pos_and_color[i << 1]));
        maxY = __max(maxY, GETPOSY(pChunk->pos_and_color[i << 1]));
    }
    *pMinPosX = minX;
    *pMinPosY = minY;
    *pMaxPosX = maxX;
    *pMaxPosY = maxY;
    return tcs_error_success;
}

TCS_Error_Code libtcs_get_raw_chunks_min_max_time(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, TCS_pHeader pHeader) {
    tcs_u32 i;
    if (!pRawChunk || !pHeader) return tcs_error_null_pointer;
    for (i = 0; i < rawChunks; i ++) {
        pHeader->minTime = __min(pHeader->minTime, pRawChunk[i].startTime);
        pHeader->maxTime = __max(pHeader->maxTime, pRawChunk[i].endTime);
    }
    return tcs_error_success;
}

TCS_Error_Code libtcs_get_chunks_min_max_time(const TCS_pChunk pChunk, tcs_u32 chunks, tcs_u32 *pMinTime, tcs_u32 *pMaxTime) {
    tcs_u32 i, minTime, maxTime;
    minTime = TCS_INIT_MIN_TIME;
    maxTime = TCS_INIT_MAX_TIME;
    if (!pChunk) return tcs_error_null_pointer;
    for (i = 0; i < chunks; i ++) {
        minTime = __min(minTime, pChunk[i].startTime);
        maxTime = __max(maxTime, pChunk[i].endTime);
    }
    *pMinTime = minTime;
    *pMaxTime = maxTime;
    return tcs_error_success;
}

TCS_Error_Code libtcs_count_chunks(const TCS_pFile pFile, tcs_unit *pChunkCount) {
    TCS_Error_Code error;
    fpos_t position;
    tcs_u32 count;    /* the same as *chunks */
    tcs_unit buf[1];    /* to hold layer_and_count */
    TCS_Header header;
    if (!pFile) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);
    if (tcs_error_success != error) {
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
        return error;
    }
    if (TCS_FLAG_RAW == header.flag) return tcs_error_file_type_not_match;
    fseek(pFile->fp, 2 * sizeof(tcs_unit), SEEK_CUR);    /* reach the first layer_and_count */
    count = 0;
    while (1) {
        fread(buf, sizeof(tcs_unit), 1, pFile->fp);
        if (feof(pFile->fp) == 0) {
            fseek(pFile->fp, (2 + (GETCOUNT(buf[0]) << 1)) * sizeof(tcs_unit), SEEK_CUR);
            count ++;
        } else break;
    }
    fsetpos(pFile->fp, &position);    /* reset file position indicator */
    *pChunkCount = count;
    return tcs_error_success;
}

TCS_Error_Code libtcs_get_min_max_time_and_chunks(const TCS_pFile pFile, tcs_unit *pMinTime, tcs_unit *pMaxTime, tcs_unit *pChunkCount) {
    TCS_Error_Code error;
    fpos_t position;
    tcs_u32 mintime, maxtime, count;    /* count is the same as *chunks */
    tcs_unit buf[3];    /* to hold startTime endTime layer_and_count */
    TCS_Header header;
    if (!pFile) return tcs_error_null_pointer;
    mintime = TCS_INIT_MIN_TIME;
    maxtime = TCS_INIT_MAX_TIME;
    count = 0;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);
    if (tcs_error_success != error) {
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
        return error;
    }
    if (TCS_FLAG_RAW == header.flag) {
        while (1) {
            fread(buf, sizeof(tcs_unit), 2, pFile->fp);    /* startTime endTime */
            if (feof(pFile->fp) == 0) {
                fseek(pFile->fp, 3 * sizeof(tcs_unit), SEEK_CUR);    /* layer position color */
                mintime = __min(mintime, buf[0]);
                maxtime = __max(maxtime, buf[1]);
                count ++;
            } else break;
        }
    } else {
        while (1) {
            fread(buf, sizeof(tcs_unit), 3, pFile->fp);    /* startTime endTime layer_and_count */
            if (feof(pFile->fp) == 0) {
                fseek(pFile->fp, (GETCOUNT(buf[2]) << 1) * sizeof(tcs_unit), SEEK_CUR);
                mintime = __min(mintime, buf[0]);
                maxtime = __max(maxtime, buf[1]);
                count ++;
            } else break;
        }
    }
    fsetpos(pFile->fp, &position);    /* reset file position indicator */
    *pMinTime = mintime;
    *pMaxTime = maxtime;
    *pChunkCount = count;
    return tcs_error_success;
}

TCS_Error_Code libtcs_parse_compressed_tcs_file(const TCS_pFile pFile, TCS_pIndex *ppIndex) {
    TCS_Error_Code error;
    fpos_t position;
    TCS_pIndex pIndex;
    TCS_Header header;
    tcs_u8 maxLayer;    /* record the max layer of chunks */
    tcs_u32 i, offset, chunks;
    tcs_unit buf[3];    /* a temp buffer to hold startTime endTime layer_and_count */
    if (!pFile) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);
    if (tcs_error_success != error) {
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
        return error;
    }
    if (TCS_FLAG_COMPRESSED != header.flag) {
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
        return tcs_error_file_type_not_match;
    }
    chunks = header.chunks;    /* get the amount of chunks */
    pIndex = (TCS_pIndex)malloc(chunks * sizeof(TCS_Index));
    maxLayer = (tcs_u8)0;
    offset = sizeof(TCS_Header) >> 2;
    for (i = 0; i < chunks; i ++) {
        fread(buf, sizeof(tcs_unit), 3, pFile->fp);   /* startTime endTime layer_and_count takes up 3 tcs_unit */
        pIndex[i].first = buf[0];    /* startTime */
        pIndex[i].last = buf[1];    /* endTime */
        pIndex[i].layer_and_count = buf[2];
        pIndex[i].offset = offset;
        maxLayer = __max(maxLayer, GETLAYER(pIndex[i].layer_and_count));
        offset += 3 + (GETCOUNT(pIndex[i].layer_and_count) << 1);
        fseek(pFile->fp, (GETCOUNT(pIndex[i].layer_and_count) << 1) * sizeof(tcs_unit), SEEK_CUR);
    }
    fsetpos(pFile->fp, &position);    /* reset file position indicator */
    if (0 != maxLayer) {    /* order chunks by layers */
        tcs_u32 size;
        tcs_u32 *layerChunks, *layerIndex;
        TCS_pIndex pNewIndex;
        size = (maxLayer + 1) * sizeof(tcs_u32);
        layerChunks = (tcs_u32 *)malloc(size);   /* count the number of chunks in each layer */
        memset(layerChunks, 0, size);
        for (i = 0; i < chunks; i ++) {
            layerChunks[GETLAYER(pIndex[i].layer_and_count)] ++;
        }
        layerIndex = (tcs_u32 *)malloc((maxLayer + 1) * sizeof(tcs_u32));    /* index of each layer */
        layerIndex[0] = 0;
        for (i = 1; i <= maxLayer; i ++) {
            layerIndex[i] = layerIndex[i - 1] + layerChunks[i - 1];
        }
        free(layerChunks);
        pNewIndex = (TCS_pIndex)malloc(chunks * sizeof(TCS_Index));
        for (i = 0; i < chunks; i ++) {
            pNewIndex[layerIndex[GETLAYER(pIndex[i].layer_and_count)] ++] = pIndex[i];    /* note: each chunk has a TCS_Index structure */
        }
        free(layerIndex);
        free(pIndex);
        pIndex = pNewIndex;
    }
    *ppIndex = pIndex;
    return tcs_error_success;
}

TCS_Error_Code libtcs_parse_compressed_tcs_file_with_fps(const TCS_pFile pFile, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, TCS_pIndex *ppIndex) {
    TCS_Error_Code error;
    fpos_t position;
    TCS_pIndex pIndex;
    TCS_Header header;
    tcs_u8 maxLayer;    /* record the max layer of chunks */
    tcs_u32 i, offset, chunks;
    tcs_unit buf[3];    /* a temp buffer to hold startTime endTime layer_and_count */
    if (!pFile) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);
    if (tcs_error_success != error) {
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
        return error;
    }
    if (TCS_FLAG_COMPRESSED != header.flag) {
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
        return tcs_error_file_type_not_match;
    }
    chunks = header.chunks;    /* get the amount of chunks */
    pIndex = (TCS_pIndex)malloc(chunks * sizeof(TCS_Index));
    maxLayer = (tcs_u8)0;
    offset = sizeof(TCS_Header) >> 2;
    for (i = 0; i < chunks; i ++) {
        fread(buf, sizeof(tcs_unit), 3, pFile->fp);   /* startTime endTime layer_and_count takes up 3 tcs_unit */
        pIndex[i].first = (tcs_u32)((tcs_s64)buf[0] * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* firstFrame, note: +1 is just intend to make it compatible with VSFilter */
        pIndex[i].last = (tcs_u32)((tcs_s64)buf[1] * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* lastFrame, note: +1 is just intend to make it compatible with VSFilter */
        pIndex[i].layer_and_count = buf[2];
        pIndex[i].offset = offset;
        maxLayer = __max(maxLayer, GETLAYER(pIndex[i].layer_and_count));
        offset += 3 + (GETCOUNT(pIndex[i].layer_and_count) << 1);
        fseek(pFile->fp, (GETCOUNT(pIndex[i].layer_and_count) << 1) * sizeof(tcs_unit), SEEK_CUR);
    }
    fsetpos(pFile->fp, &position);    /* reset file position indicator */
    if (0 != maxLayer) {    /* order chunks by layers */
        tcs_u32 size;
        tcs_u32 *layerChunks, *layerIndex;
        TCS_pIndex pNewIndex;
        size = (maxLayer + 1) * sizeof(tcs_u32);
        layerChunks = (tcs_u32 *)malloc(size);   /* count the number of chunks in each layer */
        memset(layerChunks, 0, size);
        for (i = 0; i < chunks; i ++) {
            layerChunks[GETLAYER(pIndex[i].layer_and_count)] ++;
        }
        layerIndex = (tcs_u32 *)malloc((maxLayer + 1) * sizeof(tcs_u32));    /* index of each layer */
        layerIndex[0] = 0;
        for (i = 1; i <= maxLayer; i ++) {
            layerIndex[i] = layerIndex[i - 1] + layerChunks[i - 1];
        }
        free(layerChunks);
        pNewIndex = (TCS_pIndex)malloc(chunks * sizeof(TCS_Index));
        for (i = 0; i < chunks; i ++) {
            pNewIndex[layerIndex[GETLAYER(pIndex[i].layer_and_count)] ++] = pIndex[i];    /* note: each chunk has a TCS_Index structure */
        }
        free(layerIndex);
        free(pIndex);
        pIndex = pNewIndex;
    }
    *ppIndex = pIndex;
    return tcs_error_success;
}

TCS_Error_Code libtcs_destroy_index(TCS_pIndex pIndex) {
    if (!pIndex) return tcs_error_null_pointer;
    free(pIndex);
    return tcs_error_success;
}

/* high level functions */

TCS_Error_Code libtcs_create_tcs_frame(TCS_pFile pFile, const TCS_pHeader pHeader, const TCS_pIndex pIndex, tcs_u32 n, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, tcs_u16 targetWidth, tcs_u16 targetHeight, tcs_byte **pBuf) {
    TCS_Chunk chunk;
    tcs_u16 width, height;
    tcs_u32 i, t, pitch, size;
    tcs_byte *rgba;
    if (!pFile || !pHeader) return tcs_error_null_pointer;
    width = GETPOSX(pHeader->resolution);
    height = GETPOSY(pHeader->resolution);
    pitch = width << 2;
    size = height * pitch;
    rgba = (tcs_byte *)malloc(size);
    memset(rgba, 0, size);
    if (TCS_FLAG_COMPRESSED == pHeader->flag) {
        if (!pIndex) return tcs_error_null_pointer;
        for (i = 0; i < pHeader->chunks; i ++) {
            if (n >= pIndex[i].first && n < pIndex[i].last) {
                libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[i].offset) << 2, &chunk);
                libtcs_convert_chunks_to_rgba(&chunk, width, height, rgba);
                libtcs_free_chunk(&chunk);
            }
        }
    } else if (TCS_FLAG_PARSED_HIGHEST_LV == pHeader->flag || TCS_FLAG_PARSED_HIGHEST_LV_NONCOMPRESSED == pHeader->flag) {
        if (n <= pFile->temp) libtcs_set_file_position_indicator(pFile, tcs_fpi_header);
        pFile->temp = n;
        t = (tcs_u32)((tcs_s64)n * fpsDenominator * 1000 / fpsNumerator);
        do {
            if (fread(&chunk, sizeof(tcs_unit), 3, pFile->fp) != 3) {
                *pBuf = rgba;
                return tcs_error_success;
            }
            fseek(pFile->fp, GETCOUNT(chunk.layer_and_count) * (sizeof(tcs_unit) << 1), SEEK_CUR);
            if (t < chunk.startTime) {
                fseek(pFile->fp, -(long)(3 + (GETCOUNT(chunk.layer_and_count) << 1)) * sizeof(tcs_unit), SEEK_CUR);
                *pBuf = rgba;
                return tcs_error_success;
            }
        } while (!(t >= chunk.startTime && t < chunk.endTime));
        fseek(pFile->fp, -(long)(3 + (GETCOUNT(chunk.layer_and_count) << 1)) * sizeof(tcs_unit), SEEK_CUR);
        libtcs_read_chunk(pFile, &chunk);
        fseek(pFile->fp, -(long)(3 + (GETCOUNT(chunk.layer_and_count) << 1)) * sizeof(tcs_unit), SEEK_CUR);
        libtcs_convert_chunks_to_rgba(&chunk, width, height, rgba);
        libtcs_free_chunk(&chunk);
    } else return tcs_error_file_type_not_support;
    if (!(width == targetWidth && height == targetHeight)) {
        tcs_byte *src = rgba;
        libtcs_resample_rgba(src, width, height, &rgba, targetWidth, targetHeight);
        free(src);
    }
    *pBuf = rgba;
    return tcs_error_success;
}

/* libtcs_convert_flag series function - convert compressed TCS file to compressed/non-compressed highest level parsed TCS file */
static void _vector_clean_chunks(void *v) {
    unsigned long i, count;
    TCS_pChunk pChunk;
    pChunk = (TCS_pChunk)((Vector *)v)->buffer;
    count = ((Vector *)v)->count;
    for (i = 0; i < count; i ++) {
        free(pChunk[i].pos_and_color);
    }
    free(pChunk);
}

static void _libtcs_convert_chunks_flag_1_to_2_with_ms(TCS_pFile pFile, const TCS_pHeader pHeader, const TCS_pIndex pIndex, const Vector *vi, tcs_unit t, tcs_u8 milliseconds, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u16 width, height;
    tcs_u32 i, index, num, pitch, size;
    tcs_byte *rgba;
    parsedChunk.startTime = t;
    parsedChunk.endTime = t + milliseconds;
    parsedChunk.layer_and_count = MAKECL(0, 0);
    width = GETPOSX(pHeader->resolution);
    height = GETPOSY(pHeader->resolution);
    pitch = width << 2;
    size = height * pitch;
    rgba = (tcs_byte *)malloc(size);
    memset(rgba, 0, size);
    num = vector_get_size(vi);
    for (i = 0; i < num; i ++) {
        vector_retrieve(vi, i, &index);
        libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[index].offset) << 2, &compressedChunk);
        libtcs_convert_chunks_to_rgba(&compressedChunk, width, height, rgba);
        libtcs_free_chunk(&compressedChunk);
    }
    libtcs_convert_rgba_to_chunk(rgba, width, height, &parsedChunk);
    free(rgba);
    *pParsedChunk = parsedChunk;
}

static void _libtcs_convert_chunks_flag_1_to_2_with_fps(TCS_pFile pFile, const TCS_pHeader pHeader, const TCS_pIndex pIndex, const Vector *vi, tcs_u32 frame, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u16 width, height;
    tcs_u32 i, index, num, pitch, size;
    tcs_byte *rgba;
    parsedChunk.startTime = (tcs_u32)((tcs_s64)(frame - 1) * fpsDenominator * 1000 / fpsNumerator);
    parsedChunk.endTime = (tcs_u32)((tcs_s64)frame * fpsDenominator * 1000 / fpsNumerator);
    parsedChunk.layer_and_count = MAKECL(0, 0);
    width = GETPOSX(pHeader->resolution);
    height = GETPOSY(pHeader->resolution);
    pitch = width << 2;
    size = height * pitch;
    rgba = (tcs_byte *)malloc(size);
    memset(rgba, 0, size);
    num = vector_get_size(vi);
    for (i = 0; i < num; i ++) {
        vector_retrieve(vi, i, &index);
        libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[index].offset) << 2, &compressedChunk);
        libtcs_convert_chunks_to_rgba(&compressedChunk, width, height, rgba);
        libtcs_free_chunk(&compressedChunk);
    }
    libtcs_convert_rgba_to_chunk(rgba, width, height, &parsedChunk);
    free(rgba);
    *pParsedChunk = parsedChunk;
}

static void _libtcs_convert_chunks_flag_1_to_3_with_ms(TCS_pFile pFile, const TCS_pIndex pIndex, const Vector *vi, tcs_unit t, tcs_u8 milliseconds, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u32 i, index, num, count, offset;    /* count indicates the amount of packed DIPs in a parsed chunk, offset is used in parsedChunk.pos_and_color */
    Vector vCompChunk;
    vector_init(&vCompChunk, sizeof(TCS_Chunk), 0, vector_default_copy_element, _vector_clean_chunks);    /* low copy */
    count = 0;
    num = vector_get_size(vi);
    for (i = 0; i < num; i ++) {
        vector_retrieve(vi, i, &index);
        libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[index].offset) << 2, &compressedChunk);
        vector_push_back(&vCompChunk, &compressedChunk);    /* note that compressedChunk.pos_and_color is low copy, it'll be freed by _vector_clean_chunks() */
        count += GETCOUNT(compressedChunk.layer_and_count);
    }
    offset = 0;
    parsedChunk.startTime = t;
    parsedChunk.endTime = t + milliseconds;
    parsedChunk.layer_and_count = MAKECL(count, 0);    /* layer of parsedChunk is of no use, and its value is ignored */
    parsedChunk.pos_and_color = (tcs_unit *)malloc(count * (sizeof(tcs_unit) << 1));
    for (i = 0; i < num; i ++) {
        vector_retrieve(&vCompChunk, i, &compressedChunk);
        memcpy(parsedChunk.pos_and_color + offset, 
            compressedChunk.pos_and_color, GETCOUNT(compressedChunk.layer_and_count) * (sizeof(tcs_unit) << 1));
        offset += GETCOUNT(compressedChunk.layer_and_count) << 1;
    }
    vector_clear(&vCompChunk);
    *pParsedChunk = parsedChunk;
}

static void _libtcs_convert_chunks_flag_1_to_3_with_fps(TCS_pFile pFile, const TCS_pIndex pIndex, const Vector *vi, tcs_u32 frame, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u32 i, index, num, count, offset;    /* count indicates the amount of packed DIPs in a parsed chunk, offset is used in parsedChunk.pos_and_color */
    Vector vCompChunk;
    vector_init(&vCompChunk, sizeof(TCS_Chunk), 0, vector_default_copy_element, _vector_clean_chunks);    /* low copy */
    count = 0;
    num = vector_get_size(vi);
    for (i = 0; i < num; i ++) {
        vector_retrieve(vi, i, &index);
        libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[index].offset) << 2, &compressedChunk);
        vector_push_back(&vCompChunk, &compressedChunk);    /* note that compressedChunk.pos_and_color is low copy, it'll be freed by _vector_clean_chunks() */
        count += GETCOUNT(compressedChunk.layer_and_count);
    }
    offset = 0;
    parsedChunk.startTime = (tcs_u32)((tcs_s64)(frame - 1) * fpsDenominator * 1000 / fpsNumerator);
    parsedChunk.endTime = (tcs_u32)((tcs_s64)frame * fpsDenominator * 1000 / fpsNumerator);
    parsedChunk.layer_and_count = MAKECL(count, 0);    /* layer of parsedChunk is of no use, and its value is ignored */
    parsedChunk.pos_and_color = (tcs_unit *)malloc(count * (sizeof(tcs_unit) << 1));
    for (i = 0; i < num; i ++) {
        vector_retrieve(&vCompChunk, i, &compressedChunk);
        memcpy(parsedChunk.pos_and_color + offset, 
            compressedChunk.pos_and_color, GETCOUNT(compressedChunk.layer_and_count) * (sizeof(tcs_unit) << 1));
        offset += GETCOUNT(compressedChunk.layer_and_count) << 1;
    }
    vector_clear(&vCompChunk);
    *pParsedChunk = parsedChunk;
}

TCS_Error_Code libtcs_convert_flag_1_to_2_with_ms(const TCS_pFile pFile, const char *filename, tcs_u8 milliseconds) {
    fpos_t position;
    TCS_File outfile;
    TCS_Header header;    /* both for input and output */
    TCS_Chunk parsedChunk;
    TCS_pIndex pIndex;    /* store the parsed TCS FX chunk index */
    TCS_Error_Code error;
    tcs_bool sameChunks;
    Vector vI;
    Vector vPreI;
    tcs_u32 i, num;
    tcs_unit t, chunks;    /* chunks indicates the amout of chunks in the parsed TCS file */
    if (!pFile || !filename) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);    /* get header of the compressed TCS file */
    if (tcs_error_success != error) return error;
    if (TCS_FLAG_COMPRESSED != header.flag) return tcs_error_file_type_not_match;
    error = libtcs_parse_compressed_tcs_file(pFile, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_file_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    for (i = 0; i < header.chunks; i ++) {
        if (header.minTime >= pIndex[i].first && header.minTime < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _libtcs_convert_chunks_flag_1_to_2_with_ms(pFile, &header, pIndex, &vPreI, header.minTime, milliseconds, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    for (t = header.minTime + 1; t < header.maxTime; t += milliseconds) {
        vector_clear(&vI);
        for (i = 0; i < header.chunks; i ++) {
            if (t >= pIndex[i].first && t < pIndex[i].last) {
                vector_push_back(&vI, &i);
            }
        }
        if (vector_get_size(&vI) == 0) continue;
        sameChunks = TCS_TRUE;
        num = vector_get_size(&vPreI);
        if (vector_get_size(&vI) != num) sameChunks = TCS_FALSE;
        else sameChunks = (vector_compare(&vI, &vPreI, num) == 0);
        vector_clear(&vPreI);
        vector_copy(&vPreI, &vI);
        if (sameChunks) parsedChunk.endTime += milliseconds;
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _libtcs_convert_chunks_flag_1_to_2_with_ms(pFile, &header, pIndex, &vPreI, t, milliseconds, &parsedChunk);
        }
    }
    free(pIndex);
    vector_clear(&vPreI);
    vector_clear(&vI);
    /* write the last parsedChunk to file */
    libtcs_write_chunk(&outfile, &parsedChunk);
    libtcs_free_chunk(&parsedChunk);
    chunks ++;
    header.flag = TCS_FLAG_PARSED_HIGHEST_LV;
    header.chunks = chunks;
    libtcs_write_header(&outfile, &header, 0);
    libtcs_close_file(&outfile);
    fsetpos(pFile->fp, &position);    /* reset file pointer */
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_flag_1_to_2_with_fps(const TCS_pFile pFile, const char *filename, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator) {
    fpos_t position;
    TCS_File outfile;
    TCS_Header header;    /* both for input and output */
    TCS_Chunk parsedChunk;
    TCS_pIndex pIndex;    /* store the parsed TCS FX chunk index */
    TCS_Error_Code error;
    tcs_bool sameChunks;
    Vector vI;
    Vector vPreI;
    tcs_u32 i, num, minFrame, maxFrame;
    tcs_unit t, chunks;    /* chunks indicates the amout of chunks in the parsed TCS file */
    if (!pFile || !filename) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);    /* get header of the compressed TCS file */
    if (tcs_error_success != error) return error;
    if (TCS_FLAG_COMPRESSED != header.flag) return tcs_error_file_type_not_match;
    error = libtcs_parse_compressed_tcs_file_with_fps(pFile, fpsNumerator, fpsDenominator, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_file_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    minFrame = (tcs_u32)((tcs_s64)header.minTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (i = 0; i < header.chunks; i ++) {
        if (minFrame >= pIndex[i].first && minFrame < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _libtcs_convert_chunks_flag_1_to_2_with_fps(pFile, &header, pIndex, &vPreI, minFrame, fpsNumerator, fpsDenominator, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    maxFrame = (tcs_u32)((tcs_s64)header.maxTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (t = minFrame + 1; t < maxFrame; t ++) {
        vector_clear(&vI);
        for (i = 0; i < header.chunks; i ++) {
            if (t >= pIndex[i].first && t < pIndex[i].last) {
                vector_push_back(&vI, &i);
            }
        }
        if (vector_get_size(&vI) == 0) continue;
        sameChunks = TCS_TRUE;
        num = vector_get_size(&vPreI);
        if (vector_get_size(&vI) != num) sameChunks = TCS_FALSE;
        else sameChunks = (vector_compare(&vI, &vPreI, num) == 0);
        vector_clear(&vPreI);
        vector_copy(&vPreI, &vI);
        if (sameChunks) parsedChunk.endTime += (tcs_u32)((tcs_s64)fpsDenominator * 1000 / fpsNumerator);
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _libtcs_convert_chunks_flag_1_to_2_with_fps(pFile, &header, pIndex, &vPreI, t, fpsNumerator, fpsDenominator, &parsedChunk);
        }
    }
    free(pIndex);
    vector_clear(&vPreI);
    vector_clear(&vI);
    /* write the last parsedChunk to file */
    libtcs_write_chunk(&outfile, &parsedChunk);
    libtcs_free_chunk(&parsedChunk);
    chunks ++;
    header.flag = TCS_FLAG_PARSED_HIGHEST_LV;
    header.chunks = chunks;
    header.fpsNumerator = fpsNumerator;
    header.fpsDenominator = fpsDenominator;
    libtcs_write_header(&outfile, &header, 0);
    libtcs_close_file(&outfile);
    fsetpos(pFile->fp, &position);    /* reset file pointer */
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_flag_1_to_3_with_ms(const TCS_pFile pFile, const char *filename, tcs_u8 milliseconds) {
    fpos_t position;
    TCS_File outfile;
    TCS_Header header;    /* both for input and output */
    TCS_Chunk parsedChunk;
    TCS_pIndex pIndex;    /* store the parsed TCS FX chunk index */
    TCS_Error_Code error;
    tcs_bool sameChunks;
    Vector vI;
    Vector vPreI;
    tcs_u32 i, num;
    tcs_unit t, chunks;    /* chunks indicates the amout of chunks in the parsed TCS file */
    if (!pFile || !filename) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);    /* get header of the compressed TCS file */
    if (tcs_error_success != error) return error;
    if (TCS_FLAG_COMPRESSED != header.flag) return tcs_error_file_type_not_match;
    error = libtcs_parse_compressed_tcs_file(pFile, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_file_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    for (i = 0; i < header.chunks; i ++) {
        if (header.minTime >= pIndex[i].first && header.minTime < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _libtcs_convert_chunks_flag_1_to_3_with_ms(pFile, pIndex, &vPreI, header.minTime, milliseconds, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    for (t = header.minTime + 1; t < header.maxTime; t += milliseconds) {
        vector_clear(&vI);
        for (i = 0; i < header.chunks; i ++) {
            if (t >= pIndex[i].first && t < pIndex[i].last) {
                vector_push_back(&vI, &i);
            }
        }
        if (vector_get_size(&vI) == 0) continue;
        sameChunks = TCS_TRUE;
        num = vector_get_size(&vPreI);
        if (vector_get_size(&vI) != num) sameChunks = TCS_FALSE;
        else sameChunks = (vector_compare(&vI, &vPreI, num) == 0);
        vector_clear(&vPreI);
        vector_copy(&vPreI, &vI);
        if (sameChunks) parsedChunk.endTime += milliseconds;
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _libtcs_convert_chunks_flag_1_to_3_with_ms(pFile, pIndex, &vPreI, t, milliseconds, &parsedChunk);
        }
    }
    free(pIndex);
    vector_clear(&vPreI);
    vector_clear(&vI);
    /* write the last parsedChunk to file */
    libtcs_write_chunk(&outfile, &parsedChunk);
    libtcs_free_chunk(&parsedChunk);
    chunks ++;
    header.flag = TCS_FLAG_PARSED_HIGHEST_LV_NONCOMPRESSED;
    header.chunks = chunks;
    libtcs_write_header(&outfile, &header, 0);
    libtcs_close_file(&outfile);
    fsetpos(pFile->fp, &position);    /* reset file pointer */
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_flag_1_to_3_with_fps(const TCS_pFile pFile, const char *filename, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator) {
    fpos_t position;
    TCS_File outfile;
    TCS_Header header;    /* both for input and output */
    TCS_Chunk parsedChunk;
    TCS_pIndex pIndex;    /* store the parsed TCS FX chunk index */
    TCS_Error_Code error;
    tcs_bool sameChunks;
    Vector vI;
    Vector vPreI;
    tcs_u32 i, num, minFrame, maxFrame;
    tcs_unit t, chunks;    /* chunks indicates the amout of chunks in the parsed TCS file */
    if (!pFile || !filename) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    error = libtcs_read_header(pFile, &header, 0);    /* get header of the compressed TCS file */
    if (tcs_error_success != error) return error;
    if (TCS_FLAG_COMPRESSED != header.flag) return tcs_error_file_type_not_match;
    error = libtcs_parse_compressed_tcs_file_with_fps(pFile, fpsNumerator, fpsDenominator, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_file_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    minFrame = (tcs_u32)((tcs_s64)header.minTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (i = 0; i < header.chunks; i ++) {
        if (minFrame >= pIndex[i].first && minFrame < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _libtcs_convert_chunks_flag_1_to_3_with_fps(pFile, pIndex, &vPreI, minFrame, fpsNumerator, fpsDenominator, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, vector_default_copy_element, vector_default_clean_buffer);    /* low copy */
    maxFrame = (tcs_u32)((tcs_s64)header.maxTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (t = minFrame + 1; t < maxFrame; t ++) {
        vector_clear(&vI);
        for (i = 0; i < header.chunks; i ++) {
            if (t >= pIndex[i].first && t < pIndex[i].last) {
                vector_push_back(&vI, &i);
            }
        }
        if (vector_get_size(&vI) == 0) continue;
        sameChunks = TCS_TRUE;
        num = vector_get_size(&vPreI);
        if (vector_get_size(&vI) != num) sameChunks = TCS_FALSE;
        else sameChunks = (vector_compare(&vI, &vPreI, num) == 0);
        vector_clear(&vPreI);
        vector_copy(&vPreI, &vI);
        if (sameChunks) parsedChunk.endTime += (tcs_u32)((tcs_s64)fpsDenominator * 1000 / fpsNumerator);
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _libtcs_convert_chunks_flag_1_to_3_with_fps(pFile, pIndex, &vPreI, t, fpsNumerator, fpsDenominator, &parsedChunk);
        }
    }
    free(pIndex);
    vector_clear(&vPreI);
    vector_clear(&vI);
    /* write the last parsedChunk to file */
    libtcs_write_chunk(&outfile, &parsedChunk);
    libtcs_free_chunk(&parsedChunk);
    chunks ++;
    header.flag = TCS_FLAG_PARSED_HIGHEST_LV_NONCOMPRESSED;
    header.chunks = chunks;
    header.fpsNumerator = fpsNumerator;
    header.fpsDenominator = fpsDenominator;
    libtcs_write_header(&outfile, &header, 0);
    libtcs_close_file(&outfile);
    fsetpos(pFile->fp, &position);    /* reset file pointer */
    return tcs_error_success;
}


