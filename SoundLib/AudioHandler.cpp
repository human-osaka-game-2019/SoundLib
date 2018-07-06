#include "AudioHandler.h"
#include "Common.h"


namespace SoundLib {
AudioHandler::AudioHandler(const char* pName, IAudio* pAudio) : pName(pName), pAudio(pAudio), pVoice(nullptr), pDelegate(nullptr), onPlayedToEndCallback(nullptr), status(PlayingStatus::Stopped) {
	this->pVoiceCallback = new VoiceCallback(this);
}

AudioHandler::~AudioHandler() {
	this->Stop();

	if (this->pVoice != nullptr) {
		this->pVoice->Stop();
		this->pVoice->DestroyVoice();
		this->pVoice = nullptr;
	}

	if (this->pAudio != nullptr) {
		delete this->pAudio;
		this->pAudio = nullptr;
	}

	delete this->pVoiceCallback;
}

PlayingStatus AudioHandler::GetStatus() {
	return this->status;
}

bool AudioHandler::Prepare(IXAudio2* pXAudio2) {
	HRESULT ret = pXAudio2->CreateSourceVoice(
		&this->pVoice,
		this->pAudio->GetWaveFormatEx(),
		0,                          // UINT32 Flags = 0,
		XAUDIO2_DEFAULT_FREQ_RATIO, // float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,
		this->pVoiceCallback                   // IXAudio2VoiceCallback *pCallback = NULL,
										  // const XAUDIO2_VOICE_SENDS *pSendList = NULL,
										  // const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL
	);
	if (FAILED(ret)) {
		OutputDebugStringEx("error CreateSourceVoice ret=%d\n", ret);
		return false;
	}
	return true;
}

void AudioHandler::Start(bool isLoopPlayback) {
	this->isLoopPlayback = isLoopPlayback;
	Start();
}

void AudioHandler::Start(IAudioHandlerDelegate* pDelegate) {
	this->pDelegate = pDelegate;
	Start();
}

void AudioHandler::Start(void(*onPlayedToEndCallback)(const char* pName)) {
	this->onPlayedToEndCallback = onPlayedToEndCallback;
	Start();
}

void AudioHandler::Stop() {
	Stop(true);
}

void AudioHandler::Pause() {
	this->pVoice->Stop();
	this->status = PlayingStatus::Pausing;
}

void AudioHandler::Resume() {
	this->status = PlayingStatus::Playing;
	this->pVoice->Start();
}

void AudioHandler::BufferEndCallback() {
	Push();
}

void AudioHandler::Push() {
	if (this->readBufffers == nullptr) {
		return;
	}

	// 音データ格納
	long size = this->pAudio->Read(this->readBufffers[this->buf_cnt], this->readLength);
	if (size <= 0 && this->isLoopPlayback) {
		this->pAudio->Reset();
		size = this->pAudio->Read(this->readBufffers[this->buf_cnt], this->readLength);
	}

	if (size <= 0) {
		this->Stop(false);
		if (this->pDelegate != nullptr) {
			this->pDelegate->OnPlayedToEnd(this->pName);
			this->pDelegate = nullptr;
		} else if (this->onPlayedToEndCallback != nullptr) {
			this->onPlayedToEndCallback(this->pName);
			this->onPlayedToEndCallback = nullptr;
		}

		return;
	}

	this->xAudioBuffer.AudioBytes = size;
	this->xAudioBuffer.pAudioData = this->readBufffers[this->buf_cnt];
	HRESULT ret = this->pVoice->SubmitSourceBuffer(&this->xAudioBuffer);
	if (FAILED(ret)) {
		OutputDebugStringEx("error SubmitSourceBuffer ret=%d\n", ret);
		return;
	}

	if (BUF_LEN <= ++this->buf_cnt) {
		this->buf_cnt = 0;
	}
}

void AudioHandler::Start() {
	this->xAudioBuffer = { 0 };
	this->readBufffers = new BYTE*[BUF_LEN];
	this->readLength = this->pAudio->GetWaveFormatEx()->nAvgBytesPerSec;
	for (int i = 0; i < BUF_LEN; ++i) {
		this->readBufffers[i] = new BYTE[this->readLength];
	}

	this->buf_cnt = 0;

	this->Push();

	this->status = PlayingStatus::Playing;
	this->pVoice->Start();
}

void AudioHandler::Stop(bool clearsCallback) {
	this->pVoice->Stop();
	this->status = PlayingStatus::Stopped;
	this->pAudio->Reset();
	if (this->readBufffers != nullptr) {
		for (int i = 0; i < BUF_LEN; i++) {
			delete[] this->readBufffers[i];
		}
		delete this->readBufffers;
		this->readBufffers = nullptr;
	}

	if (clearsCallback) {
		this->pDelegate = nullptr;
		this->onPlayedToEndCallback = nullptr;
	}
}
}
