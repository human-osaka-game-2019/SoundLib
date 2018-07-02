#include "VoiceCallback.h"

#include <stdio.h>
#include <tchar.h>


#ifdef _DEBUG
#define OutputDebugStringEx( str, ... ) \
      { \
        TCHAR c[256]; \
        _stprintf_s( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#    define OutputDebugString( str, ... ) // ‹óŽÀ‘•
#endif


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
