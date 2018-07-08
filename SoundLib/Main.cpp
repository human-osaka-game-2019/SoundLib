#include <stdint.h>
#include <windows.h>
#include "Common.h"
#include "SoundsManager.h"


SoundLib::SoundsManager soundsManager;

static void OnPlayedToEnd(const TCHAR* pKey);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// メモリリークチェック
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	
	const TCHAR* filePath1 = _T("Resources\\toujyo.wav");
	const TCHAR* filePath2 = _T("Resources\\musicbox.mp3");
	const TCHAR* filePath3 = _T("Resources\\jump03.mp3");
	const TCHAR* filePath4 = _T("Resources\\bgm_maoudamashii_fantasy10.ogg");
	const TCHAR* filePath5 = _T("Resources\\Sample_BeeMoved_48kHz16bit.m4a");
	const TCHAR* filePath6 = _T("Resources\\s3.wma");

	//SoundsManager soundsManager;
	soundsManager.Initialize();
	
	// Waveファイルオープン
	if (!soundsManager.AddFile(filePath1, _T("wav"))) {
		return -1;
	}

	// mp3ファイルオープン
	if (!soundsManager.AddFile(filePath2, _T("mp3"))) {
		return -1;
	}

	// mp3ファイルオープン
	if (!soundsManager.AddFile(filePath3, _T("mp3SE"))) {
		return -1;
	}

	// oggファイルオープン
	if (!soundsManager.AddFile(filePath4, _T("ogg"))) {
		return -1;
	}

	// AACファイルオープン
	if (!soundsManager.AddFile(filePath5, _T("aac"))) {
		return -1;
	}

	// wmaファイルオープン
	/*
	if (!soundsManager.AddFile(filePath6, _T("wma"))) {
		return -1;
	}
	*/

	SoundLib::PlayingStatus status = soundsManager.GetStatus(_T("aac"));
	OutputDebugStringEx(_T("Status of the aac is %d\n"), status);

	soundsManager.Start(_T("aac"), false);
	Sleep(1000);
	OutputDebugStringEx(_T("Status of the aac is %d\n"), soundsManager.GetStatus(_T("aac")));

	for (int i = 0; i < 1; ++i) {
		soundsManager.Start(_T("mp3SE"));
		Sleep(1500);
	}

	//Sleep(1000);
	soundsManager.Pause(_T("aac"));
	OutputDebugStringEx(_T("Status of the aac is %d\n"), soundsManager.GetStatus(_T("aac")));
	Sleep(1000);


	soundsManager.Resume(_T("aac"));
	OutputDebugStringEx(_T("Status of the aac is %d\n"), soundsManager.GetStatus(_T("aac")));
	Sleep(20000);

	soundsManager.Stop(_T("aac"));
	OutputDebugStringEx(_T("Status of the aac is %d\n"), soundsManager.GetStatus(_T("aac")));

	Sleep(1000);
	soundsManager.Start(_T("aac"), OnPlayedToEnd);
	OutputDebugStringEx(_T("Status of the aac is %d\n"), soundsManager.GetStatus(_T("aac")));

	Sleep(120000);

	return 0;
}

static void OnPlayedToEnd(const TCHAR* pKey) {
	soundsManager.Start(_T("mp3"));
}
