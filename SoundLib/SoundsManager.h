#ifndef SOUNDS_MANAGER_H
#define SOUNDS_MANAGER_H

#include <unordered_map>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include <stdint.h>
#include "Common.h"
#include "AudioHandler.h"
#include "ISoundsManagerDelegate.h"


namespace SoundLib {

class SoundsManager {
public:
	SoundsManager();
	SoundsManager(SoundsManager&&) = default;
	~SoundsManager();

	PlayingStatus GetStatus(const TCHAR* pKey) const;
	uint8_t GetVolume(const TCHAR* pKey) const;
	bool SetVolume(const TCHAR* pKey, uint8_t volume);

	SoundsManager& operator=(SoundsManager&&) = default;

	bool Initialize();
	bool AddFile(const TCHAR* pFilePath, const TCHAR* pKey);
	bool Start(const TCHAR* pKey, bool isLoopPlayback = false);
	bool Start(const TCHAR* pKey, ISoundsManagerDelegate* pDelegate);
	bool Start(const TCHAR* pKey, void(*onPlayedToEndCallback)(const TCHAR* pKey));
	bool Stop(const TCHAR* pKey);
	bool Pause(const TCHAR* pKey);
	bool Resume(const TCHAR* pKey);

private:
	std::unordered_map<TString, AudioHandler*> audioMap;
	IXAudio2* pXAudio2;

	SoundsManager(const SoundsManager&) = delete;
	SoundsManager& operator=(const SoundsManager&) = delete;

	bool ExistsKey(TString key) const;
};

}
#endif
