#ifndef WMA_AUDIO_H
#define WMA_AUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include "IAudio.h"


namespace SoundLib {
namespace Audio {

class WmaAudio : public IAudio {
public:
	WmaAudio();
	~WmaAudio();
	bool Load(const TCHAR* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize) { return 0; }
	long Read(BYTE* pBuffer, DWORD bufSize, XAUDIO2_BUFFER* pBbufinfo, XAUDIO2_BUFFER_WMA* pXbw);
	const WAVEFORMATEX* GetWaveFormatEx();
	void Reset();

private:
	HMMIO hMmio;
	WAVEFORMATEX waveFormatEx;
	WAVEFORMATEXTENSIBLE waveFormatExtensible;
	int packetCount;
	UINT32* decodedPacketCumulativeBytes;
	long pos;
};

}
}
#endif
