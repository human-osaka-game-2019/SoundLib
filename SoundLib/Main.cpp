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

	SoundLib::PlayingStatus status = soundsManager.GetStatus(_T("ogg"));
	OutputDebugStringEx(_T("Status of the ogg is %d\n"), status);

	soundsManager.Start(_T("ogg"), OnPlayedToEnd);
	Sleep(1000);
	OutputDebugStringEx(_T("Status of the ogg is %d\n"), soundsManager.GetStatus(_T("ogg")));

	for (int i = 0; i < 1; ++i) {
		soundsManager.Start(_T("mp3SE"));
		Sleep(1500);
	}

	//Sleep(1000);
	soundsManager.Pause(_T("ogg"));
	OutputDebugStringEx(_T("Status of the ogg is %d\n"), soundsManager.GetStatus(_T("ogg")));
	Sleep(1000);


	soundsManager.Resume(_T("ogg"));
	OutputDebugStringEx(_T("Status of the ogg is %d\n"), soundsManager.GetStatus(_T("ogg")));
	Sleep(1000);

	//soundsManager.Stop(_T("wav"));
	OutputDebugStringEx(_T("Status of the ogg is %d\n"), soundsManager.GetStatus(_T("ogg")));

	Sleep(10000);
	OutputDebugStringEx(_T("Status of the ogg is %d\n"), soundsManager.GetStatus(_T("ogg")));

	return 0;
}

static void OnPlayedToEnd(const TCHAR* pKey) {
	soundsManager.Start(_T("mp3"));
}
