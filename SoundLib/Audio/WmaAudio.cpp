#include "WmaAudio.h"
#include "../Common.h"


namespace SoundLib{
namespace Audio {

WmaAudio::WmaAudio() : hMmio(nullptr), pos(0) {}

WmaAudio::~WmaAudio() {
	if (this->hMmio != nullptr) {
		mmioClose(this->hMmio, 0);
		this->hMmio = nullptr;
	}
}

bool WmaAudio::Load(const TCHAR* pFilePath) {
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
	riffChunk.fccType = mmioFOURCC(_T('w'), _T('m'), _T('a'), _T(' '));
	mmRes = mmioDescend(this->hMmio, &riffChunk, NULL, MMIO_FINDRIFF);
	if (mmRes != MMSYSERR_NOERROR) {
		OutputDebugStringEx(_T("error mmioDescend(wma) ret=%d\n"), mmRes);
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
	dataChunk.ckid = mmioFOURCC(_T('d'), _T('p'), _T('d'), _T('s'));
	mmRes = mmioDescend(this->hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		OutputDebugStringEx(_T("error mmioDescend(dpds) ret=%d\n"), mmRes);
		mmioClose(this->hMmio, 0);
		return false;
	}

	this->packetCount = riffChunk.cksize / 4;
	this->decodedPacketCumulativeBytes = new UINT32[this->packetCount];

	// Read the 'dpds' chunk into m_aDPCB.  
	if (mmioRead(this->hMmio, (HPSTR)this->decodedPacketCumulativeBytes, riffChunk.cksize) != riffChunk.cksize) {
		OutputDebugStringEx(_T("error mmioRead(decodedPacketCumulativeBytes)\n"));
		return false;
	}

	// Ascend the input file out of the 'dpds' chunk.  
	if (0 != mmioAscend(this->hMmio, &riffChunk, 0)) {
		OutputDebugStringEx(_T("error mmioAscend(riffChunk)\n"));
		return false;
	}

	return true;
}

long WmaAudio::Read(BYTE* pBuffer, DWORD bufSize, XAUDIO2_BUFFER* pBbufinfo, XAUDIO2_BUFFER_WMA* pXbw) {
	// データの部分格納
	long size = mmioRead(this->hMmio, (HPSTR)pBuffer, bufSize);

	this->pos += size;   // ファイルポインタのオフセット値

	pBbufinfo->Flags = ((UINT32)size >= bufSize) ? 0 : XAUDIO2_END_OF_STREAM;
	pBbufinfo->AudioBytes = size;
	pBbufinfo->pAudioData = pBuffer;
	pBbufinfo->PlayBegin = 0;

	//const UINT32 SEND_PACKET_NUM = 48;//パケット送信数
	*pXbw = { 0 };
	/*
	if (sendPacketCnt == nPacketCount) {
		sendPacketCnt = 0;
	}
	if ((sendPacketCnt + SEND_PACKET_NUM) > nPacketCount) {
		xbw.PacketCount += (nPacketCount - sendPacketCnt);
		sendPacketCnt += (nPacketCount - sendPacketCnt);
	} else {
	*/
		pXbw->PacketCount = this->packetCount;
		pXbw->pDecodedPacketCumulativeBytes = this->decodedPacketCumulativeBytes;
		//sendPacketCnt += SEND_PACKET_NUM;
	//}

	return size;
}

const WAVEFORMATEX* WmaAudio::GetWaveFormatEx() {
	return &this->waveFormatEx;
}

void WmaAudio::Reset() {
	mmioSeek(this->hMmio, -this->pos, SEEK_CUR);
	this->pos = 0;   // ファイルポインタを先頭に戻す
}

}
}