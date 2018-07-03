#include "AudioHandler.h"
#include "Common.h"


AudioHandler::AudioHandler(IAudio* pAudio) : pAudio(pAudio), pVoice(nullptr) {
	this->pCallback = new VoiceCallback(this);
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

	delete this->pCallback;
}

bool AudioHandler::Prepare(IXAudio2* pXAudio2) {
	HRESULT ret = pXAudio2->CreateSourceVoice(
		&this->pVoice,
		this->pAudio->GetWaveFormatEx(),
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
	return true;
}

void AudioHandler::Start(bool isLoopPlayback) {
	this->isLoopPlayback = isLoopPlayback;

	this->xAudioBuffer = { 0 };
	this->readBufffers = new BYTE*[BUF_LEN];
	this->readLength = this->pAudio->GetWaveFormatEx()->nAvgBytesPerSec;
	for (int i = 0; i < BUF_LEN; ++i) {
		this->readBufffers[i] = new BYTE[this->readLength];
	}

	this->buf_cnt = 0;

	this->Push();

	this->pVoice->Start();
}

void AudioHandler::Stop() {
	this->pVoice->Stop();
	this->pAudio->Reset();
	if (this->readBufffers != nullptr) {
		for (int i = 0; i < BUF_LEN; i++) {
			delete[] this->readBufffers[i];
		}
		delete this->readBufffers;
		this->readBufffers = nullptr;
	}
}

void AudioHandler::Pause() {
	this->pVoice->Stop();
}

void AudioHandler::Resume() {
	this->pVoice->Start();
}

void AudioHandler::BufferEndCallback() {
	this->Push();
}

void AudioHandler::Push() {
	if (this->readBufffers == nullptr) {
		return;
	}

	// ‰¹ƒf[ƒ^Ši”[
	long size = this->pAudio->Read(this->readBufffers[this->buf_cnt], this->readLength);
	if (size <= 0 && this->isLoopPlayback) {
		size = this->pAudio->Read(this->readBufffers[this->buf_cnt], this->readLength);
	}

	if (size <= 0) {
		this->Stop();
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