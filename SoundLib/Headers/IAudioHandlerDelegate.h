//----------------------------------------------------------
// <filename>IAudioHandlerDelegate.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef IAUDIO_HANDLER_DELEGATE_H
#define IAUDIO_HANDLER_DELEGATE_H

#include "Common.h"

namespace SoundLib {
/// <summary>
/// <see cref="AudioHandler"/>のコールバック定義用インターフェース
/// </summary>
template <typename T>
class IAudioHandlerDelegate {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IAudioHandlerDelegate() = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// 音声ファイルの最後まで再生完了した後に呼び出すコールバック関数
	/// </summary>
	/// <param name="name">音声ファイルを識別する名前</param>
	virtual void OnPlayedToEnd(std::basic_string<T> name) = 0;
};
}
#endif
