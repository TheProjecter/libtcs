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
    TCS_File file;
    TCS_Header header;
    tcs_unit minTime, maxTime, chunks;
    if (libtcs_open_file(&file, "test.tcs", tcs_file_read_write) != tcs_error_success) return -1;
    libtcs_get_min_max_time_and_chunks(&file, &minTime, &maxTime, &chunks);
    libtcs_set_header(&header, TCS_FLAG_COMPRESSED, 1280, 720, minTime, maxTime, chunks, 23976, 1000);
    libtcs_write_header(&file, &header, 0);
    libtcs_close_file(&file);
    return 0;
}
