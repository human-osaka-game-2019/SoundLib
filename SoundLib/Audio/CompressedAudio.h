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
	CompressedAudio(CompressedAudio&&) = default;
	~CompressedAudio();

	const WAVEFORMATEX* GetWaveFormatEx() const;
	TString GetFormatName() const;
	int GetChannelCount() const;
	int GetSamplingRate() const;
	int GetBitsPerSample() const;
	bool HasReadToEnd() const;

	CompressedAudio& operator=(CompressedAudio&&) = default;

	bool Load(TString filePath);
	long Read(BYTE* pBuffer, long bufSize);
	void Reset();

private:
	AVFormatContext* pFormatContext;
	AVStream* pAudioStream;
	AVCodec* pCodec;
	AVCodecContext* pCodecContext;
	SwrContext* pSwr;
	WAVEFORMATEX waveFormatEx;
	AVPacket* pPacket;
	AVFrame* pFrame;
	bool hasReadToEnd;
	BYTE* pRemainingConvertedBuf;
	int remainingConvertedBufSize;

	CompressedAudio(const CompressedAudio&) = delete;
	CompressedAudio& operator=(const CompressedAudio&) = delete;

	bool CreateCodecContext();
	long ConvertPcmFormat(BYTE* pBuffer, long bufSize);
};

}
}
#endif
