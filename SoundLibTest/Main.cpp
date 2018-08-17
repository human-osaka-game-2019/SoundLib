#include <windows.h>
#include <crtdbg.h>

#define USE_C_WRAPPER false
#if (USE_C_WRAPPER == true)
#include "SoundLibCWrapper.h"
#else
#include "SoundsManager.h"
#endif

#define OutputDebugStringEx(str, ...) \
      { \
        TCHAR c[256]; \
        _stprintf_s(c, str, __VA_ARGS__); \
        OutputDebugString(c); \
      }

#if (USE_C_WRAPPER == false)
static SoundLib::SoundsManager soundsManager;
#endif

static std::vector<std::basic_string<TCHAR>> keys;
static int currentKeysPos = 0;

static void OnPlayedToEnd(const TCHAR* pKey);
static void TestPlaying();
static void PrintStatus(std::basic_string<TCHAR> key);
static bool hasFinishedOneFIle = false;;
static bool isTakingTest;
static bool isSuccess;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#if _DEBUG
	// メモリリークチェック
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_Initialize();
#else
	isSuccess = soundsManager.Initialize();
#endif

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
				memset(filePath, 0x00, sizeof(filePath) * sizeof(TCHAR));
				_tcscpy_s(filePath, _T("Resources\\"));
				_tcscat_s(filePath, win32fd.cFileName);

				TCHAR key2[256];
				memset(key2, 0x00, sizeof(key2) * sizeof(TCHAR));
				_tcscat_s(key2, win32fd.cFileName);
				_tcscat_s(key2, _T("2"));

#if (USE_C_WRAPPER == true)
				bool couldLoad = SoundLibCWrapper_AddFile(filePath, win32fd.cFileName);
				couldLoad = SoundLibCWrapper_AddFile(filePath, key2) && couldLoad;
#else
				bool couldLoad = soundsManager.AddFile(filePath, win32fd.cFileName);
				couldLoad = soundsManager.AddFile(filePath, key2) && couldLoad;
#endif
				if (couldLoad) {
					keys.push_back(win32fd.cFileName);
					keys.push_back(key2);
				} else {
					isSuccess = false;
				}
			}
		} while (FindNextFile(hFind, &win32fd));

		FindClose(hFind);
	}

	// 同時再生テスト用音源
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_AddFile(_T("Resources\\kaifuku.wav"), _T("SE")) && isSuccess;
#else
	isSuccess = soundsManager.AddFile(_T("Resources\\kaifuku.wav"), _T("SE")) && isSuccess;
#endif

	if (keys.size() == 0) {
		OutputDebugStringEx(_T("Resourcesフォルダ内にテスト用音源を格納して下さい。\n"));
		return -1;
	}
	isTakingTest = true;
	TestPlaying();
	while (isTakingTest) {
		if (hasFinishedOneFIle) {
			hasFinishedOneFIle = false;
			TestPlaying();
		}
		Sleep(1000);
	}

#if (USE_C_WRAPPER)
	SoundLibCWrapper_Free();
#endif

	return isSuccess ? 0 : -1;
}

static void OnPlayedToEnd(const TCHAR* pKey) {
	OutputDebugStringEx(_T("%s has stopped\n"), pKey);
	currentKeysPos += 2;
	if (currentKeysPos < static_cast<int>(keys.size())) {
		hasFinishedOneFIle = true;
	} else {
		isTakingTest = false;
	}
}

static void TestPlaying() {
	const TCHAR* charKey = const_cast<const TCHAR*>(keys[currentKeysPos].c_str());
	const TCHAR* secondCharKey = const_cast<const TCHAR*>(keys[currentKeysPos + 1].c_str());
	OutputDebugStringEx(_T("++++++++++++ start to test %s ++++++++++++++++++++\n"), charKey);
	PrintStatus(keys[currentKeysPos]);

	// 再生開始
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_Start(charKey, true) && isSuccess;
#else
	isSuccess = soundsManager.Start(charKey, true) && isSuccess;
#endif
	PrintStatus(keys[currentKeysPos]);
	Sleep(1000);

	// ボリューム変更
	for (int i = 100; i > 0; i -= 2) {
#if (USE_C_WRAPPER == true)
		isSuccess = SoundLibCWrapper_SetVolume(charKey, i) && isSuccess;
		OutputDebugStringEx(_T("Volume of %s is %d\n"), charKey, SoundLibCWrapper_GetVolume(charKey));
#else
		isSuccess = soundsManager.SetVolume(charKey, i) && isSuccess;
		OutputDebugStringEx(_T("Volume of %s is %d\n"), charKey, soundsManager.GetVolume(charKey));
#endif
		Sleep(100);
	}
	Sleep(1000);

	for (int i = 2; i <= 100; i += 2) {
#if (USE_C_WRAPPER == true)
		isSuccess = SoundLibCWrapper_SetVolume(charKey, i) && isSuccess;
		OutputDebugStringEx(_T("Volume of %s is %d\n"), charKey, SoundLibCWrapper_GetVolume(charKey));
#else
		isSuccess = soundsManager.SetVolume(charKey, i) && isSuccess;
		OutputDebugStringEx(_T("Volume of %s is %d\n"), charKey, soundsManager.GetVolume(charKey));
#endif
		Sleep(100);
	}
	Sleep(1000);

	// 周波数・速度変更
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_SetFrequencyRatio(charKey, 0.5f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, SoundLibCWrapper_GetFrequencyRatio(charKey));
#else
	isSuccess = soundsManager.SetFrequencyRatio(charKey, 0.5f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, soundsManager.GetFrequencyRatio(charKey));
#endif
	Sleep(4000);

#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_SetFrequencyRatio(charKey, 2.0f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, SoundLibCWrapper_GetFrequencyRatio(charKey));
#else
	isSuccess = soundsManager.SetFrequencyRatio(charKey, 2.0f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, soundsManager.GetFrequencyRatio(charKey));
#endif
	Sleep(4000);

#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_SetFrequencyRatio(charKey, 1.0f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, SoundLibCWrapper_GetFrequencyRatio(charKey));
#else
	isSuccess = soundsManager.SetFrequencyRatio(charKey, 1.0f) && isSuccess;
	OutputDebugStringEx(_T("FrequencyRatio of %s is %f\n"), charKey, soundsManager.GetFrequencyRatio(charKey));
#endif
	Sleep(2000);

	// 同時再生
	for (int i = 0; i < 2; ++i) {
#if (USE_C_WRAPPER == true)
		isSuccess = SoundLibCWrapper_Start(_T("SE")) && isSuccess;
#else
		isSuccess = soundsManager.Start(_T("SE")) && isSuccess;
#endif
		Sleep(4000);
	}
	Sleep(1000);

	// 多重再生
	OutputDebugStringEx(_T("多重再生\n"));
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_Start(secondCharKey, true) && isSuccess;
#else
	isSuccess = soundsManager.Start(secondCharKey, true) && isSuccess;
#endif
	PrintStatus(keys[currentKeysPos + 1]);
	Sleep(7000);
	isSuccess = soundsManager.Stop(secondCharKey) && isSuccess;
	Sleep(2000);

	// 一時停止
	OutputDebugStringEx(_T("一時停止\n"));
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_Pause(charKey) && isSuccess;
#else
	isSuccess = soundsManager.Pause(charKey) && isSuccess;
#endif
	PrintStatus(keys[currentKeysPos]);
	Sleep(2000);

	// 再開
	OutputDebugStringEx(_T("再開\n"));
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_Resume(charKey) && isSuccess;
#else
	isSuccess = soundsManager.Resume(charKey) && isSuccess;
#endif
	PrintStatus(keys[currentKeysPos]);
	Sleep(60000);

	// 停止
	OutputDebugStringEx(_T("停止\n"));
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_Stop(charKey) && isSuccess;
#else
	isSuccess = soundsManager.Stop(charKey) && isSuccess;
#endif
	PrintStatus(keys[currentKeysPos]);
	Sleep(2000);

	// 最初から1曲分再生し次の曲へ
	OutputDebugStringEx(_T("最初から1曲分再生し次の曲へ\n"));
#if (USE_C_WRAPPER == true)
	isSuccess = SoundLibCWrapper_Start(charKey, OnPlayedToEnd) && isSuccess;
#else
	isSuccess = soundsManager.Start(charKey, OnPlayedToEnd) && isSuccess;
#endif
}

static void PrintStatus(std::basic_string<TCHAR> key) {
#if (USE_C_WRAPPER == true)
	OutputDebugStringEx(_T("Status of %s is %d\n"), const_cast<const TCHAR*>(key.c_str()), SoundLibCWrapper_GetStatus(key.c_str()));
#else
	OutputDebugStringEx(_T("Status of %s is %d\n"), const_cast<const TCHAR*>(key.c_str()), soundsManager.GetStatus(key.c_str()));
#endif
}
