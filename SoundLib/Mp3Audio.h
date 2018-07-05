#ifndef MP3_AUDIO_H
#define MP3_AUDIO_H

#include "IAudio.h"
#include <mmreg.h>
#include <msacm.h>


class Mp3Audio : public IAudio {
public:
	Mp3Audio();
	~Mp3Audio();
	bool Load(const char* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	const WAVEFORMATEX* GetWaveFormatEx();
	void Reset();

private:
	HANDLE hFile;
	DWORD offset; // MP3データの位置
	DWORD mp3DataSize; // MP3データのサイズ
	WAVEFORMATEX waveFormatEx;
	ACMSTREAMHEADER* pAsh;
	HACMSTREAM has;
	long pos;

	DWORD GetDataSize();
	WORD GetBitRate(BYTE header[], int version);
	WORD GetSampleRate(BYTE header[], int version);
};

#endif