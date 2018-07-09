#include "AudioHandler.h"
#include "Common.h"


namespace SoundLib {
AudioHandler::AudioHandler(const TCHAR* pName, Audio::IAudio* pAudio) : 
	pName(pName), 
	pAudio(pAudio), 
	pVoice(nullptr), 
	readBuffers(nullptr), 
	pDelegate(nullptr), 
	onPlayedToEndCallback(nullptr), 
	status(PlayingStatus::Stopped) {
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
		OutputDebugStringEx(_T("error CreateSourceVoice ret=%d\n"), ret);
		return false;
	}
	return true;
}

void AudioHandler::Start(bool isLoopPlayback) {
	if (this->status == PlayingStatus::Pausing) {
		this->Stop(true);
	}
	if (this->status == PlayingStatus::Stopped) {
		this->isLoopPlayback = isLoopPlayback;
		Start();
	}
}

void AudioHandler::Start(IAudioHandlerDelegate* pDelegate) {
	if(this->status == PlayingStatus::Pausing) {
		this->Stop(true);
	}
	if (this->status == PlayingStatus::Stopped) {
		this->pDelegate = pDelegate;
		this->isLoopPlayback = false;
		Start();
	}
}

void AudioHandler::Start(void(*onPlayedToEndCallback)(const TCHAR* pName)) {
	if(this->status == PlayingStatus::Pausing) {
		this->Stop(true);
	}
	if (this->status == PlayingStatus::Stopped) {
		this->onPlayedToEndCallback = onPlayedToEndCallback;
		this->isLoopPlayback = false;
		Start();
	}
}

void AudioHandler::Stop() {
	if (this->status == PlayingStatus::Playing) {
		Stop(true);
	}
}

void AudioHandler::Pause() {
	if (this->status == PlayingStatus::Playing) {
		this->pVoice->Stop();
		this->status = PlayingStatus::Pausing;
	}
}

void AudioHandler::Resume() {
	if (this->status == PlayingStatus::Pausing) {
		this->status = PlayingStatus::Playing;
		this->pVoice->Start();
	}
}

void AudioHandler::BufferEndCallback() {
	Push();
}

void AudioHandler::Push() {
	if (this->readBuffers == nullptr) {
		return;
	}

	// 音データ格納
	memset(this->readBuffers[this->buf_cnt], 0, this->readLength);
	long size = this->pAudio->Read(this->readBuffers[this->buf_cnt], this->readLength);
	if (size <= 0 && this->isLoopPlayback) {
		this->pAudio->Reset();
		size = this->pAudio->Read(this->readBuffers[this->buf_cnt], this->readLength);
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
	this->xAudioBuffer.pAudioData = this->readBuffers[this->buf_cnt];
	HRESULT ret = this->pVoice->SubmitSourceBuffer(&this->xAudioBuffer);
	if (FAILED(ret)) {
		OutputDebugStringEx(_T("error SubmitSourceBuffer ret=%d\n"), ret);
		return;
	}

	if (BUF_LEN <= ++this->buf_cnt) {
		this->buf_cnt = 0;
	}
}

void AudioHandler::Start() {
	this->xAudioBuffer = { 0 };
	this->readBuffers = new BYTE*[BUF_LEN];
	this->readLength = this->pAudio->GetWaveFormatEx()->nAvgBytesPerSec;
	for (int i = 0; i < BUF_LEN; ++i) {
		this->readBuffers[i] = new BYTE[this->readLength];
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
	if (this->readBuffers != nullptr) {
		for (int i = 0; i < BUF_LEN; i++) {
			delete[] this->readBuffers[i];
		}
		delete this->readBuffers;
		this->readBuffers = nullptr;
	}

	if (clearsCallback) {
		this->pDelegate = nullptr;
		this->onPlayedToEndCallback = nullptr;
	}
}
}
