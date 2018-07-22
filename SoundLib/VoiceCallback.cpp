//----------------------------------------------------------
// <filename>VoiceCallback.cpp</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#include "VoiceCallback.h"
#include "Common.h"


namespace SoundLib {
/* Constructor / Destructor ------------------------------------------------------------------------- */
VoiceCallback::VoiceCallback(IVoiceCallbackDelegate* pDelegate) : pDelegate(pDelegate) {}


/* Public Functions  -------------------------------------------------------------------------------- */
void STDMETHODCALLTYPE VoiceCallback::OnStreamEnd() {
	OutputDebugStringEx("%s\n", __func__);
}

void STDMETHODCALLTYPE VoiceCallback::OnBufferEnd(void* pBufferContext) {
	this->pDelegate->BufferEndCallback();
}

void STDMETHODCALLTYPE VoiceCallback::OnLoopEnd(void* pBufferContext) {
	OutputDebugStringEx("%s\n", __func__);
}

void STDMETHODCALLTYPE VoiceCallback::OnVoiceError(void* pBufferContext, HRESULT Error) {
	OutputDebugStringEx("%s\n", __func__);
}
}
