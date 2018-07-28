#include "SoundsManager.h"

namespace SoundLib {
/* Constructor / Destructor ------------------------------------------------------------------------- */
template <typename T>
SoundsManagerTmpl<T>::SoundsManagerTmpl() : pXAudio2(nullptr) {}

template <typename T>
SoundsManagerTmpl<T>::~SoundsManagerTmpl() {
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
template <typename T>
PlayingStatus SoundsManagerTmpl<T>::GetStatus(const T* pKey) const {
	if (!ExistsKey(pKey)) {
		return PlayingStatus::Stopped;
	}

	return this->audioMap.at(pKey)->GetStatus();
}

template <typename T>
uint8_t SoundsManagerTmpl<T>::GetVolume(const T* pKey) const {
	if (!ExistsKey(pKey)) {
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

template <typename T>
bool SoundsManagerTmpl<T>::SetVolume(const T* pKey, uint8_t volume) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	if (volume > 100) {
		// 音源以上のボリューム設定は不可
		volume = 100;
	}
	float centiVolume = volume / 100.0f;
	return this->audioMap[pKey]->SetVolume(centiVolume);
}

template <typename T>
float SoundsManagerTmpl<T>::GetFrequencyRatio(const T* pKey) const {
	if (!ExistsKey(pKey)) {
		return 0;
	}

	return this->audioMap.at(pKey)->GetFrequencyRatio();
}

template <typename T>
bool SoundsManagerTmpl<T>::SetFrequencyRatio(const T* pKey, float ratio) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	return this->audioMap[pKey]->SetFrequencyRatio(ratio);
}

/* Public Functions  -------------------------------------------------------------------------------- */
template <typename T>
bool SoundsManagerTmpl<T>::Initialize() {
	HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(result)) {
		Common::OutputDebugString("error CoInitializeEx result=%d\n", result);
		return false;
	}

	result = XAudio2Create(&this->pXAudio2);
	if (FAILED(result)) {
		Common::OutputDebugString("error XAudio2Create result=%d\n", result);
		return false;
	}

	IXAudio2MasteringVoice* master = nullptr;
	result = this->pXAudio2->CreateMasteringVoice(&master);
	if (FAILED(result)) {
		Common::OutputDebugString("error CreateMasteringVoice result=%d\n", result);
		return false;
	}

	return true;
}

bool SoundsManagerTmpl<wchar_t>::AddFile(const wchar_t* pFilePath, const wchar_t* pKey) {
	const char* pCharKey = Common::ToChar(pKey);

	if (ExistsKey(pKey, false)) {
		Common::OutputDebugString("既に登録済みのキー%sが指定されました。\n", pCharKey);
		return false;
	}
	
	const char* pCharFilePath = Common::ToChar(pFilePath);
	Audio::IAudio* pAudio = nullptr;
	bool ret = JudgeAudio(pCharFilePath, &pAudio);
	if (ret) {
		OutputAudioInfo(pAudio, pCharKey);
		ret = AddToMap(pAudio, pKey);
	} 

	delete[] pCharKey;
	delete[] pCharFilePath;
	return ret;
}

bool SoundsManagerTmpl<char>::AddFile(const char* pFilePath, const char* pKey) {
	if (ExistsKey(pKey, false)) {
		Common::OutputDebugString("既に登録済みのキー%sが指定されました。\n", pKey);
		return false;
	}

	Audio::IAudio* pAudio = nullptr;
	bool ret = JudgeAudio(pFilePath, &pAudio);
	if (ret) {
		OutputAudioInfo(pAudio, pKey);
		ret = AddToMap(pAudio, pKey);
	}

	return ret;
}

template <typename T>
bool SoundsManagerTmpl<T>::Start(const T* pKey, bool isLoopPlayback) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	this->audioMap[pKey]->Start(isLoopPlayback);
	return true;
}

template <typename T>
bool SoundsManagerTmpl<T>::Start(const T* pKey, ISoundsManagerDelegate<T>* pDelegate) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	this->audioMap[pKey]->Start(pDelegate);
	return true;
}

template <typename T>
bool SoundsManagerTmpl<T>::Start(const T* pKey, void(*onPlayedToEndCallback)(const T* pKey)) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	this->audioMap[pKey]->Start(onPlayedToEndCallback);
	return true;
}

template <typename T>
bool SoundsManagerTmpl<T>::Stop(const T* pKey) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	this->audioMap[pKey]->Stop();
	return true;
}

template <typename T>
bool SoundsManagerTmpl<T>::Pause(const T* pKey) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	this->audioMap[pKey]->Pause();
	return true;
}

template <typename T>
bool SoundsManagerTmpl<T>::Resume(const T* pKey) {
	if (!ExistsKey(pKey)) {
		return false;
	}

	this->audioMap[pKey]->Resume();
	return true;
}

/* Private Functions  ------------------------------------------------------------------------------- */
template <typename T>
bool SoundsManagerTmpl<T>::ExistsKey(const T* pKey, bool isErrWhenNotExists) const {
	auto itr = this->audioMap.find(pKey);
	bool ret = (itr != this->audioMap.end());
	if (!ret && isErrWhenNotExists) {
		OutputStrWithKey("キー%sは存在しません。\n", pKey);
	}
	return ret;
}

template <typename T>
bool SoundsManagerTmpl<T>::JudgeAudio(const char* pFilePath, Audio::IAudio** ppAudio) const {
	const char* pExtension = strrchr(pFilePath, '.');
	if (pExtension != nullptr && strcmp(pExtension, ".wav") == 0) {
		*ppAudio = new Audio::WaveAudio();
	} else if (pExtension != nullptr && strcmp(pExtension, ".ogg") == 0) {
		*ppAudio = new Audio::CompressedAudio();
	} else if (pExtension != nullptr && strcmp(pExtension, ".mp3") == 0) {
		*ppAudio = new Audio::Mp3Audio();
	} else {
		*ppAudio = new Audio::CompressedAudio();
	}

	int tryCount = 0;
	bool couldLoad = false;
	while (!(couldLoad = (*ppAudio)->Load(pFilePath)) && ++tryCount < 3) {
		// 他の形式で読めるか試してみる
		if (typeid(**ppAudio) == typeid(Audio::WaveAudio)) {
			delete *ppAudio;
			*ppAudio = new Audio::Mp3Audio();
		} else if (typeid(**ppAudio) == typeid(Audio::Mp3Audio)) {
			delete *ppAudio;
			*ppAudio = new Audio::CompressedAudio();
		} else {
			delete *ppAudio;
			*ppAudio = new Audio::WaveAudio();
		}
	}

	if (!couldLoad) {
		Common::OutputDebugString("%s は再生可能な形式ではありません。\n", pFilePath);
		delete *ppAudio;
		*ppAudio = nullptr;
	}

	return couldLoad;
}

template <typename T>
void SoundsManagerTmpl<T>::OutputAudioInfo(Audio::IAudio* pAudio, const char* pKey) const {
	const WAVEFORMATEX* pFormat = pAudio->GetWaveFormatEx();
	Common::OutputDebugString("-----------------キー%sのオーディオ情報--------------------\n", pKey);
	Common::OutputDebugString("file foramt=%s\n", pAudio->GetFormatName().c_str());
	Common::OutputDebugString("**** デコード前 ****\n");
	Common::OutputDebugString("channel    =%d\n", pAudio->GetChannelCount());
	Common::OutputDebugString("sampling   =%dHz\n", pAudio->GetSamplingRate());
	Common::OutputDebugString("bit/sample =%d\n", pAudio->GetBitsPerSample());
	Common::OutputDebugString("**** デコード後 ****\n");
	Common::OutputDebugString("foramt     =%d\n", pFormat->wFormatTag);
	Common::OutputDebugString("channel    =%d\n", pFormat->nChannels);
	Common::OutputDebugString("sampling   =%dHz\n", pFormat->nSamplesPerSec);
	Common::OutputDebugString("bit/sample =%d\n", pFormat->wBitsPerSample);
	Common::OutputDebugString("byte/sec   =%d\n", pFormat->nAvgBytesPerSec);
	Common::OutputDebugString("-----------------------------------------------------------\n");
}

template <typename T>
bool SoundsManagerTmpl<T>::AddToMap(Audio::IAudio* pAudio, const T* pKey) {
	std::basic_string<T> key(pKey);
	this->audioMap[key] = new AudioHandler<T>(key, pAudio);
	return this->audioMap[key]->Prepare(*this->pXAudio2);
}

void SoundsManagerTmpl<char>::OutputStrWithKey(const char* pStr, const char* pKey) const {
	Common::OutputDebugString(pStr, pKey);
}

void SoundsManagerTmpl<wchar_t>::OutputStrWithKey(const char* pStr, const wchar_t* pKey) const {
	const char* pCharKey = Common::ToChar(pKey);
	Common::OutputDebugString(pStr, pCharKey);
	delete[] pCharKey;
}


template class SoundsManagerTmpl<char>;
template class SoundsManagerTmpl<wchar_t>;

}