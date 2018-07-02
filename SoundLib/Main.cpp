#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "SoundManager2.h"



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



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	//char* filePath = (char*)"Resources\\toujyo.wav";
	char* filePath = (char*)"Resources\\musicbox.mp3";

	SoundManager soundManager;
	soundManager.Initialize();
	
	// Waveファイルオープン
	if (!soundManager.OpenSoundFile(filePath)) {
		return -1;
	}

	if (!soundManager.Start(true)) {
		return -1;
	}

	Sleep(10000);

	//soundManager.Pause();
	//Sleep(1000);
	//soundManager.Resume();

	//Sleep(10000);

	soundManager.Stop();

	return 0;
}
