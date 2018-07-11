#include "Mp3Audio.h"
#include "../Common.h"


namespace {
// ビットレートのテーブル
const WORD BIT_RATE_TABLE[][16] = {
	// MPEG1, Layer1
	{ 0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,-1 },
	// MPEG1, Layer2
	{ 0,32,48,56,64,80,96,112,128,160,192,224,256,320,384,-1 },
	// MPEG1, Layer3
	{ 0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,-1 },
	// MPEG2/2.5, Layer1,2
	{ 0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,-1 },
	// MPEG2/2.5, Layer3
	{ 0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,-1 }
};

// サンプリングレートのテーブル
const WORD SAMPLE_RATE_TABLE[][4] = {
	{ 44100, 48000, 32000, -1 }, // MPEG1
	{ 22050, 24000, 16000, -1 }, // MPEG2
	{ 11025, 12000, 8000, -1 } // MPEG2.5
};
}

namespace SoundLib {
namespace Audio {

Mp3Audio::Mp3Audio() : hFile(nullptr), has(nullptr), pAsh(nullptr), pos(0) {}

Mp3Audio::~Mp3Audio() {
	// ACMの後始末
	if (this->pAsh != nullptr) {
		acmStreamUnprepareHeader(this->has, this->pAsh, 0);
		// 動的確保したデータを開放
		delete[] this->pAsh->pbSrc;
		delete[] this->pAsh->pbDst;

		delete this->pAsh;
		this->pAsh = nullptr;
	}

	if (this->has != nullptr) {
		acmStreamClose(this->has, 0);
		this->has = nullptr;
	}

	// ファイルを閉じる
	if (this->hFile != nullptr) {
		CloseHandle(this->hFile);
		this->hFile = nullptr;
	}
}


const WAVEFORMATEX* Mp3Audio::GetWaveFormatEx() {
	return &this->waveFormatEx;
}

const TCHAR* Mp3Audio::GetFormatName() {
	return _T("mp3");
}

int Mp3Audio::GetChannelCount() {
	return this->channelCount;
}

int Mp3Audio::GetSamplingRate() {
	return this->sampleRate;
}

int Mp3Audio::GetBitsPerSample() {
	return this->bitsPerSample;
}


bool Mp3Audio::Load(const TCHAR* pFilePath) {
	// ファイルを開く
	this->hFile = CreateFile(
		pFilePath,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (this->hFile == INVALID_HANDLE_VALUE)
		return FALSE; // エラー

	// ファイルサイズ取得
	this->mp3DataSize = GetDataSize();

	// フレームヘッダ確認
	BYTE header[4];
	DWORD readSize;

	ReadFile(this->hFile, header, 4, &readSize, NULL);
	if (!(header[0] == 0xFF && (header[1] & 0xE0) == 0xE0))
		return FALSE;

	// MPWGバージョン取得
	BYTE version = (header[1] >> 3) & 0x03;

	//　ビットレート取得
	WORD bitRatePerMilliSec = GetBitRate(header, version);

	// サンプルレート取得
	this->sampleRate = GetSampleRate(header, version);

	BYTE padding = header[2] >> 1 & 0x01;
	BYTE channel = header[3] >> 6;
	this->channelCount = (channel == 3 ? 1 : 2);

	// サイズ取得
	WORD blockSize = ((144 * bitRatePerMilliSec * 1000) / this->sampleRate) + padding;

	// フォーマット取得
	MPEGLAYER3WAVEFORMAT mf;
	mf.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
	mf.wfx.nChannels = this->channelCount;
	mf.wfx.nSamplesPerSec = this->sampleRate;
	mf.wfx.nAvgBytesPerSec = (bitRatePerMilliSec * 1000) / 8;
	mf.wfx.nBlockAlign = 1;
	mf.wfx.wBitsPerSample = 0;
	mf.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
	this->bitsPerSample = (bitRatePerMilliSec * 1000) / this->sampleRate;

	mf.wID = MPEGLAYER3_ID_MPEG;
	mf.fdwFlags = padding ? MPEGLAYER3_FLAG_PADDING_ON : MPEGLAYER3_FLAG_PADDING_OFF;
	mf.nBlockSize = blockSize;
	mf.nFramesPerBlock = 1;
	mf.nCodecDelay = 1393;

	// wavフォーマット取得
	this->waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	acmFormatSuggest(
		NULL,
		&mf.wfx,
		&this->waveFormatEx,
		sizeof(WAVEFORMATEX),
		ACM_FORMATSUGGESTF_WFORMATTAG
	);

	// ACMストリームを開く
	acmStreamOpen(&this->has, NULL, &mf.wfx, &this->waveFormatEx, NULL, 0, 0, 0);

	// WAV変換後のブロックサイズ取得
	DWORD mp3BlockSize = blockSize;
	DWORD waveBlockSize;
	acmStreamSize(this->has, mp3BlockSize, &waveBlockSize, ACM_STREAMSIZEF_SOURCE);

	this->pAsh = new ACMSTREAMHEADER();
	*(this->pAsh) = { 0 };
	this->pAsh->cbStruct = sizeof(ACMSTREAMHEADER);
	this->pAsh->pbSrc = new BYTE[mp3BlockSize];
	this->pAsh->cbSrcLength = mp3BlockSize;
	this->pAsh->pbDst = new BYTE[waveBlockSize];
	this->pAsh->cbDstLength = waveBlockSize;

	// デコード準備
	acmStreamPrepareHeader(this->has, this->pAsh, 0);

	return true;
}

long Mp3Audio::Read(BYTE* pBuffer, DWORD bufSize) {
	DWORD bufRead = 0;	// バッファを読み込んだサイズ

	while (this->mp3DataSize - this->pos >= this->pAsh->cbSrcLength) { // MP3データの終端？
		// １ブロック分だけMP3データを読み込む
		DWORD readSize;
		ReadFile(this->hFile, this->pAsh->pbSrc, this->pAsh->cbSrcLength, &readSize, NULL);
		UINT result = acmStreamConvert(this->has, this->pAsh, ACM_STREAMCONVERTF_BLOCKALIGN);
		if (result != 0) {
			return bufRead;
		}
		this->pos += readSize;

		if (bufSize - bufRead > this->pAsh->cbDstLengthUsed) {
			// WAVEデータを格納するバッファに余裕があれば、
			// デコードしたWAVEデータをそのまま格納
			CopyMemory(pBuffer + bufRead, this->pAsh->pbDst, this->pAsh->cbDstLengthUsed);
			bufRead += this->pAsh->cbDstLengthUsed;
		} else {
			// WAVEデータを格納するバッファに余裕がなければ、
			// バッファの残り分だけデータを書き込む
			CopyMemory(pBuffer + bufRead, this->pAsh->pbDst, bufSize - bufRead);
			bufRead += bufSize - bufRead;
			break;
		}
	}

	return bufRead;
}

void Mp3Audio::Reset() {
	// ファイルポインタをMP3データの開始位置に移動
	SetFilePointer(this->hFile, this->offset, NULL, FILE_BEGIN);
	this->pos = 0;
}

DWORD Mp3Audio::GetDataSize() {
	DWORD ret;
	DWORD fileSize = GetFileSize(this->hFile, NULL);

	BYTE header[10];
	DWORD readSize;

	// ヘッダの読み込み
	ReadFile(this->hFile, header, 10, &readSize, NULL);

	// 先頭３バイトのチェック
	if (memcmp(header, _T("ID3"), 3) == 0) {
		// タグサイズを取得
		DWORD tagSize = ((header[6] << 21) | (header[7] << 14) | (header[8] << 7) | (header[9])) + 10;

		// データの位置、サイズを計算
		this->offset = tagSize;
		ret = fileSize - offset;
	} else {
		// 末尾のタグに移動
		BYTE tag[3];
		SetFilePointer(this->hFile, fileSize - 128, NULL, FILE_BEGIN);
		ReadFile(this->hFile, tag, 3, &readSize, NULL);

		// データの位置、サイズを計算
		this->offset = 0;
		if (memcmp(tag, _T("TAG"), 3) == 0)
			ret = fileSize - 128; // 末尾のタグを省く
		else
			ret = fileSize; // ファイル全体がMP3データ
	}

	// ファイルポインタをMP3データの開始位置に移動
	SetFilePointer(this->hFile, this->offset, NULL, FILE_BEGIN);

	return ret;
}


WORD Mp3Audio::GetBitRate(BYTE header[], int version) {
	//　レイヤー数取得
	BYTE layer = (header[1] >> 1) & 0x03;

	INT index;
	if (version == 3) {
		index = 3 - layer;
	} else {
		if (layer == 3)
			index = 3;
		else
			index = 4;
	}

	return BIT_RATE_TABLE[index][header[2] >> 4];
}

WORD Mp3Audio::GetSampleRate(BYTE header[], int version) {
	int index;
	switch (version) {
	case 0:
		index = 2;
		break;
	case 2:
		index = 1;
		break;
	case 3:
		index = 0;
		break;
	}

	return SAMPLE_RATE_TABLE[index][(header[2] >> 2) & 0x03];
}

}
}
