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
#include <windows.h>

int main() {
    TCS_File file;
    DWORD dw;
    dw = GetTickCount();
    if (libtcs_open_file(&file, "compressed.tcs", tcs_file_open_existing) != tcs_error_success) return 0;
    libtcs_convert_flag_1_to_2_with_fps(&file, "parsed.tcs", 30000, 1001);
    libtcs_close_file(&file);
    printf("%i", GetTickCount() - dw);
    system("PAUSE");
    return 0;
}


