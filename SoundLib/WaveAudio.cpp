#include "WaveAudio.h"
#include "Common.h"


namespace SoundLib{
WaveAudio::WaveAudio() : hMmio(nullptr), pos(0) {

}

WaveAudio::~WaveAudio() {
	if (this->hMmio != nullptr) {
		mmioClose(this->hMmio, 0);
		this->hMmio = nullptr;
	}
}

bool WaveAudio::Load(const char* pFilePath) {
	MMIOINFO mmioInfo;

	// Waveファイルオープン
	memset(&mmioInfo, 0, sizeof(MMIOINFO));

	this->hMmio = mmioOpen((LPSTR)pFilePath, &mmioInfo, MMIO_READ);
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
	DWORD size = mmioRead(this->hMmio, (HPSTR)&this->waveFormatEx, fmsize);
	if (size != fmsize) {
		OutputDebugStringEx("error mmioRead(fmt) size=%d\n", size);
		mmioClose(this->hMmio, 0);
		return false;
	}

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

long WaveAudio::Read(BYTE* pBuffer, DWORD bufSize) {
	// データの部分格納
	long size = mmioRead(this->hMmio, (HPSTR)pBuffer, bufSize);

	this->pos += size;   // ファイルポインタのオフセット値

	return size;
}

const WAVEFORMATEX* WaveAudio::GetWaveFormatEx() {
	return &this->waveFormatEx;
}

void WaveAudio::Reset() {
	mmioSeek(this->hMmio, -this->pos, SEEK_CUR);
	this->pos = 0;   // ファイルポインタを先頭に戻す
}
}