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

#include "tcs.h"

int main() {
    tcs_u32 rawChunks;
    TCS_pFile pFile;
    TCS_pHeader pHeader;
    TCS_pRawChunk pRawChunk;
    TCS_pChunk pChunk;
    pFile = (TCS_pFile)malloc(sizeof(TCS_File));
    pHeader = (TCS_pHeader)malloc(sizeof(TCS_Header));
    libtcs_open_file(pFile, "test.tcs", tcs_open_existing);
    libtcs_read_header(pFile, pHeader, 0);
    pChunk = (TCS_pChunk)malloc(pHeader->chunks * sizeof(TCS_Chunk));
    libtcs_read_chunks(pFile, pChunk, pHeader->chunks);
    libtcs_close_file(pFile);
    libtcs_open_file(pFile, "test_uncompressed.tcs", tcs_create_new);
    libtcs_convert_chunks(pChunk, pHeader->chunks, &pRawChunk, &rawChunks, 1);
    libtcs_set_header(pHeader, TCS_FLAG_RAW, GETPOSX(pHeader->resolution), GETPOSY(pHeader->resolution), pHeader->minTime, pHeader->maxTime, rawChunks);
    libtcs_write_header(pFile, pHeader, 0);
    libtcs_write_raw_chunks(pFile, pRawChunk, rawChunks);
    libtcs_close_file(pFile);
    free(pChunk);
    free(pRawChunk);
    free(pHeader);
    free(pFile);
    return 0;
}

