#include "OggAudio.h"
#include "../Common.h"


namespace SoundLib {
namespace Audio {

OggAudio::OggAudio() : hasOpenedFile(false), hasReadToEnd(false) {}

OggAudio::~OggAudio() {
	if (this->hasOpenedFile) {
		ov_clear(&this->ovf);
		this->hasOpenedFile = false;
	}
}


const WAVEFORMATEX* OggAudio::GetWaveFormatEx() {
	return &this->waveFormatEx;
}

TString OggAudio::GetFormatName() {
	return _T("Ogg Vorbis");
}

int OggAudio::GetChannelCount() {
	return ov_info(&this->ovf, -1)->channels;
}

int OggAudio::GetSamplingRate() {
	return ov_info(&this->ovf, -1)->rate;
}

int OggAudio::GetBitsPerSample() {
	vorbis_info* pVorbisInfo = ov_info(&this->ovf, -1);
	return (pVorbisInfo->bitrate_nominal / pVorbisInfo->rate);
}

bool OggAudio::HasReadToEnd() {
	return this->hasReadToEnd;
}


bool OggAudio::Load(TString filePath) {
	// ファイルを開く
	int error = ov_fopen(filePath.c_str(), &this->ovf);
	if (error != 0) {
		return false;
	}
	this->hasOpenedFile = true;

	int bitPerSample = 16; // 16bit固定
	vorbis_info* pVorbisInfo = ov_info(&this->ovf, -1);
	this->waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	this->waveFormatEx.nChannels = pVorbisInfo->channels;
	this->waveFormatEx.nSamplesPerSec = pVorbisInfo->rate;
	this->waveFormatEx.wBitsPerSample = bitPerSample;
	this->waveFormatEx.nBlockAlign = (bitPerSample / 8) * pVorbisInfo->channels;
	this->waveFormatEx.nAvgBytesPerSec = this->waveFormatEx.nSamplesPerSec * this->waveFormatEx.nBlockAlign;
	this->waveFormatEx.cbSize = 0;

	return true;
}

long OggAudio::Read(BYTE* pBuffer, DWORD bufSize) {
	DWORD bufRead = 0;	// バッファを読み込んだサイズ

	while (bufRead < bufSize) {
		long readSize = ov_read(&this->ovf, (char*)(pBuffer + bufRead), bufSize - bufRead, 0, 2, 1, nullptr);
		if (readSize == 0) {
			this->hasReadToEnd = true;
			break;
		} else if (readSize < 0) {
			break;
		}

		bufRead += readSize;
	}

	return bufRead;
}

void OggAudio::Reset() {
	// 読み込み位置を最初に戻す
	ov_time_seek(&this->ovf, 0.0);
	this->hasReadToEnd = false;
}

}
}
