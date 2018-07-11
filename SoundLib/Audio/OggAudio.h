#ifndef OGG_AUDIO_H
#define OGG_AUDIO_H

#include "IAudio.h"
#include "vorbis/vorbisfile.h"


namespace SoundLib {
namespace Audio {

class OggAudio : public IAudio {
public:
	OggAudio();
	~OggAudio();

	const WAVEFORMATEX* GetWaveFormatEx();
	const TCHAR* GetFormatName();
	int GetChannelCount();
	int GetSamplingRate();
	int GetBitsPerSample();

	bool Load(const TCHAR* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	void Reset();

private:
	OggVorbis_File ovf;
	bool hasOpenedFile;
	WAVEFORMATEX waveFormatEx;
};

}
}
#endif
