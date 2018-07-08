#include "VoiceCallback.h"
#include "Common.h"


namespace SoundLib {
VoiceCallback::VoiceCallback(IVoiceCallbackDelegate* pDelegate) : pDelegate(pDelegate) {}

void STDMETHODCALLTYPE VoiceCallback::OnStreamEnd() {
	OutputDebugStringEx(_T("%s\n"), __func__);
}

void STDMETHODCALLTYPE VoiceCallback::OnBufferEnd(void* pBufferContext) {
	this->pDelegate->BufferEndCallback();
}

void STDMETHODCALLTYPE VoiceCallback::OnLoopEnd(void* pBufferContext) {
	OutputDebugStringEx(_T("%s\n"), __func__);
}

void STDMETHODCALLTYPE VoiceCallback::OnVoiceError(void* pBufferContext, HRESULT Error) {
	OutputDebugStringEx(_T("%s\n"), __func__);
}
}
