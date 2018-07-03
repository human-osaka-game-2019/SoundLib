#ifndef IAUDIO_H
#define IAUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

class IAudio {
public:
	virtual ~IAudio() {};
	virtual bool Load(const char* pFilePath) = 0;
	virtual long Read(BYTE* pBuffer, DWORD bufSize) = 0;
	virtual const WAVEFORMATEX* GetWaveFormatEx() = 0;
	virtual void Reset() = 0;
};

#endif