#include "WaveAudio.h"
#include "../Common.h"


namespace SoundLib{
namespace Audio {

WaveAudio::WaveAudio() : hMmio(nullptr), offset(0), dataSize(0), restSize(0), hasReadToEnd(false) {}

WaveAudio::~WaveAudio() {
	if (this->hMmio != nullptr) {
		mmioClose(this->hMmio, 0);
		this->hMmio = nullptr;
	}
}


const WAVEFORMATEX* WaveAudio::GetWaveFormatEx() {
	return &this->waveFormatEx;
}

TString WaveAudio::GetFormatName() {
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

bool WaveAudio::HasReadToEnd() {
	return this->hasReadToEnd;
}


bool WaveAudio::Load(TString filePath) {
	// Waveファイルオープン
	this->hMmio = mmioOpen(const_cast<TCHAR*>(filePath.c_str()), nullptr, MMIO_READ);
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

	// データチャンクの先頭までの位置をシークし直してオフセットとして保持する
	this->offset = mmioSeek(this->hMmio, 0, SEEK_CUR);
	this->dataSize = dataChunk.cksize;
	this->restSize = this->dataSize;

	return true;
}

long WaveAudio::Read(BYTE* pBuffer, DWORD bufSize) {
	long readSize = 0;
	/*
	 * バッファサイズとファイルの残りサイズのうち小さい方を読み出しサイズとして設定する。
	 * ファイルの残りサイズより大きな値を設定してもファイル末尾までしか読み込みは行われないが、
	 * ファイルの音声データサイズが奇数の場合は末尾に1バイトのNULL文字が格納されており
	 * これも読み込んでしまうとノイズとなるので、音声データのみを読み込むようにする。
	 */
	long readableSize = bufSize > this->restSize ? this->restSize: bufSize;

	if (readableSize == 0) {
		this->hasReadToEnd = true;
	} else {
		// データの部分格納
		readSize = mmioRead(this->hMmio, (HPSTR)pBuffer, readableSize);
		this->restSize -= readSize;
	}
	return readSize;
}

void WaveAudio::Reset() {
	// ファイルポインタを先頭に戻す
	mmioSeek(this->hMmio, this->offset, SEEK_SET);
	this->restSize = this->dataSize;
	this->hasReadToEnd = false;
}

}
}