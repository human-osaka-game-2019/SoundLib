#ifndef COMPRESSED_AUDIO_H
#define COMPRESSED_AUDIO_H

#include "IAudio.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

namespace SoundLib {
namespace Audio {

class CompressedAudio : public IAudio {
public:
	CompressedAudio();
	~CompressedAudio();

	const WAVEFORMATEX* GetWaveFormatEx();
	const TCHAR* GetFormatName();
	int GetChannelCount();
	int GetSamplingRate();
	int GetBitsPerSample();

	bool Load(const TCHAR* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	void Reset();

private:
	AVFormatContext* pFormatContext;
	AVStream* pAudioStream;
	AVCodec* pCodec;
	AVCodecContext* pCodecContext;
	SwrContext* pSwr;
	WAVEFORMATEX waveFormatEx;
	AVPacket* pPacket;

	bool CreateCodecContext();
};

}
}
#endif
