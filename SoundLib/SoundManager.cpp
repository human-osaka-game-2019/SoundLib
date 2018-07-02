#include "SoundManager.h"
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
#    define OutputDebugString( str, ... ) // 空実装
#endif


SoundManager::SoundManager() : hMmio(nullptr), buf(nullptr) {
	this->pCallback = new VoiceCallback(this);
}

SoundManager::~SoundManager() {
	delete this->pCallback;
	delete this->audio;

	if (this->hMmio != nullptr) {
		this->Stop();
		mmioClose(this->hMmio, 0);
		this->hMmio = nullptr;
	}
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
	MMIOINFO mmioInfo;

	// Waveファイルオープン
	memset(&mmioInfo, 0, sizeof(MMIOINFO));

	this->hMmio = mmioOpen(filePath, &mmioInfo, MMIO_READ);
	if (!this->hMmio) {
		// ファイルオープン失敗
		OutputDebugStringEx("error mmioOpen\n");
		return false;
	}

	// RIFFチャンク検索
	MMRESULT mmRes;
	MMCKINFO riffChunk;
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmRes = mmioDescend(this->hMmio, &riffChunk, NULL, MMIO_FINDRIFF);
	if (mmRes != MMSYSERR_NOERROR) {
		OutputDebugStringEx("error mmioDescend(wave) ret=%d\n", mmRes);
		mmioClose(this->hMmio, 0);
		return false;
	}

	// フォーマットチャンク検索
	MMCKINFO formatChunk;
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmRes = mmioDescend(this->hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(this->hMmio, 0);
		return false;
	}

	// WAVEFORMATEX構造体格納
	DWORD fmsize = formatChunk.cksize;
	DWORD size = mmioRead(this->hMmio, (HPSTR)&(this->waveFormatEx), fmsize);
	if (size != fmsize) {
		OutputDebugStringEx("error mmioRead(fmt) size=%d\n", size);
		mmioClose(this->hMmio, 0);
		return false;
	}

	OutputDebugStringEx("foramt    =%d\n", this->waveFormatEx.wFormatTag);
	OutputDebugStringEx("channel   =%d\n", this->waveFormatEx.nChannels);
	OutputDebugStringEx("sampling  =%dHz\n", this->waveFormatEx.nSamplesPerSec);
	OutputDebugStringEx("bit/sample=%d\n", this->waveFormatEx.wBitsPerSample);
	OutputDebugStringEx("byte/sec  =%d\n", this->waveFormatEx.nAvgBytesPerSec);

	// WAVEFORMATEX構造体格納
	mmioAscend(this->hMmio, &formatChunk, 0);

	// データチャンク検索
	MMCKINFO dataChunk;
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmRes = mmioDescend(this->hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		OutputDebugStringEx("error mmioDescend(data) ret=%d\n", mmRes);
		mmioClose(this->hMmio, 0);
		return false;
	}

	return true;
}

bool SoundManager::Start() {
	HRESULT ret = this->audio->CreateSourceVoice(
		&this->voice,
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
	this->voice->Start();

	this->buffer = { 0 };
	this->buf = new BYTE*[BUF_LEN];
	this->len = this->waveFormatEx.nAvgBytesPerSec;
	for (int i = 0; i < BUF_LEN; i++) {
		this->buf[i] = new BYTE[this->len];
	}

	this->buf_cnt = 0;

	this->Push();
}

void SoundManager::Stop() {
	this->voice->Stop();
	this->voice->DestroyVoice();
	if (this->buf != nullptr) {
		for (int i = 0; i < BUF_LEN; i++) {
			delete[] this->buf[i];
		}
	}
	delete this->buf;
	this->buf = nullptr;
}

void SoundManager::BufferEndCallback() {
	this->Push();
}

long SoundManager::ReadSoundData() {
	static int curOffset = 0;

	// データの部分格納
	long size = mmioRead(this->hMmio, (HPSTR)this->buf[this->buf_cnt], this->len);
	if (size <= 0) {
		// 最後まで読み込んだ場合は最初に戻る
		//mmioSeek(hMmio, -curOffset, SEEK_CUR);
		//curOffset = 0;   // ファイルポインタを先頭に戻す
		//size = mmioRead(hMmio, (HPSTR)pData, readSize);
	}

	curOffset += size;   // ファイルポインタのオフセット値

	return size;
}

void SoundManager::Push() {
	// 音データ格納
	long size = ReadSoundData();
	if (size <= 0) {
		this->Stop();
		return;
	}
	this->buffer.AudioBytes = size;
	this->buffer.pAudioData = this->buf[this->buf_cnt];
	HRESULT ret = this->voice->SubmitSourceBuffer(&this->buffer);
	if (FAILED(ret)) {
		OutputDebugStringEx("error SubmitSourceBuffer ret=%d\n", ret);
		return;
	}
	if (BUF_LEN <= ++this->buf_cnt) {
		this->buf_cnt = 0;
	}
}


