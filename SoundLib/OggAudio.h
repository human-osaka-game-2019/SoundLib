#ifndef OGG_AUDIO_H
#define OGG_AUDIO_H

#include "IAudio.h"
#include "vorbis/vorbisfile.h"


namespace SoundLib {
class OggAudio : public IAudio {
public:
	OggAudio();
	~OggAudio();
	bool Load(const TCHAR* pFilePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	const WAVEFORMATEX* GetWaveFormatEx();
	void Reset();

private:
	OggVorbis_File ovf;
	bool hasOpenedFile;
	WAVEFORMATEX waveFormatEx;
};
}
#endif
