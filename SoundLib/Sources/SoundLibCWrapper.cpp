//----------------------------------------------------------
// <filename>SoundsLibCWrapper.cpp</filename>
// <author>Masami Sugao</author>
// <date>2018/08/09</date>
//----------------------------------------------------------
#include "SoundLibCWrapper.h"
#include <stdint.h>
#include "SoundsManager.h"


static SoundLib::SoundsManagerTmpl<wchar_t>* pSoundsManagerW = nullptr;
static SoundLib::SoundsManagerTmpl<char>* pSoundsManagerA = nullptr;

static enum SoundPlayingStatus SoundLibCWrapper_ConvertPlayingStatus(SoundLib::PlayingStatus status);


/* Global Functions --------------------------------------------------------------------------------- */
bool SoundLibCWrapper_InitializeA() {
	pSoundsManagerA = new SoundLib::SoundsManagerTmpl<char>();
	return pSoundsManagerA->Initialize();
}

bool SoundLibCWrapper_InitializeW() {
	pSoundsManagerW = new SoundLib::SoundsManagerTmpl<wchar_t>();
	return pSoundsManagerW->Initialize();
}

void SoundLibCWrapper_Free() {
	delete pSoundsManagerA;
	pSoundsManagerA = nullptr;
	delete pSoundsManagerW;
	pSoundsManagerW = nullptr;
}

enum SoundPlayingStatus SoundLibCWrapper_GetStatusA(const char* pKey) {
	if (pSoundsManagerA == nullptr) {
		return SoundPlayingStatus_Stopped;
	}
	return SoundLibCWrapper_ConvertPlayingStatus(pSoundsManagerA->GetStatus(pKey));
}

enum SoundPlayingStatus SoundLibCWrapper_GetStatusW(const wchar_t* pKey) {
	if (pSoundsManagerW == nullptr) {
		return SoundPlayingStatus_Stopped;
	}
	return SoundLibCWrapper_ConvertPlayingStatus(pSoundsManagerW->GetStatus(pKey));
}

uint8_t SoundLibCWrapper_GetVolumeA(const char* pKey) {
	if (pSoundsManagerA == nullptr) {
		return 0;
	}
	return pSoundsManagerA->GetVolume(pKey);
}

uint8_t SoundLibCWrapper_GetVolumeW(const wchar_t* pKey) {
	if (pSoundsManagerW == nullptr) {
		return 0;
	}
	return pSoundsManagerW->GetVolume(pKey);
}

bool SoundLibCWrapper_SetVolumeA(const char* pKey, uint8_t volume) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->SetVolume(pKey, volume);
}

bool SoundLibCWrapper_SetVolumeW(const wchar_t* pKey, uint8_t volume) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->SetVolume(pKey, volume);
}

float SoundLibCWrapper_GetFrequencyRatioA(const char* pKey) {
	if (pSoundsManagerA == nullptr) {
		return 0;
	}
	return pSoundsManagerA->GetFrequencyRatio(pKey);
}

float SoundLibCWrapper_GetFrequencyRatioW(const wchar_t* pKey) {
	if (pSoundsManagerW == nullptr) {
		return 0;
	}
	return pSoundsManagerW->GetFrequencyRatio(pKey);
}

bool SoundLibCWrapper_SetFrequencyRatioA(const char* pKey, float ratio) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->SetFrequencyRatio(pKey, ratio);
}

bool SoundLibCWrapper_SetFrequencyRatioW(const wchar_t* pKey, float ratio) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->SetFrequencyRatio(pKey, ratio);
}

bool SoundLibCWrapper_AddFileA(const char* pFilePath, const char* pKey) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->AddFile(pFilePath, pKey);
}

bool SoundLibCWrapper_AddFileW(const wchar_t* pFilePath, const wchar_t* pKey) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->AddFile(pFilePath, pKey);
}

bool SoundLibCWrapper_StartA(const char* pKey, bool isLoopPlayback) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->Start(pKey, isLoopPlayback);
}

bool SoundLibCWrapper_StartW(const wchar_t* pKey, bool isLoopPlayback) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->Start(pKey, isLoopPlayback);
}

bool SoundLibCWrapper_StartA(const char* pKey, void(*onPlayedToEndCallback)(const char* pKey)) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->Start(pKey, onPlayedToEndCallback);
}

bool SoundLibCWrapper_StartW(const wchar_t* pKey, void(*onPlayedToEndCallback)(const wchar_t* pKey)) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->Start(pKey, onPlayedToEndCallback);
}

bool SoundLibCWrapper_StopA(const char* pKey) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->Stop(pKey);
}

bool SoundLibCWrapper_StopW(const wchar_t* pKey) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->Stop(pKey);
}

bool SoundLibCWrapper_PauseA(const char* pKey) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->Pause(pKey);
}

bool SoundLibCWrapper_PauseW(const wchar_t* pKey) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->Pause(pKey);
}

bool SoundLibCWrapper_ResumeA(const char* pKey) {
	if (pSoundsManagerA == nullptr) {
		return false;
	}
	return pSoundsManagerA->Resume(pKey);
}

bool SoundLibCWrapper_ResumeW(const wchar_t* pKey) {
	if (pSoundsManagerW == nullptr) {
		return false;
	}
	return pSoundsManagerW->Resume(pKey);
}

/* Static Functions --------------------------------------------------------------------------------- */
static enum SoundPlayingStatus SoundLibCWrapper_ConvertPlayingStatus(SoundLib::PlayingStatus status) {
	switch (status) {
	case SoundLib::PlayingStatus::Playing:
		return SoundPlayingStatus_Playing;
	case SoundLib::PlayingStatus::Pausing:
		return SoundPlayingStatus_Pausing;
	default:
		return SoundPlayingStatus_Stopped;
	}
}
