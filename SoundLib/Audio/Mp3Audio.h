#ifndef MP3_AUDIO_H
#define MP3_AUDIO_H

#include "IAudio.h"
#include <mmreg.h>
#include <msacm.h>


namespace SoundLib {
namespace Audio {

class Mp3Audio : public IAudio {
public:
	Mp3Audio();
	~Mp3Audio();

	const WAVEFORMATEX* GetWaveFormatEx();
	TString GetFormatName();
	int GetChannelCount();
	int GetSamplingRate();
	int GetBitsPerSample();

	bool Load(TString filePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	void Reset();

private:
	HANDLE hFile;
	DWORD offset; // MP3データの位置
	DWORD mp3DataSize; // MP3データのサイズ
	int channelCount;
	int bitsPerSample;
	WORD sampleRate;
	WAVEFORMATEX waveFormatEx;
	ACMSTREAMHEADER* pAsh;
	HACMSTREAM has;
	long pos;

	DWORD GetDataSize();
	WORD GetBitRate(BYTE header[], int version);
	WORD GetSampleRate(BYTE header[], int version);
};

}
}
#endif
