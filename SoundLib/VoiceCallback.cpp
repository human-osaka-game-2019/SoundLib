#include "VoiceCallback.h"
#include "Common.h"


VoiceCallback::VoiceCallback(IVoiceCallbackDelegate* pDelegate) : pDelegate(pDelegate) {}

void STDMETHODCALLTYPE VoiceCallback::OnStreamEnd() { 
	OutputDebugStringEx("%s\n", __func__); 
}

void STDMETHODCALLTYPE VoiceCallback::OnBufferEnd(void * pBufferContext) {
	this->pDelegate->BufferEndCallback();
}

void STDMETHODCALLTYPE VoiceCallback::OnLoopEnd(void * pBufferContext) {
	OutputDebugStringEx("%s\n", __func__); 
}

void STDMETHODCALLTYPE VoiceCallback::OnVoiceError(void * pBufferContext, HRESULT Error) { 
	OutputDebugStringEx("%s\n", __func__); 
}
