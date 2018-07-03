#ifndef SOUNDS_MANAGER_H
#define SOUNDS_MANAGER_H

#include <unordered_map>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include "AudioHandler.h"


class SoundsManager {
public:
	SoundsManager();
	~SoundsManager();
	bool Initialize();
	bool AddFile(const char* pFilePath, const char* key);
	void Start(const char* key, bool isLoopPlayback = false);
	void Stop(const char* key);
	void Pause(const char* key);
	void Resume(const char* key);

private:
	IAudio* pAudio;
	std::unordered_map<const char*, AudioHandler*> audioMap;
	IXAudio2* pXAudio2;
};
#endif
