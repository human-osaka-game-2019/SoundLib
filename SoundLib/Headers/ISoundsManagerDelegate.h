//----------------------------------------------------------
// <filename>ISoundsManagerDelegate.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef ISOUNDS_MANAGER_DELEGATE_H
#define ISOUNDS_MANAGER_DELEGATE_H

#include "IAudioHandlerDelegate.h"


namespace SoundLib {
/// <summary>
/// <see cref="SoundsManager"/>のコールバック定義用インターフェース
/// </summary>
template <typename T>
class ISoundsManagerDelegate : public IAudioHandlerDelegate<T> {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ISoundsManagerDelegate() = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// 音声ファイルの最後まで再生完了した後に呼び出すコールバック関数
	/// </summary>
	/// <param name="key">音声ファイルを識別するキー</param>
	virtual void OnPlayedToEnd(std::basic_string<T> key) = 0;
};
}
#endif
