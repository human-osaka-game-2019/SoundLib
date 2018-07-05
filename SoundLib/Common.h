#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <tchar.h>


#ifdef _DEBUG
#define OutputDebugStringEx( str, ... ) \
      { \
        TCHAR c[256]; \
        _stprintf_s( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#    define OutputDebugString( str, ... ) // 空実装
#endif

#endif
