#ifndef WAVE_AUDIO_H
#define WAVE_AUDIO_H

#include "IAudio.h"


namespace SoundLib {
class WaveAudio : public IAudio {
public:
	WaveAudio();
	~WaveAudio();
	bool Load(const TCHAR* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	const WAVEFORMATEX* GetWaveFormatEx();
	void Reset();

private:
	HMMIO hMmio;
	WAVEFORMATEX waveFormatEx;
	long pos;
};
}
#endif
