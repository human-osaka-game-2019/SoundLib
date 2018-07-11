#include "WaveAudio.h"
#include "../Common.h"


namespace SoundLib{
namespace Audio {

WaveAudio::WaveAudio() : hMmio(nullptr), pos(0) {}

WaveAudio::~WaveAudio() {
	if (this->hMmio != nullptr) {
		mmioClose(this->hMmio, 0);
		this->hMmio = nullptr;
	}
}


const WAVEFORMATEX* WaveAudio::GetWaveFormatEx() {
	return &this->waveFormatEx;
}

const TCHAR* WaveAudio::GetFormatName() {
	return _T("WAVE");
}

int WaveAudio::GetChannelCount() {
	return this->waveFormatEx.nChannels;
}

int WaveAudio::GetSamplingRate() {
	return this->waveFormatEx.nSamplesPerSec;
}

int WaveAudio::GetBitsPerSample() {
	return this->waveFormatEx.wBitsPerSample;
}


bool WaveAudio::Load(const TCHAR* pFilePath) {
	MMIOINFO mmioInfo;

	// Waveファイルオープン
	memset(&mmioInfo, 0, sizeof(MMIOINFO));

	this->hMmio = mmioOpen(const_cast<TCHAR*>(pFilePath), &mmioInfo, MMIO_READ);
	if (!this->hMmio) {
		// ファイルオープン失敗
		OutputDebugStringEx(_T("error mmioOpen\n"));
		return false;
	}

	// RIFFチャンク検索
	MMRESULT mmRes;
	MMCKINFO riffChunk;
	riffChunk.fccType = mmioFOURCC(_T('W'), _T('A'), _T('V'), _T('E'));
	mmRes = mmioDescend(this->hMmio, &riffChunk, NULL, MMIO_FINDRIFF);
	if (mmRes != MMSYSERR_NOERROR) {
		OutputDebugStringEx(_T("error mmioDescend(wave) ret=%d\n"), mmRes);
		mmioClose(this->hMmio, 0);
		return false;
	}

	// フォーマットチャンク検索
	MMCKINFO formatChunk;
	formatChunk.ckid = mmioFOURCC(_T('f'), _T('m'), _T('t'), _T(' '));
	mmRes = mmioDescend(this->hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(this->hMmio, 0);
		return false;
	}

	// WAVEFORMATEX構造体格納
	DWORD fmsize = formatChunk.cksize;
	DWORD size = mmioRead(this->hMmio, (HPSTR)&this->waveFormatEx, fmsize);
	if (size != fmsize) {
		OutputDebugStringEx(_T("error mmioRead(fmt) size=%d\n"), size);
		mmioClose(this->hMmio, 0);
		return false;
	}

	// WAVEFORMATEX構造体格納
	mmioAscend(this->hMmio, &formatChunk, 0);

	// データチャンク検索
	MMCKINFO dataChunk;
	dataChunk.ckid = mmioFOURCC(_T('d'), _T('a'), _T('t'), _T('a'));
	mmRes = mmioDescend(this->hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		OutputDebugStringEx(_T("error mmioDescend(data) ret=%d\n"), mmRes);
		mmioClose(this->hMmio, 0);
		return false;
	}

	return true;
}

long WaveAudio::Read(BYTE* pBuffer, DWORD bufSize) {
	// データの部分格納
	long size = mmioRead(this->hMmio, (HPSTR)pBuffer, bufSize);

	this->pos += size;   // ファイルポインタのオフセット値

	return size;
}

void WaveAudio::Reset() {
	mmioSeek(this->hMmio, -this->pos, SEEK_CUR);
	this->pos = 0;   // ファイルポインタを先頭に戻す
}

}
}