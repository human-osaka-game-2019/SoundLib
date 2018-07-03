#include "SoundsManager.h"
#include "Common.h"
#include "WaveAudio.h"
#include "Mp3Audio.h"


SoundsManager::SoundsManager() : pXAudio2(nullptr) {}

SoundsManager::~SoundsManager() {
	/*
	for (auto& rAudioPair : this->audioMap) {
		delete this->audioMap[rAudioPair.first];
	}
	this->audioMap.clear();
	*/

	if (this->pXAudio2 != nullptr) {
		this->pXAudio2->Release();
		this->pXAudio2 = nullptr;
	}
	CoUninitialize();
}

bool SoundsManager::Initialize() {
	HRESULT ret = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(ret)) {
		OutputDebugStringEx("error CoInitializeEx ret=%d\n", ret);
		return false;
	}

	ret = XAudio2Create(
		&this->pXAudio2
		// UINT32 Flags = 0,
		// XAUDIO2_PROCESSOR XAudio2Processor = XAUDIO2_DEFAULT_PROCESSOR
	);
	if (FAILED(ret)) {
		OutputDebugStringEx("error XAudio2Create ret=%d\n", ret);
		return false;
	}

	IXAudio2MasteringVoice* master = NULL;
	ret = this->pXAudio2->CreateMasteringVoice(
		&master
		// UINT32 InputChannels = XAUDIO2_DEFAULT_CHANNELS,
		// UINT32 InputSampleRate = XAUDIO2_DEFAULT_SAMPLERATE,
		// UINT32 Flags = 0,
		// UINT32 DeviceIndex = 0,
		// const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL
	);
	if (FAILED(ret)) {
		OutputDebugStringEx("error CreateMasteringVoice ret=%d\n", ret);
		return false;
	}

	return true;
}

bool SoundsManager::AddFile(const char* pFilePath, const char* key) {
	char* extension = strstr((char*)pFilePath, ".");

	IAudio* pAudio;
	if (strcmp(extension, ".wav") == 0) {
		pAudio = new WaveAudio();
	} else if (strcmp(extension, ".mp3") == 0) {
		pAudio = new Mp3Audio;
	} else {
		OutputDebugStringEx("Šg’£Žq%s‚Í‘ÎÛŠO\n", extension);
		return false;
	}

	if (!pAudio->Load(pFilePath)) {
		return false;
	}

	const WAVEFORMATEX* pFormat = pAudio->GetWaveFormatEx();
	OutputDebugStringEx("foramt    =%d\n", pFormat->wFormatTag);
	OutputDebugStringEx("channel   =%d\n", pFormat->nChannels);
	OutputDebugStringEx("sampling  =%dHz\n", pFormat->nSamplesPerSec);
	OutputDebugStringEx("bit/sample=%d\n", pFormat->wBitsPerSample);
	OutputDebugStringEx("byte/sec  =%d\n", pFormat->nAvgBytesPerSec);

	this->audioMap[key] = new AudioHandler(pAudio);
	return this->audioMap[key]->Prepare(this->pXAudio2);
}

void SoundsManager::Start(const char* key, bool isLoopPlayback) {
	this->audioMap[key]->Start(isLoopPlayback);
}

void SoundsManager::Stop(const char* key) {
	this->audioMap[key]->Stop();
}

void SoundsManager::Pause(const char* key) {
	this->audioMap[key]->Pause();
}

void SoundsManager::Resume(const char* key) {
	this->audioMap[key]->Resume();
}

