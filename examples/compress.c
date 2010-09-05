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
    tcs_u32 chunks;
    TCS_pFile pFile;
    TCS_pHeader pHeader;
    TCS_pRawChunk pRawChunk;
    TCS_pChunk pChunk;
    pFile = (TCS_pFile)malloc(sizeof(TCS_File));
    pHeader = (TCS_pHeader)malloc(sizeof(TCS_Header));
    pRawChunk = (TCS_pRawChunk)malloc(4 * sizeof(TCS_RawChunk));
    libtcs_open_file(pFile, "test_raw.tcs", tcs_file_open_existing);
    libtcs_read_header(pFile, pHeader, 0);
    libtcs_read_raw_chunks(pFile, pRawChunk, 4);
    libtcs_close_file(pFile);
    memset(pFile, 0, sizeof(TCS_File));
    libtcs_open_file(pFile, "test.tcs", tcs_file_create_new);
    pFile->minTime = pHeader->minTime;
    pFile->maxTime = pHeader->maxTime;
    libtcs_set_file_position_indicator(pFile, tcs_fpi_header);
    libtcs_convert_raw_chunks(pRawChunk, 4, &pChunk, &chunks);
    pFile->chunks += chunks;
    libtcs_write_chunks(pFile, pChunk, chunks, 1);
    libtcs_set_header(pHeader, TCS_FLAG_COMPRESSED, GETPOSX(pHeader->resolution), GETPOSY(pHeader->resolution), pFile->minTime, pFile->maxTime, pFile->chunks, pHeader->fpsNumerator, pHeader->fpsDenominator);
    libtcs_write_header(pFile, pHeader, 0);
    libtcs_close_file(pFile);
    free(pRawChunk);
    free(pHeader);
    free(pFile);
    return 0;
}



