#include "Common.h"
#include <windows.h>


namespace SoundLib {
/* Public Static Functions  ------------------------------------------------------------------------- */
const char* Common::ToChar(const wchar_t* pSrc) {
	// 必要なバイト数の取得
	int charStrLength = WideCharToMultiByte(CP_THREAD_ACP, 0, pSrc, -1, NULL, 0, NULL, NULL);
	char* pCharStr = new char[charStrLength];

	// 変換
	WideCharToMultiByte(CP_THREAD_ACP, 0, pSrc, int(wcslen(pSrc)) + 1, pCharStr, charStrLength, NULL, NULL);

	return	pCharStr;
}

}