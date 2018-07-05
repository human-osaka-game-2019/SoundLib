#ifndef SOUNDS_MANAGER_H
#define SOUNDS_MANAGER_H

#include <unordered_map>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include "AudioHandler.h"
#include "ISoundsManagerDelegate.h"


class SoundsManager {
public:
	SoundsManager();
	~SoundsManager();
	bool Initialize();
	bool AddFile(const char* pFilePath, const char* pKey);
	void Start(const char* pKey, bool isLoopPlayback = false);
	void Start(const char* pKey, ISoundsManagerDelegate* pDelegate);
	void Start(const char* pKey, void(*onPlayedToEndCallback)(const char* pKey));
	void Stop(const char* pKey);
	void Pause(const char* pKey);
	void Resume(const char* pKey);

private:
	IAudio* pAudio;
	std::unordered_map<const char*, AudioHandler*> audioMap;
	IXAudio2* pXAudio2;
};
#endif
