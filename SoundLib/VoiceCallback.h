#ifndef VOICE_CALLBACK_H
#define VOICE_CALLBACK_H

#include <xaudio2.h>
#include "IVoiceCallbackDelegate.h"


namespace SoundLib {
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	explicit VoiceCallback(IVoiceCallbackDelegate* pDelegate);
	VoiceCallback(VoiceCallback&&) = default;
	~VoiceCallback() = default;

	void STDMETHODCALLTYPE OnStreamEnd();
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
	void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext);
	void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) {}
	void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext);
	void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT Error);

private:
	VoiceCallback() = delete;
	VoiceCallback(const VoiceCallback&) = delete;
	VoiceCallback& operator=(const VoiceCallback&) = delete;

	IVoiceCallbackDelegate* pDelegate;
};
}
#endif
