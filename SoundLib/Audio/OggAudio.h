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

	const WAVEFORMATEX* GetWaveFormatEx() const;
	TString GetFormatName() const;
	int GetChannelCount() const;
	int GetSamplingRate() const;
	int GetBitsPerSample() const;
	bool HasReadToEnd() const;

	bool Load(TString filePath);
	long Read(BYTE* pBuffer, DWORD bufSize);
	void Reset();

private:
	OggVorbis_File ovf;
	bool hasOpenedFile;
	WAVEFORMATEX waveFormatEx;
	bool hasReadToEnd;
};

}
}
#endif
