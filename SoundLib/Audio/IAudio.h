#ifndef IAUDIO_H
#define IAUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "../Common.h"


namespace SoundLib {
namespace Audio {

class IAudio {
public:
	virtual ~IAudio() {};

	virtual const WAVEFORMATEX* GetWaveFormatEx() const = 0;
	virtual TString GetFormatName() const = 0;
	virtual int GetChannelCount() const = 0;
	virtual int GetSamplingRate()const = 0;
	virtual int GetBitsPerSample() const = 0;
	virtual bool HasReadToEnd() const = 0;

	virtual bool Load(TString filePath) = 0;
	virtual long Read(BYTE* pBuffer, DWORD bufSize) = 0;
	virtual void Reset() = 0;
};

}
}

#endif
