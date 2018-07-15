#include "AudioHandler.h"
#include <typeinfo>
#include "Common.h"


namespace SoundLib {
AudioHandler::AudioHandler(TString name, Audio::IAudio* pAudio) : 
	name(name), 
	pAudio(pAudio), 
	pVoice(nullptr), 
	pReadBuffers(nullptr), 
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

float AudioHandler::GetVolume() {
	float volume;
	this->pVoice->GetVolume(&volume);
	return volume;
}

bool AudioHandler::SetVolume(float volume) {
	HRESULT result = this->pVoice->SetVolume(volume);
	if (FAILED(result)) {
		OutputDebugStringEx(_T("error SetVolume resule=%d\n"), result);
		return false;
	}
	return true;
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
	if (this->pReadBuffers == nullptr) {
		return;
	}

	if (this->pAudio->HasReadToEnd()) {
		if (this->isLoopPlayback) {
			this->pAudio->Reset();
		} else {
			this->Stop(false);
			if (this->pDelegate != nullptr) {
				this->pDelegate->OnPlayedToEnd(this->name);
				this->pDelegate = nullptr;
			} else if (this->onPlayedToEndCallback != nullptr) {
				this->onPlayedToEndCallback(this->name.c_str());
				this->onPlayedToEndCallback = nullptr;
			}

			return;
		}
	}

	// 音データ格納
	memset(this->pReadBuffers[this->currentBufNum], 0, this->bufferSize);
	long size = this->pAudio->Read(this->pReadBuffers[this->currentBufNum], this->bufferSize);

	if (size <= 0) {
		if (this->pAudio->HasReadToEnd()) {
			// ファイル末尾まで再生した後の処理
			Push();
		} else {
			// エラー発生による停止
			this->Stop(true);
		}

		return;
	}

	this->xAudioBuffer.AudioBytes = size;
	this->xAudioBuffer.pAudioData = this->pReadBuffers[this->currentBufNum];
	HRESULT ret = this->pVoice->SubmitSourceBuffer(&this->xAudioBuffer);
	if (FAILED(ret)) {
		OutputDebugStringEx(_T("error SubmitSourceBuffer ret=%d\n"), ret);
		return;
	}

	if (BUF_COUNT <= ++this->currentBufNum) {
		this->currentBufNum = 0;
	}
}

void AudioHandler::Start() {
	this->xAudioBuffer = { 0 };
	this->pReadBuffers = new BYTE*[BUF_COUNT];
	this->bufferSize = this->pAudio->GetWaveFormatEx()->nAvgBytesPerSec;
	for (int i = 0; i < BUF_COUNT; ++i) {
		this->pReadBuffers[i] = new BYTE[this->bufferSize];
	}

	this->currentBufNum = 0;

	this->Push();

	this->status = PlayingStatus::Playing;
	this->pVoice->Start();
}

void AudioHandler::Stop(bool clearsCallback) {
	this->pVoice->Stop();
	this->status = PlayingStatus::Stopped;

	if (this->pReadBuffers != nullptr) {
		for (int i = 0; i < BUF_COUNT; i++) {
			delete[] this->pReadBuffers[i];
		}
		delete this->pReadBuffers;
		this->pReadBuffers = nullptr;
	}
	this->pVoice->FlushSourceBuffers();

	this->pAudio->Reset();

	if (clearsCallback) {
		this->pDelegate = nullptr;
		this->onPlayedToEndCallback = nullptr;
	}
}
}
