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
	this->pDelegate->OnStreamEnd();
}

void STDMETHODCALLTYPE VoiceCallback::OnBufferEnd(void* pBufferContext) {
	this->pDelegate->OnBufferEnd();
}

void STDMETHODCALLTYPE VoiceCallback::OnLoopEnd(void* pBufferContext) {
	Common::OutputDebugString("%s\n", __func__);
}

void STDMETHODCALLTYPE VoiceCallback::OnVoiceError(void* pBufferContext, HRESULT Error) {
	Common::OutputDebugString("%s\n", __func__);
}
}
