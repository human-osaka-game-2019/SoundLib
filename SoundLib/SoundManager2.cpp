#include "SoundManager2.h"
#include <stdio.h>
#include <tchar.h>


#ifdef _DEBUG
#define OutputDebugStringEx( str, ... ) \
      { \
        TCHAR c[256]; \
        _stprintf_s( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#    define OutputDebugString( str, ... ) // ‹óŽÀ‘•
#endif


SoundManager::SoundManager() : audio(nullptr), pVoice(nullptr), buf(nullptr) {
	this->pCallback = new VoiceCallback(this);
}

SoundManager::~SoundManager() {
	delete this->pCallback;

	
		this->Stop();
		

	if (this->audio != nullptr) {
		this->audio->Release();
	}
	CoUninitialize();
}

bool SoundManager::Initialize() {
	HRESULT ret = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(ret)) {
		OutputDebugStringEx("error CoInitializeEx ret=%d\n", ret);
		return false;
	}

	ret = XAudio2Create(
		&this->audio
		// UINT32 Flags = 0,
		// XAUDIO2_PROCESSOR XAudio2Processor = XAUDIO2_DEFAULT_PROCESSOR
	);
	if (FAILED(ret)) {
		OutputDebugStringEx("error XAudio2Create ret=%d\n", ret);
		return false;
	}

	IXAudio2MasteringVoice* master = NULL;
	ret = this->audio->CreateMasteringVoice(
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

bool SoundManager::OpenSoundFile(char* filePath) {
	if (!this->reader.OpenFile(filePath, &this->waveFormatEx)) {
		return false;
	}

	OutputDebugStringEx("foramt    =%d\n", this->waveFormatEx.wFormatTag);
	OutputDebugStringEx("channel   =%d\n", this->waveFormatEx.nChannels);
	OutputDebugStringEx("sampling  =%dHz\n", this->waveFormatEx.nSamplesPerSec);
	OutputDebugStringEx("bit/sample=%d\n", this->waveFormatEx.wBitsPerSample);
	OutputDebugStringEx("byte/sec  =%d\n", this->waveFormatEx.nAvgBytesPerSec);

	return true;
}

bool SoundManager::Start(bool isLoopPlayback = false) {
	this->isLoopPlayback = isLoopPlayback;
	HRESULT ret = this->audio->CreateSourceVoice(
		&this->pVoice,
		&waveFormatEx,
		0,                          // UINT32 Flags = 0,
		XAUDIO2_DEFAULT_FREQ_RATIO, // float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,
		this->pCallback                   // IXAudio2VoiceCallback *pCallback = NULL,
									// const XAUDIO2_VOICE_SENDS *pSendList = NULL,
									// const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL
	);
	if (FAILED(ret)) {
		OutputDebugStringEx("error CreateSourceVoice ret=%d\n", ret);
		return false;
	}
	this->pVoice->Start();

	this->buffer = { 0 };
	this->buf = new BYTE*[BUF_LEN];
	this->len = this->waveFormatEx.nAvgBytesPerSec;
	for (int i = 0; i < BUF_LEN; i++) {
		this->buf[i] = new BYTE[this->len];
	}

	this->buf_cnt = 0;

	this->Push();

	return true;
}

void SoundManager::Stop() {
	if (this->pVoice != nullptr) {
		this->pVoice->Stop();
		this->pVoice->DestroyVoice();
		this->pVoice = nullptr;
	}

	if (this->buf != nullptr) {
		for (int i = 0; i < BUF_LEN; i++) {
			delete[] this->buf[i];
		}
		delete this->buf;
		this->buf = nullptr;
	}
}

void SoundManager::Pause() {
	this->pVoice->Stop();
}

void SoundManager::Resume() {
	this->pVoice->Start();
}

void SoundManager::BufferEndCallback() {
	this->Push();
}

void SoundManager::Push() {
	// ‰¹ƒf[ƒ^Ši”[
	long size = this->reader.Read(this->buf[this->buf_cnt], this->len);
	if (size <= 0) {
		this->Stop();
		return;
	}
	this->buffer.AudioBytes = size;
	this->buffer.pAudioData = this->buf[this->buf_cnt];
	HRESULT ret = this->pVoice->SubmitSourceBuffer(&this->buffer);
	if (FAILED(ret)) {
		OutputDebugStringEx("error SubmitSourceBuffer ret=%d\n", ret);
		return;
	}
	if (BUF_LEN <= ++this->buf_cnt) {
		this->buf_cnt = 0;
	}
}


