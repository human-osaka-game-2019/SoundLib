//----------------------------------------------------------
// <filename>IVoiceCallbackDelegate.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef IVOICE_CALLBACK_DELEGATE_H
#define IVOICE_CALLBACK_DELEGATE_H


namespace SoundLib {
/// <summary>
/// <see cref="VoiceCallback"/>のコールバック定義用インターフェース
/// </summary>
class IVoiceCallbackDelegate {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IVoiceCallbackDelegate() = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// XAusio2再生用バッファが空になったタイミングで呼び出されるコールバック関数。
	/// </summary>
	virtual void OnBufferEnd() = 0;

	/// <summary>
	/// XAudio2再生用バッファの再生がすべて完了したタイミングで呼び出されるコールバック関数。
	/// </summary>
	virtual void OnStreamEnd() = 0;
};
}
#endif
