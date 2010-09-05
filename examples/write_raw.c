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

#include "../tcs.h"

int main() {
    TCS_pFile pFile;
    TCS_pHeader pHeader;
    TCS_pRawChunk pRawChunk;
    pFile = (TCS_pFile)malloc(sizeof(TCS_File));
    memset(pFile, 0, sizeof(TCS_File));
    pFile->minTime = TCS_INIT_MIN_TIME;
    pHeader = (TCS_pHeader)malloc(sizeof(TCS_Header));
    pRawChunk = (TCS_pRawChunk)malloc(4 * sizeof(TCS_RawChunk));
    pRawChunk[0].startTime = 0;
    pRawChunk[0].endTime = 100;
    pRawChunk[0].layer = 5;
    pRawChunk[0].posX = 100;
    pRawChunk[0].posY = 100;
    pRawChunk[0].r = (tcs_byte)1;
    pRawChunk[0].g = (tcs_byte)2;
    pRawChunk[0].b = (tcs_byte)3;
    pRawChunk[0].a = (tcs_byte)4;
    pFile->minTime = __min(pFile->minTime, pRawChunk[0].startTime);
    pFile->maxTime = __max(pFile->maxTime, pRawChunk[0].endTime);
    pFile->chunks ++;
    pRawChunk[1].startTime = 200;
    pRawChunk[1].endTime = 300;
    pRawChunk[1].layer = 1;
    pRawChunk[1].posX = 200;
    pRawChunk[1].posY = 100;
    pRawChunk[1].r = (tcs_byte)5;
    pRawChunk[1].g = (tcs_byte)6;
    pRawChunk[1].b = (tcs_byte)7;
    pRawChunk[1].a = (tcs_byte)8;
    pFile->minTime = __min(pFile->minTime, pRawChunk[1].startTime);
    pFile->maxTime = __max(pFile->maxTime, pRawChunk[1].endTime);
    pFile->chunks ++;
    pRawChunk[2].startTime = 400;
    pRawChunk[2].endTime = 500;
    pRawChunk[2].layer = 3;
    pRawChunk[2].posX = 100;
    pRawChunk[2].posY = 200;
    pRawChunk[2].r = (tcs_byte)1;
    pRawChunk[2].g = (tcs_byte)2;
    pRawChunk[2].b = (tcs_byte)3;
    pRawChunk[2].a = (tcs_byte)4;
    pFile->minTime = __min(pFile->minTime, pRawChunk[2].startTime);
    pFile->maxTime = __max(pFile->maxTime, pRawChunk[2].endTime);
    pFile->chunks ++;
    pRawChunk[3].startTime = 400;
    pRawChunk[3].endTime = 500;
    pRawChunk[3].layer = 3;
    pRawChunk[3].posX = 200;
    pRawChunk[3].posY = 200;
    pRawChunk[3].r = (tcs_byte)1;
    pRawChunk[3].g = (tcs_byte)2;
    pRawChunk[3].b = (tcs_byte)3;
    pRawChunk[3].a = (tcs_byte)4;
    pFile->minTime = __min(pFile->minTime, pRawChunk[3].startTime);
    pFile->maxTime = __max(pFile->maxTime, pRawChunk[3].endTime);
    pFile->chunks ++;
    libtcs_open_file(pFile, "test_raw.tcs", tcs_file_create_new);
    libtcs_set_file_position_indicator(pFile, tcs_fpi_header);
    libtcs_write_raw_chunks(pFile, pRawChunk, 4);
    libtcs_set_header(pHeader, TCS_FLAG_RAW, 640, 480, pFile->minTime, pFile->maxTime, pFile->chunks, 1, 1);
    libtcs_write_header(pFile, pHeader, 1);
    libtcs_close_file(pFile);
    free(pRawChunk);
    free(pHeader);
    free(pFile);
    return 0;
}







