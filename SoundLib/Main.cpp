#include <stdint.h>
#include <windows.h>
#include "Common.h"
#include "SoundsManager.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	const char* filePath1 = "Resources\\toujyo.wav";
	const char* filePath2 = "Resources\\musicbox.mp3";
	const char* filePath3 = "Resources\\jump03.mp3";

	SoundsManager soundsManager;
	soundsManager.Initialize();
	
	// Waveファイルオープン
	if (!soundsManager.AddFile(filePath1, "wav")) {
		return -1;
	}

	// mp3ファイルオープン
	if (!soundsManager.AddFile(filePath2, "mp3")) {
		return -1;
	}

	// mp3ファイルオープン
	if (!soundsManager.AddFile(filePath3, "mp3SE")) {
		return -1;
	}

	soundsManager.Start("wav", true);

	Sleep(2000);

	for (int i = 0; i < 1; ++i) {
		soundsManager.Start("mp3SE");
		Sleep(1200);
	}

	Sleep(1000);
	soundsManager.Pause("wav");
	Sleep(1000);
	soundsManager.Resume("wav");

	Sleep(2000);

	soundsManager.Stop("wav");

	return 0;
}
