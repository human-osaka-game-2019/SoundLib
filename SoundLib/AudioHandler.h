#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include "IVoiceCallbackDelegate.h"
#include "Audio/IAudio.h"
#include "VoiceCallback.h"
#include "IAudioHandlerDelegate.h"


namespace SoundLib {
enum PlayingStatus {
	Stopped,
	Playing,
	Pausing
};

class AudioHandler : public IVoiceCallbackDelegate {
public:
	AudioHandler(TString name,  Audio::IAudio* pAudio);
	~AudioHandler();

	PlayingStatus GetStatus();
	
	bool Prepare(IXAudio2* pXAudio2);
	void Start(bool isLoopPlayback);
	void Start(IAudioHandlerDelegate* pDelegate);
	void Start(void(*onPlayedToEndCallback)(const TCHAR* pName));
	void Stop();
	void Pause();
	void Resume();
	void BufferEndCallback();


private:
	const int BUF_COUNT = 2;

	TString name;
	Audio::IAudio * pAudio;
	IXAudio2SourceVoice* pVoice;
	VoiceCallback* pVoiceCallback;
	XAUDIO2_BUFFER xAudioBuffer;
	BYTE** pReadBuffers;
	int bufferSize;
	int currentBufNum;
	bool isLoopPlayback;
	IAudioHandlerDelegate* pDelegate;
	void(*onPlayedToEndCallback)(const TCHAR* pName);
	PlayingStatus status;

	void Push();
	void Start();
	void Stop(bool clearsCallback);
};
}

#endif
