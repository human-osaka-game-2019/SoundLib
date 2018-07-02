#ifndef MP3_READER_H
#define MP3_READER_H

#include <windows.h>
#include <mmreg.h>
#include <msacm.h>


class Mp3Reader {
public:
	~Mp3Reader();
	bool OpenFile(const char* pFileName, WAVEFORMATEX* pWf);
	int Read(BYTE* pBuffer, DWORD bufSize);

private:
	
	
	HANDLE hFile;
	DWORD offset; // MP3データの位置
	DWORD mp3DataSize; // MP3データのサイズ
	ACMSTREAMHEADER ash;
	HACMSTREAM has;

	DWORD GetDataSize();
	WORD GetBitRate(BYTE header[], int version);
	WORD GetSampleRate(BYTE header[], int version);
};

#endif