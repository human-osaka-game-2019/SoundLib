//----------------------------------------------------------
// <filename>Common.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <tchar.h>
#include <string>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef _DEBUG
#define OutputDebugStringEx( str, ... ) \
      { \
        TCHAR c[256]; \
        _stprintf_s( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#define OutputDebugStringEx( str, ... ) // 空実装
#endif


namespace SoundLib {

/// <summary>
/// TCHAR格納用Stringクラス
/// </summary>
typedef std::basic_string<TCHAR> TString;

}

#endif
