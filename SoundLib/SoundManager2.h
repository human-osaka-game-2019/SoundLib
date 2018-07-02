#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include "IVoiceCallbackDelegate.h"
#include "VoiceCallback.h"
#include "Mp3Reader.h"


class SoundManager : public IVoiceCallbackDelegate {
public:
	SoundManager();
	~SoundManager();
	bool Initialize();
	bool OpenSoundFile(char* filePath);
	bool Start(bool isLoopPlayback);
	void Stop();
	void Pause();
	void Resume();
	void BufferEndCallback();

private:
	const int BUF_LEN = 2;

	Mp3Reader reader;
	WAVEFORMATEX waveFormatEx;
	IXAudio2* audio;
	IXAudio2SourceVoice* pVoice;
	VoiceCallback* pCallback;
	XAUDIO2_BUFFER buffer;
	BYTE** buf;
	int len;
	int buf_cnt;
	bool isLoopPlayback;

	void Push();

};
#endif
