#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include "IVoiceCallbackDelegate.h"
#include "IAudio.h"
#include "VoiceCallback.h"


class AudioHandler : public IVoiceCallbackDelegate {
public:
	AudioHandler(IAudio* pAudio);
	~AudioHandler();
	bool Prepare(IXAudio2* pXAudio2);
	void Start(bool isLoopPlayback);
	void Stop();
	void Pause();
	void Resume();
	void BufferEndCallback();

private:
	const int BUF_LEN = 2;

	IAudio * pAudio;
	IXAudio2SourceVoice* pVoice;
	VoiceCallback* pCallback;
	XAUDIO2_BUFFER xAudioBuffer;
	BYTE** readBufffers;
	int readLength;
	int buf_cnt;
	bool isLoopPlayback;

	void Push();
};

#endif
