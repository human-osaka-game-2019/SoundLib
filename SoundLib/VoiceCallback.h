//----------------------------------------------------------
// <filename>VoiceCallback.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef VOICE_CALLBACK_H
#define VOICE_CALLBACK_H

#include <xaudio2.h>
#include "IVoiceCallbackDelegate.h"


namespace SoundLib {
/// <summary>
/// <see cref="IXAudio2VoiceCallback"/>実装クラス
/// </summary>
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pDelegate">コールバック関数を定義したオブジェクト</param>
	explicit VoiceCallback(IVoiceCallbackDelegate* pDelegate);

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	VoiceCallback(VoiceCallback&& obj) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~VoiceCallback() = default;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	/// <summary>
	/// ムーブ代入演算子のオーバーロード
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	/// <returns>ムーブ後のオブジェクト</returns>
	VoiceCallback& operator=(VoiceCallback&& obj) = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// Called when this voice has just finished playing a buffer stream
	/// (as marked with the XAUDIO2_END_OF_STREAM flag on the last buffer).
	/// </summary>
	void STDMETHODCALLTYPE OnStreamEnd();

	/// <summary>
	/// Called just after this voice's processing pass ends.
	/// </summary>
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {}

	/// <summary>
	/// Called just before this voice's processing pass begins.
	/// </summary>
	/// <param name="SamplesRequired">The number of bytes that must be submitted immediately to avoid starvation. </param>
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}

	/// <summary>
	/// Called when this voice has just finished processing a buffer.
	/// </summary>
	/// <param name="pBufferContext">Context pointer assigned to the pContext member of the XAUDIO2_BUFFER structure when the buffer was submitted.</param>
	/// <remarks>The buffer can now be reused or destroyed.</remarks>
	void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext);

	/// <summary>
	/// Called when this voice is about to start processing a new buffer.
	/// </summary>
	/// <param name="pBufferContext">Context pointer that was assigned to the pContext member of the XAUDIO2_BUFFER structure when the buffer was submitted.</param>
	void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) {}

	/// <summary>
	/// Called when this voice has just reached the end position of a loop.
	/// </summary>
	/// <param name="pBufferContext">Context pointer that was assigned to the pContext member of the XAUDIO2_BUFFER structure when the buffer was submitted.</param>
	void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext);

	/// <summary>
	/// Called in the event of a critical error during voice processing, such as a failing xAPO or an error from the hardware XMA decoder.
	/// </summary>
	/// <param name="pBufferContext">reports which buffer was being processed when the error occurred</param>
	/// <param name="Error">its HRESULT code</param>
	/// <remarks>
	/// The voice may have to be destroyed and re-created to recover from the error.
	/// </remarks>
	void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT Error);

private:
	/* Variables ---------------------------------------------------------------------------------------- */
	IVoiceCallbackDelegate* pDelegate;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	VoiceCallback() = delete;
	VoiceCallback(const VoiceCallback&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	VoiceCallback& operator=(const VoiceCallback&) = delete;
};
}
#endif
