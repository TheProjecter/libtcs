/*
 *  Copyright (C) 2009-2010 milkyjing <milkyjing@gmail.com>
 *
 *  milkyjing
 *
 */

#include "tcs.h"
#include "vector.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)    /* disable warnings for fopen function() */
#endif    /* _MSC_VER */


TCS_Error_Code libtcs_open_file(TCS_pFile pFile, const char *filename, TCS_Open_Type type) {
    if (!pFile) return tcs_error_null_pointer;
    if (tcs_open_existing == type) {
        pFile->fp = fopen(filename, "rb");    /* file should open in binary mode */
        if (!pFile->fp) return tcs_error_file_cant_open;
    } else if (tcs_create_new == type) {
        pFile->fp = fopen(filename, "wb");    /* file should open in binary mode */
        if (!pFile->fp) return tcs_error_file_cant_create;
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
    fpos_t position;
    if (!pFile || !pHeader) return tcs_error_null_pointer;
    if (keepPosition) {
        fgetpos(pFile->fp, &position);    /* remember file position indicator */
        fseek(pFile->fp, 0, SEEK_SET);
        fread(pHeader, sizeof(tcs_unit), sizeof(TCS_Header) >> 2, pFile->fp);
        fsetpos(pFile->fp, &position);    /* reset file position indicator */
    } else {
        fseek(pFile->fp, 0, SEEK_SET);
        fread(pHeader, sizeof(tcs_unit), sizeof(TCS_Header) >> 2, pFile->fp);
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
        if (fwrite(pHeader, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_writing;
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
    if (fread(pChunk->pos_and_color, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_reading;
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
    if (fread(pChunk->pos_and_color, sizeof(tcs_unit), count, pFile->fp) != count) return tcs_error_file_while_reading;
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

TCS_Error_Code libtcs_compress_raw_chunks(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, tcs_unit **pBuf, tcs_u32 *chunks, tcs_u32 *units) {
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
    realloc(compBuf, count * sizeof(tcs_unit));    /* sweep no-used memory */
    *pBuf = compBuf;
    *chunks = rawChunks - compCount;
    *units = count;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_raw_chunks(const TCS_pRawChunk pRawChunk, tcs_u32 rawChunks, TCS_pChunk *ppChunk, tcs_u32 *chunks) {
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
    *chunks = compChunks;
    return tcs_error_success;
}

TCS_Error_Code libtcs_uncompress_chunk(const TCS_pChunk pChunk, tcs_unit **pBuf, tcs_u32 *rawChunks, tcs_u32 *units) {
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
    *rawChunks = count;
    *units = size >> 2;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_chunk(const TCS_pChunk pChunk, TCS_pRawChunk *ppRawChunk, tcs_u32 *rawChunks) {
    tcs_u32 units;
    tcs_unit *buf;
    TCS_Error_Code error;
    error = libtcs_uncompress_chunk(pChunk, &buf, rawChunks, &units);
    if (tcs_error_success != error) return error;
    *ppRawChunk = (TCS_pRawChunk)buf;
    return tcs_error_success;
}

TCS_Error_Code libtcs_uncompress_chunks(TCS_pChunk pChunk, tcs_u32 chunks, tcs_unit **pBuf, tcs_u32 *rawChunks, tcs_u32 *units, tcs_bool freeChunks) {
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
    *rawChunks = count;
    *units = size >> 2;
    return tcs_error_success;
}

TCS_Error_Code libtcs_convert_chunks(TCS_pChunk pChunk, tcs_u32 chunks, TCS_pRawChunk *ppRawChunk, tcs_u32 *rawChunks, tcs_bool freeChunks) {
    tcs_u32 units;
    tcs_unit *buf;
    TCS_Error_Code error;
    error = libtcs_uncompress_chunks(pChunk, chunks, &buf, rawChunks, &units, freeChunks);
    if (tcs_error_success != error) return error;
    *ppRawChunk = (TCS_pRawChunk)buf;
    return tcs_error_success;
}

TCS_Error_Code libtcs_count_chunks(const TCS_pFile pFile, tcs_unit *chunks) {
    fpos_t position;
    tcs_u32 count;    /* the same as *chunks */
    tcs_unit buf[1];    /* to hold layer_and_count */
    TCS_Header header;
    if (!pFile) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    libtcs_read_header(pFile, &header, 0);
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
    *chunks = count;
    return tcs_error_success;
}

TCS_Error_Code libtcs_get_min_max_time_and_chunks(const TCS_pFile pFile, tcs_unit *minTime, tcs_unit *maxTime, tcs_unit *chunks) {
    fpos_t position;
    tcs_u32 mintime, maxtime, count;    /* count is the same as *chunks */
    tcs_unit buf[3];    /* to hold startTime endTime layer_and_count */
    TCS_Header header;
    if (!pFile) return tcs_error_null_pointer;
    mintime = TCS_INIT_MIN_TIME;
    maxtime = TCS_INIT_MAX_TIME;
    count = 0;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    libtcs_read_header(pFile, &header, 0);
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
    *minTime = mintime;
    *maxTime = maxtime;
    *chunks = count;
    return tcs_error_success;
}

TCS_Error_Code libtcs_parse_compressed_tcs_file(const TCS_pFile pFile, TCS_pIndex *ppIndex) {
    fpos_t position;
    TCS_pIndex pIndex;
    TCS_Header header;
    tcs_u8 maxLayer;    /* record the max layer of chunks */
    tcs_u32 i, offset, chunks;
    tcs_unit buf[3];    /* a temp buffer to hold startTime endTime layer_and_count */
    if (!pFile) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    libtcs_read_header(pFile, &header, 0);
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

TCS_Error_Code libtcs_parse_compressed_tcs_file_with_fps(const TCS_pFile pFile, TCS_pIndex *ppIndex) {
    fpos_t position;
    TCS_pIndex pIndex;
    TCS_Header header;
    tcs_u8 maxLayer;    /* record the max layer of chunks */
    tcs_u32 i, offset, chunks;
    tcs_unit buf[3];    /* a temp buffer to hold startTime endTime layer_and_count */
    if (!pFile) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    libtcs_read_header(pFile, &header, 0);
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
        pIndex[i].first = (tcs_u32)((__int64)buf[0] * header.fpsNumerator / (header.fpsDenominator * 1000)) + 1;    /* firstFrame, note: +1 is just intend to make it compatible with VSFilter */
        pIndex[i].last = (tcs_u32)((__int64)buf[1] * header.fpsNumerator / (header.fpsDenominator * 1000)) + 1;    /* lastFrame, note: +1 is just intend to make it compatible with VSFilter */
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

TCS_Error_Code libtcs_parse_compressed_tcs_file_with_user_fps(const TCS_pFile pFile, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, TCS_pIndex *ppIndex) {
    fpos_t position;
    TCS_pIndex pIndex;
    TCS_Header header;
    tcs_u8 maxLayer;    /* record the max layer of chunks */
    tcs_u32 i, offset, chunks;
    tcs_unit buf[3];    /* a temp buffer to hold startTime endTime layer_and_count */
    if (!pFile) return tcs_error_null_pointer;
    fgetpos(pFile->fp, &position);    /* remember file position indicator */
    libtcs_read_header(pFile, &header, 0);
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
        pIndex[i].first = (tcs_u32)((__int64)buf[0] * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* firstFrame, note: +1 is just intend to make it compatible with VSFilter */
        pIndex[i].last = (tcs_u32)((__int64)buf[1] * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* lastFrame, note: +1 is just intend to make it compatible with VSFilter */
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

TCS_Error_Code libtcs_create_tcs_frame(TCS_pFile pFile, const TCS_pHeader pHeader, const TCS_pIndex pIndex, int pitch, tcs_u32 n, tcs_byte **pBuf) {
    TCS_Chunk chunk;
    tcs_u32 i, j, size;
    tcs_u16 x, y, xx, yy;
    tcs_byte r, g, b, a, r0, g0, b0, a0, A;
    tcs_byte *src;
    if (!pFile || !pHeader || !pIndex) return tcs_error_null_pointer;
    size = GETPOSY(pHeader->resolution) * pitch;
    src = (tcs_byte *)malloc(size);
    memset(src, 0, size);
    for (i = 0; i < pHeader->chunks; i ++) {
        if (n >= pIndex[i].first && n < pIndex[i].last) {
            libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[i].offset) << 2, &chunk);
            for (j = 0; j < GETCOUNT(chunk.layer_and_count); j ++) {
                x = GETPOSX(chunk.pos_and_color[j << 1]);
                y = GETPOSY(chunk.pos_and_color[j << 1]);
                if (x >= GETPOSX(pHeader->resolution) || y >= GETPOSY(pHeader->resolution)) continue;
                r = GETR(chunk.pos_and_color[(j << 1) + 1]);
                g = GETG(chunk.pos_and_color[(j << 1) + 1]);
                b = GETB(chunk.pos_and_color[(j << 1) + 1]);
                a = GETA(chunk.pos_and_color[(j << 1) + 1]);
                yy = y * pitch;
                xx = x << 2;
                r0 = src[yy + xx];
                g0 = src[yy + xx + 1];
                b0 = src[yy + xx + 2];
                a0 = src[yy + xx + 3];
                A = 255 - (255 - a) * (255 - a0) / 255;
                if (0 == A) continue;
                src[yy + xx]     = (r * a + r0 * a0 * (255 - a) / 255) / A;
                src[yy + xx + 1] = (g * a + g0 * a0 * (255 - a) / 255) / A;
                src[yy + xx + 2] = (b * a + b0 * a0 * (255 - a) / 255) / A;
                src[yy + xx + 3] =  A;
            }
            libtcs_free_chunk(&chunk);
        }
    }
    *pBuf = src;
    return tcs_error_success;
}

/* libtcs_convert_flag series function - convert compressed TCS file to compressed/non-compressed highest level parsed TCS file */

void _vector_clean_buf(void *v) {
    free(((Vector *)v)->pBuf);
}

void _vector_clean_chunks(void *v) {
    unsigned long i, count;
    TCS_pChunk pChunk;
    pChunk = (TCS_pChunk)((Vector *)v)->pBuf;
    count = ((Vector *)v)->count;
    for (i = 0; i < count; i ++) {
        free(pChunk[i].pos_and_color);
    }
    free(pChunk);
}

tcs_u32 _blend_color(tcs_u32 back, tcs_u32 over) {
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

void _convert_chunks_flag_1_to_3_with_param(TCS_pFile pFile, const TCS_pIndex pIndex, const Vector *vi, tcs_unit t, tcs_u8 milliseconds, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u32 i, index, num, count, offset;    /* count indicates the amount of packed DIPs in a parsed chunk, offset is used in parsedChunk.pos_and_color */
    Vector vCompChunk;
    vector_init(&vCompChunk, sizeof(TCS_Chunk), 0, _vector_clean_chunks);
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

void _convert_chunks_flag_1_to_3_with_user_fps(TCS_pFile pFile, const TCS_pIndex pIndex, const Vector *vi, tcs_u32 frame, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u32 i, index, num, count, offset;    /* count indicates the amount of packed DIPs in a parsed chunk, offset is used in parsedChunk.pos_and_color */
    Vector vCompChunk;
    vector_init(&vCompChunk, sizeof(TCS_Chunk), 0, _vector_clean_chunks);
    count = 0;
    num = vector_get_size(vi);
    for (i = 0; i < num; i ++) {
        vector_retrieve(vi, i, &index);
        libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[index].offset) << 2, &compressedChunk);
        vector_push_back(&vCompChunk, &compressedChunk);    /* note that compressedChunk.pos_and_color is low copy, it'll be freed by _vector_clean_chunks() */
        count += GETCOUNT(compressedChunk.layer_and_count);
    }
    offset = 0;
    parsedChunk.startTime = (tcs_u32)((__int64)(frame - 1) * fpsDenominator * 1000 / fpsNumerator);
    parsedChunk.endTime = (tcs_u32)((__int64)frame * fpsDenominator * 1000 / fpsNumerator);
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

void _convert_chunks_flag_1_to_2_with_param(TCS_pFile pFile, const TCS_pIndex pIndex, const Vector *vi, tcs_unit t, tcs_u8 milliseconds, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u32 i, j, index, num, count, offset;    /* count indicates the amount of packed DIPs in a parsed chunk, offset is used in parsedChunk.pos_and_color */
    tcs_unit *buf;    /* memory buffer to hold compressed parsed chunk */
    Vector vCompChunk;
    vector_init(&vCompChunk, sizeof(TCS_Chunk), 0, _vector_clean_chunks);
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
    parsedChunk.pos_and_color = (tcs_unit *)malloc(count * (sizeof(tcs_unit) << 1));
    for (i = 0; i < num; i ++) {
        vector_retrieve(&vCompChunk, i, &compressedChunk);
        memcpy(parsedChunk.pos_and_color + offset, 
            compressedChunk.pos_and_color, GETCOUNT(compressedChunk.layer_and_count) * (sizeof(tcs_unit) << 1));
        offset += GETCOUNT(compressedChunk.layer_and_count) << 1;
    }
    vector_clear(&vCompChunk);
    buf = (tcs_unit *)malloc(count * (sizeof(tcs_unit) << 1));
    num = count;
    count = 0;
    index = 0;
    for (i = 0; i < num; i ++) {
        if (TCS_INVALID_POS == parsedChunk.pos_and_color[i << 1]) continue;
        buf[index << 1] = parsedChunk.pos_and_color[i << 1];    /* pos */
        buf[(index << 1) + 1] = parsedChunk.pos_and_color[(i << 1) + 1];    /* color */
        for (j = i + 1; j < num; j ++) {
            if (parsedChunk.pos_and_color[i << 1] == parsedChunk.pos_and_color[j << 1]) {
                buf[(index << 1) + 1] = _blend_color(buf[(index << 1) + 1], parsedChunk.pos_and_color[(j << 1) + 1]);
                parsedChunk.pos_and_color[j << 1] = TCS_INVALID_POS;
            }
        }
        count ++;
        index ++;
    }
    free(parsedChunk.pos_and_color);
    parsedChunk.layer_and_count = MAKECL(count, 0);    /* layer of parsedChunk is of no use, and its value is ignored */
    parsedChunk.pos_and_color = (tcs_unit *)realloc(buf, count * (sizeof(tcs_unit) << 1));
    *pParsedChunk = parsedChunk;
}

void _convert_chunks_flag_1_to_2_with_user_fps(TCS_pFile pFile, const TCS_pIndex pIndex, const Vector *vi, tcs_u32 frame, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator, TCS_pChunk pParsedChunk) {
    TCS_Chunk compressedChunk;
    TCS_Chunk parsedChunk;
    tcs_u32 i, j, index, num, count, offset;    /* count indicates the amount of packed DIPs in a parsed chunk, offset is used in parsedChunk.pos_and_color */
    tcs_unit *buf;    /* memory buffer to hold compressed parsed chunk */
    Vector vCompChunk;
    vector_init(&vCompChunk, sizeof(TCS_Chunk), 0, _vector_clean_chunks);
    count = 0;
    num = vector_get_size(vi);
    for (i = 0; i < num; i ++) {
        vector_retrieve(vi, i, &index);
        libtcs_read_specified_chunk(pFile, ((tcs_s64)pIndex[index].offset) << 2, &compressedChunk);
        vector_push_back(&vCompChunk, &compressedChunk);    /* note that compressedChunk.pos_and_color is low copy, it'll be freed by _vector_clean_chunks() */
        count += GETCOUNT(compressedChunk.layer_and_count);
    }
    offset = 0;
    parsedChunk.startTime = (tcs_u32)((__int64)(frame - 1) * fpsDenominator * 1000 / fpsNumerator);
    parsedChunk.endTime = (tcs_u32)((__int64)frame * fpsDenominator * 1000 / fpsNumerator);
    parsedChunk.pos_and_color = (tcs_unit *)malloc(count * (sizeof(tcs_unit) << 1));
    for (i = 0; i < num; i ++) {
        vector_retrieve(&vCompChunk, i, &compressedChunk);
        memcpy(parsedChunk.pos_and_color + offset, 
            compressedChunk.pos_and_color, GETCOUNT(compressedChunk.layer_and_count) * (sizeof(tcs_unit) << 1));
        offset += GETCOUNT(compressedChunk.layer_and_count) << 1;
    }
    vector_clear(&vCompChunk);
    buf = (tcs_unit *)malloc(count * (sizeof(tcs_unit) << 1));
    num = count;
    count = 0;
    index = 0;
    for (i = 0; i < num; i ++) {
        if (TCS_INVALID_POS == parsedChunk.pos_and_color[i << 1]) continue;
        buf[index << 1] = parsedChunk.pos_and_color[i << 1];    /* pos */
        buf[(index << 1) + 1] = parsedChunk.pos_and_color[(i << 1) + 1];    /* color */
        for (j = i + 1; j < num; j ++) {
            if (parsedChunk.pos_and_color[i << 1] == parsedChunk.pos_and_color[j << 1]) {
                buf[(index << 1) + 1] = _blend_color(buf[(index << 1) + 1], parsedChunk.pos_and_color[(j << 1) + 1]);
                parsedChunk.pos_and_color[j << 1] = TCS_INVALID_POS;
            }
        }
        count ++;
        index ++;
    }
    free(parsedChunk.pos_and_color);
    parsedChunk.layer_and_count = MAKECL(count, 0);    /* layer of parsedChunk is of no use, and its value is ignored */
    parsedChunk.pos_and_color = (tcs_unit *)realloc(buf, count * (sizeof(tcs_unit) << 1));
    *pParsedChunk = parsedChunk;
}

TCS_Error_Code libtcs_convert_flag_1_to_2(const TCS_pFile pFile, const char *filename) {
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
    error = libtcs_parse_compressed_tcs_file_with_fps(pFile, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, _vector_clean_buf);
    minFrame = (tcs_u32)((__int64)header.minTime * header.fpsNumerator / (header.fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (i = 0; i < header.chunks; i ++) {
        if (minFrame >= pIndex[i].first && minFrame < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _convert_chunks_flag_1_to_2_with_user_fps(pFile, pIndex, &vPreI, minFrame, header.fpsNumerator, header.fpsDenominator, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, _vector_clean_buf);
    maxFrame = (tcs_u32)((__int64)header.maxTime * header.fpsNumerator / (header.fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
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
        if (sameChunks) parsedChunk.endTime += (tcs_u32)((__int64)header.fpsDenominator * 1000 / header.fpsNumerator);
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _convert_chunks_flag_1_to_2_with_user_fps(pFile, pIndex, &vPreI, t, header.fpsNumerator, header.fpsDenominator, &parsedChunk);
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

TCS_Error_Code libtcs_convert_flag_1_to_2_with_param(const TCS_pFile pFile, const char *filename, tcs_u8 milliseconds) {
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
    error = libtcs_open_file(&outfile, filename, tcs_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, _vector_clean_buf);
    for (i = 0; i < header.chunks; i ++) {
        if (header.minTime >= pIndex[i].first && header.minTime < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _convert_chunks_flag_1_to_2_with_param(pFile, pIndex, &vPreI, header.minTime, milliseconds, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, _vector_clean_buf);
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
            _convert_chunks_flag_1_to_2_with_param(pFile, pIndex, &vPreI, t, milliseconds, &parsedChunk);
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

TCS_Error_Code libtcs_convert_flag_1_to_2_with_user_fps(const TCS_pFile pFile, const char *filename, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator) {
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
    error = libtcs_parse_compressed_tcs_file_with_user_fps(pFile, fpsNumerator, fpsDenominator, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, _vector_clean_buf);
    minFrame = (tcs_u32)((__int64)header.minTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (i = 0; i < header.chunks; i ++) {
        if (minFrame >= pIndex[i].first && minFrame < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _convert_chunks_flag_1_to_2_with_user_fps(pFile, pIndex, &vPreI, minFrame, fpsNumerator, fpsDenominator, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, _vector_clean_buf);
    maxFrame = (tcs_u32)((__int64)header.maxTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
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
        if (sameChunks) parsedChunk.endTime += (tcs_u32)((__int64)fpsDenominator * 1000 / fpsNumerator);
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _convert_chunks_flag_1_to_2_with_user_fps(pFile, pIndex, &vPreI, t, fpsNumerator, fpsDenominator, &parsedChunk);
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

TCS_Error_Code libtcs_convert_flag_1_to_3(const TCS_pFile pFile, const char *filename) {
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
    error = libtcs_parse_compressed_tcs_file_with_fps(pFile, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, _vector_clean_buf);
    minFrame = (tcs_u32)((__int64)header.minTime * header.fpsNumerator / (header.fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (i = 0; i < header.chunks; i ++) {
        if (minFrame >= pIndex[i].first && minFrame < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _convert_chunks_flag_1_to_3_with_user_fps(pFile, pIndex, &vPreI, minFrame, header.fpsNumerator, header.fpsDenominator, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, _vector_clean_buf);
    maxFrame = (tcs_u32)((__int64)header.maxTime * header.fpsNumerator / (header.fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
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
        if (sameChunks) parsedChunk.endTime += (tcs_u32)((__int64)header.fpsDenominator * 1000 / header.fpsNumerator);
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _convert_chunks_flag_1_to_3_with_user_fps(pFile, pIndex, &vPreI, t, header.fpsNumerator, header.fpsDenominator, &parsedChunk);
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

TCS_Error_Code libtcs_convert_flag_1_to_3_with_param(const TCS_pFile pFile, const char *filename, tcs_u8 milliseconds) {
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
    error = libtcs_open_file(&outfile, filename, tcs_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, _vector_clean_buf);
    for (i = 0; i < header.chunks; i ++) {
        if (header.minTime >= pIndex[i].first && header.minTime < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _convert_chunks_flag_1_to_3_with_param(pFile, pIndex, &vPreI, header.minTime, milliseconds, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, _vector_clean_buf);
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
            _convert_chunks_flag_1_to_3_with_param(pFile, pIndex, &vPreI, t, milliseconds, &parsedChunk);
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

TCS_Error_Code libtcs_convert_flag_1_to_3_with_user_fps(const TCS_pFile pFile, const char *filename, tcs_u32 fpsNumerator, tcs_u32 fpsDenominator) {
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
    error = libtcs_parse_compressed_tcs_file_with_user_fps(pFile, fpsNumerator, fpsDenominator, &pIndex);    /* get parsed TCS Index of the compressed TCS file */
    if (tcs_error_success != error) return error;
    error = libtcs_open_file(&outfile, filename, tcs_create_new);   /* create the output TCS file to store parsed chunks */
    if (tcs_error_success != error) return error;
    libtcs_set_file_position_indicator(&outfile, tcs_fpi_header);
    /* get the very first parsed TCS chunk */
    chunks = 0;
    vector_init(&vPreI, sizeof(tcs_u32), 0, _vector_clean_buf);
    minFrame = (tcs_u32)((__int64)header.minTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
    for (i = 0; i < header.chunks; i ++) {
        if (minFrame >= pIndex[i].first && minFrame < pIndex[i].last) {
            vector_push_back(&vPreI, &i);
        }
    }
    _convert_chunks_flag_1_to_3_with_user_fps(pFile, pIndex, &vPreI, minFrame, fpsNumerator, fpsDenominator, &parsedChunk);
    /* convert other chunks */
    vector_init(&vI, sizeof(tcs_u32), 0, _vector_clean_buf);
    maxFrame = (tcs_u32)((__int64)header.maxTime * fpsNumerator / (fpsDenominator * 1000)) + 1;    /* note: +1 is just intend to make it compatible with VSFilter */
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
        if (sameChunks) parsedChunk.endTime += (tcs_u32)((__int64)fpsDenominator * 1000 / fpsNumerator);
        else {
            libtcs_write_chunk(&outfile, &parsedChunk);
            libtcs_free_chunk(&parsedChunk);
            chunks ++;
            _convert_chunks_flag_1_to_3_with_user_fps(pFile, pIndex, &vPreI, t, fpsNumerator, fpsDenominator, &parsedChunk);
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


