#include <windows.h>
#include <crtdbg.h>
#include "SoundsManager.h"


#define OutputDebugStringEx(str, ...) \
      { \
        TCHAR c[256]; \
        _stprintf_s(c, str, __VA_ARGS__); \
        OutputDebugString(c); \
      }

static SoundLib::SoundsManager soundsManager;
static std::vector<std::basic_string<TCHAR>> keys;
static int currentKeysPos = 0;

static void OnPlayedToEnd(const TCHAR* pKey);
static void TestPlaying(std::basic_string<TCHAR> key);
static void PrintStatus(std::basic_string<TCHAR> key);
static bool hasFinishedOneFIle = false;;
static bool isTakingTest;
static bool isSuccess;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#if _DEBUG
	// メモリリークチェック
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

	isSuccess = soundsManager.Initialize();

	// Resourcesフォルダに入っている音声ファイルを全て動作チェックする
	WIN32_FIND_DATA win32fd;
	HANDLE hFind = FindFirstFile(_T("Resources\\*"), &win32fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		int number = 0;
		do {
			if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY || _tcscmp(win32fd.cFileName, _T("kaifuku.wav")) == 0) {
			} else {
				// ファイルオープン
				TCHAR filePath[256];
				_tcscpy_s(filePath, _T("Resources\\"));
				_tcscat_s(filePath, win32fd.cFileName);
				if (soundsManager.AddFile(filePath, win32fd.cFileName)) {
					keys.push_back(win32fd.cFileName);
				} else {
					isSuccess = false;
				}
			}
		} while (FindNextFile(hFind, &win32fd));

		FindClose(hFind);
	}

	// 同時再生テスト用音源
	soundsManager.AddFile(_T("Resources\\kaifuku.wav"), _T("SE"));

	if (keys.size() == 0) {
		OutputDebugStringEx(_T("Resourcesフォルダ内にテスト用音源を格納して下さい。\n"));
		return -1;
	}
	isTakingTest = true;
	TestPlaying(keys[currentKeysPos]);
	while (isTakingTest) {
		if (hasFinishedOneFIle) {
			hasFinishedOneFIle = false;
			TestPlaying(keys[currentKeysPos]);
		}
		Sleep(1000);
	}

	return isSuccess ? 0 : -1;
}

static void OnPlayedToEnd(const TCHAR* pKey) {
	OutputDebugStringEx(_T("%s has stopped\n"), pKey);
	if (++currentKeysPos < static_cast<int>(keys.size())) {
		hasFinishedOneFIle = true;
	} else {
		isTakingTest = false;
	}
}

static void TestPlaying(std::basic_string<TCHAR> key) {
	const TCHAR* charKey = const_cast<const TCHAR*>(key.c_str());
	OutputDebugStringEx(_T("++++++++++++ start to test %s ++++++++++++++++++++\n"), charKey);
	PrintStatus(key);

	// 再生開始
	isSuccess = soundsManager.Start(charKey, true) && isSuccess;
	PrintStatus(key);
	Sleep(1000);

	// ボリューム変更
	for (int i = 100; i > 0; i -= 2) {
		isSuccess = soundsManager.SetVolume(charKey, i) && isSuccess;
		OutputDebugStringEx(_T("Volume of %s is %d\n"), charKey, soundsManager.GetVolume(charKey));
		Sleep(100);
	}
	Sleep(1000);

	for (int i = 2; i <= 100; i += 2) {
		isSuccess = soundsManager.SetVolume(charKey, i) && isSuccess;
		OutputDebugStringEx(_T("Volume of %s is %d\n"), charKey, soundsManager.GetVolume(charKey));
		Sleep(100);
	}
	Sleep(1000);

	// 周波数・速度変更
	isSuccess = soundsManager.SetFrequencyRatio(charKey, 0.5f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, soundsManager.GetFrequencyRatio(charKey));
	Sleep(4000);

	isSuccess = soundsManager.SetFrequencyRatio(charKey, 2.0f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, soundsManager.GetFrequencyRatio(charKey));
	Sleep(4000);

	isSuccess = soundsManager.SetFrequencyRatio(charKey, 1.0f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, soundsManager.GetFrequencyRatio(charKey));
	Sleep(2000);

	// 同時再生
	for (int i = 0; i < 2; ++i) {
		isSuccess = soundsManager.Start(_T("SE")) && isSuccess;
		Sleep(4000);
	}
	Sleep(1000);

	// 一時停止
	OutputDebugStringEx(_T("一時停止\n"));
	isSuccess = soundsManager.Pause(charKey) && isSuccess;
	PrintStatus(key);
	Sleep(2000);

	// 再開
	OutputDebugStringEx(_T("再開\n"));
	isSuccess = soundsManager.Resume(charKey) && isSuccess;
	PrintStatus(key);
	Sleep(60000);

	// 停止
	OutputDebugStringEx(_T("停止\n"));
	isSuccess = soundsManager.Stop(charKey) && isSuccess;
	PrintStatus(key);
	Sleep(2000);

	// 最初から1曲分再生し次の曲へ
	OutputDebugStringEx(_T("最初から1曲分再生し次の曲へ\n"));
	isSuccess = soundsManager.Start(charKey, OnPlayedToEnd) && isSuccess;
}

static void PrintStatus(std::basic_string<TCHAR> key) {
	OutputDebugStringEx(_T("Status of %s is %d\n"), const_cast<const TCHAR*>(key.c_str()), soundsManager.GetStatus(key.c_str()));
}
