#ifndef AAC_AUDIO_H
#define AAC_AUDIO_H

#include "IAudio.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

namespace SoundLib {
namespace Audio {

class AacAudio : public IAudio {
public:
	AacAudio();
	~AacAudio();
	bool Load(const TCHAR* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	const WAVEFORMATEX* GetWaveFormatEx();
	void Reset();

private:
	AVFormatContext* pFormatContext;
	AVStream* pAudioStream;
	AVCodecContext* pCodecContext;
	SwrContext* pSwr;
	WAVEFORMATEX waveFormatEx;
	AVPacket* pPacket;
};

}
}
#endif
