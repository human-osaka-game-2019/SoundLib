#ifndef WAVE_AUDIO_H
#define WAVE_AUDIO_H

#include "IAudio.h"


namespace SoundLib {
namespace Audio {

class WaveAudio : public IAudio {
public:
	WaveAudio();
	~WaveAudio();

	const WAVEFORMATEX* GetWaveFormatEx();
	const TCHAR* GetFormatName();
	int GetChannelCount();
	int GetSamplingRate();
	int GetBitsPerSample();

	bool Load(const TCHAR* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	void Reset();

private:
	HMMIO hMmio;
	WAVEFORMATEX waveFormatEx;
	long pos;
};

}
}
#endif
