//----------------------------------------------------------
// <filename>SoundsManager.cpp</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#include "SoundsManager.h"
#include <typeinfo>
#include "Audio/WaveAudio.h"
#include "Audio/Mp3Audio.h"
#include "Audio/OggAudio.h"
#include "Audio/CompressedAudio.h"


namespace SoundLib {
/* Constructor / Destructor ------------------------------------------------------------------------- */
SoundsManager::SoundsManager() : pXAudio2(nullptr) {}

SoundsManager::~SoundsManager() {
	for (auto& rAudioPair : this->audioMap) {
		delete rAudioPair.second;
		rAudioPair.second = nullptr;
	}
	this->audioMap.clear();

	if (this->pXAudio2 != nullptr) {
		this->pXAudio2->Release();
		this->pXAudio2 = nullptr;
	}
	CoUninitialize();
}


/* Getters / Setters -------------------------------------------------------------------------------- */
PlayingStatus SoundsManager::GetStatus(const TCHAR* pKey) const {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		throw std::invalid_argument(_T("キーが存在しません。"));
	}

	return this->audioMap.at(pKey)->GetStatus();
}

uint8_t SoundsManager::GetVolume(const TCHAR* pKey) const {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return 0;
	}

	float volume = this->audioMap.at(pKey)->GetVolume();
	if (volume < 0) {
		// 位相反転は設定不可のため考慮しない
		return 0;
	} else {
		if (volume > 1) {
			// 音源以上の音量設定は設定不可のため考慮しない
			volume = 1;
		}
		return (uint8_t)(volume * 100);
	}
}

bool SoundsManager::SetVolume(const TCHAR* pKey, uint8_t volume) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return false;
	}

	if (volume > 100) {
		// 音源以上のボリューム設定は不可
		volume = 100;
	}
	float centiVolume = volume / 100.0f;
	return this->audioMap[pKey]->SetVolume(centiVolume);
}

float SoundsManager::GetFrequencyRatio(const TCHAR* pKey) const {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return 0;
	}

	return this->audioMap.at(pKey)->GetFrequencyRatio();
}

bool SoundsManager::SetFrequencyRatio(const TCHAR* pKey, float ratio) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return 0;
	}

	return this->audioMap[pKey]->SetFrequencyRatio(ratio);
}

/* Public Functions  -------------------------------------------------------------------------------- */
bool SoundsManager::Initialize() {
	HRESULT ret = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(ret)) {
		OutputDebugStringEx(_T("error CoInitializeEx ret=%d\n"), ret);
		return false;
	}

	ret = XAudio2Create(&this->pXAudio2);
	if (FAILED(ret)) {
		OutputDebugStringEx(_T("error XAudio2Create ret=%d\n"), ret);
		return false;
	}

	IXAudio2MasteringVoice* master = nullptr;
	ret = this->pXAudio2->CreateMasteringVoice(&master);
	if (FAILED(ret)) {
		OutputDebugStringEx(_T("error CreateMasteringVoice ret=%d\n"), ret);
		return false;
	}

	return true;
}

bool SoundsManager::AddFile(const TCHAR* pFilePath, const TCHAR* pKey) {
	if (ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは既に登録済み。\n"), pKey);
		return false;
	}
	
	const TCHAR* pExtension = _tcsstr(pFilePath, _T("."));

	Audio::IAudio* pAudio;
	if (pExtension != nullptr && strcmp(pExtension, _T(".wav")) == 0) {
		pAudio = new Audio::WaveAudio();
	} else if (pExtension != nullptr && strcmp(pExtension, _T(".ogg")) == 0) {
		pAudio = new Audio::CompressedAudio();
	} else if (pExtension != nullptr && strcmp(pExtension, _T(".mp3")) == 0) {
		pAudio = new Audio::Mp3Audio();
	} else {
		pAudio = new Audio::CompressedAudio();
	} 

	int tryCount = 0;
	bool couldLoad = false;
	while (!(couldLoad = pAudio->Load(pFilePath)) && ++tryCount < 3) {
		// 他の形式で読めるか試してみる
		if (typeid(*pAudio) == typeid(Audio::WaveAudio)) {
			delete pAudio;
			pAudio = new Audio::Mp3Audio();
		} else if (typeid(*pAudio) == typeid(Audio::OggAudio)) {
			delete pAudio;
			pAudio = new Audio::WaveAudio();
		} else if (typeid(*pAudio) == typeid(Audio::Mp3Audio)) {
			delete pAudio;
			pAudio = new Audio::CompressedAudio();
		} else {
			delete pAudio;
			pAudio = new Audio::OggAudio();
		}
	}

	if (!couldLoad) {
		OutputDebugStringEx(_T("%sは再生可能な形式ではありません。\n"), pFilePath);
		delete pAudio;
		return false;
	}

	const WAVEFORMATEX* pFormat = pAudio->GetWaveFormatEx();
	OutputDebugStringEx(_T("-----------------キー%sのオーディオ情報--------------------\n"), pKey);
	OutputDebugStringEx(_T("file foramt=%s\n"), pAudio->GetFormatName().c_str());
	OutputDebugStringEx(_T("**** デコード前 ****\n"));
	OutputDebugStringEx(_T("channel    =%d\n"), pAudio->GetChannelCount());
	OutputDebugStringEx(_T("sampling   =%dHz\n"), pAudio->GetSamplingRate());
	OutputDebugStringEx(_T("bit/sample =%d\n"), pAudio->GetBitsPerSample());
	OutputDebugStringEx(_T("**** デコード後 ****\n"));
	OutputDebugStringEx(_T("foramt     =%d\n"), pFormat->wFormatTag);
	OutputDebugStringEx(_T("channel    =%d\n"), pFormat->nChannels);
	OutputDebugStringEx(_T("sampling   =%dHz\n"), pFormat->nSamplesPerSec);
	OutputDebugStringEx(_T("bit/sample =%d\n"), pFormat->wBitsPerSample);
	OutputDebugStringEx(_T("byte/sec   =%d\n"), pFormat->nAvgBytesPerSec);
	OutputDebugStringEx(_T("-----------------------------------------------------------\n"));

	this->audioMap[pKey] = new AudioHandler(pKey, pAudio);
	return this->audioMap[pKey]->Prepare(*this->pXAudio2);
}

bool SoundsManager::Start(const TCHAR* pKey, bool isLoopPlayback) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return false;
	}

	this->audioMap[pKey]->Start(isLoopPlayback);
	return true;
}

bool SoundsManager::Start(const TCHAR* pKey, ISoundsManagerDelegate* pDelegate) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return false;
	}
	
	this->audioMap[pKey]->Start(pDelegate);
	return true;
}

bool SoundsManager::Start(const TCHAR* pKey, void(*onPlayedToEndCallback)(const TCHAR* pKey)) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return false;
	}
	
	this->audioMap[pKey]->Start(onPlayedToEndCallback);
	return true;
}

bool SoundsManager::Stop(const TCHAR* pKey) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return false;
	}
	
	this->audioMap[pKey]->Stop();
	return true;
}

bool SoundsManager::Pause(const TCHAR* pKey) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return false;
	}
	
	this->audioMap[pKey]->Pause();
	return true;
}

bool SoundsManager::Resume(const TCHAR* pKey) {
	if (!ExistsKey(pKey)) {
		OutputDebugStringEx(_T("キー%sは存在しません。\n"), pKey);
		return false;
	}
	
	this->audioMap[pKey]->Resume();
	return true;
}


/* Private Functions  ------------------------------------------------------------------------------- */
bool SoundsManager::ExistsKey(TString key) const {
	auto itr = this->audioMap.find(key);
	return (itr != this->audioMap.end());
}
}
