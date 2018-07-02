#ifndef VOICE_CALLBACK_H
#define VOICE_CALLBACK_H

#include <xaudio2.h>
#include "IVoiceCallbackDelegate.h"


class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	explicit VoiceCallback(IVoiceCallbackDelegate* pDelegate);
	~VoiceCallback() {}
	void STDMETHODCALLTYPE OnStreamEnd();
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
	void STDMETHODCALLTYPE OnBufferEnd(void * pBufferContext);
	void STDMETHODCALLTYPE OnBufferStart(void * pBufferContext) {}
	void STDMETHODCALLTYPE OnLoopEnd(void * pBufferContext);
	void STDMETHODCALLTYPE OnVoiceError(void * pBufferContext, HRESULT Error);

private:
	IVoiceCallbackDelegate* pDelegate;

	VoiceCallback();
	VoiceCallback(const VoiceCallback&);
};

#endif
