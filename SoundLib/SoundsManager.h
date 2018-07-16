//----------------------------------------------------------
// <filename>SoundsManager.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef SOUNDS_MANAGER_H
#define SOUNDS_MANAGER_H

#include <unordered_map>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include <stdint.h>
#include "Common.h"
#include "AudioHandler.h"
#include "ISoundsManagerDelegate.h"


namespace SoundLib {
/// <summary>
/// サウンド統括クラス
/// </summary>
class SoundsManager {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SoundsManager();

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	SoundsManager(SoundsManager&& obj) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SoundsManager();

	/* Getters / Setters -------------------------------------------------------------------------------- */
	/// <summary>
	/// 再生状況を示すステータスを取得する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>再生ステータス</returns>
	PlayingStatus GetStatus(const TCHAR* pKey) const;

	/// <summary>
	/// ボリュームを取得する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>ボリューム(0:無音　100:音源ボリューム)</returns>
	uint8_t GetVolume(const TCHAR* pKey) const;

	/// <summary>
	/// ボリュームを設定する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="volume">ボリューム(0:無音　100:音源ボリューム)</param>
	/// <returns>成否</returns>
	bool SetVolume(const TCHAR* pKey, uint8_t volume);

	/* Operator Overloads ------------------------------------------------------------------------------- */
	/// <summary>
	/// ムーブ代入演算子のオーバーロード
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	/// <returns>ムーブ後のオブジェクト</returns>
	SoundsManager& operator=(SoundsManager&& obj) = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// XAudio2の初期化処理を行う。
	/// </summary>
	/// <returns>成否</returns>
	/// <remarks>ライブラリ使用時はまず当メソッドを呼び出して下さい。</remarks>
	bool Initialize();

	/// <summary>
	/// 音声ファイルを追加する。
	/// </summary>
	/// <param name="pFilePath">ファイルパス</param>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>成否</returns>
	bool AddFile(const TCHAR* pFilePath, const TCHAR* pKey);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="isLoopPlayback">ループ再生を行うかどうか</param>
	/// <returns>成否</returns>
	bool Start(const TCHAR* pKey, bool isLoopPlayback = false);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="pDelegate">最後まで再生完了後に呼び出すコールバック関数を定義したオブジェクト</param>
	/// <returns>成否</returns>
	/// <remarks>C++から利用する場合用</remarks>
	bool Start(const TCHAR* pKey, ISoundsManagerDelegate* pDelegate);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="onPlayedToEndCallback">最後まで再生完了後に呼び出すコールバック関数</param>
	/// <returns>成否</returns>
	/// <remarks>C言語から利用する場合用</remarks>
	bool Start(const TCHAR* pKey, void(*onPlayedToEndCallback)(const TCHAR* pKey));

	/// <summary>
	/// 再生を停止する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>成否</returns>
	/// <remarks>後で続きから再生する必要がある場合は、<see cref="Pause(const TCHAR*)"/>を使用して下さい。</remarks>
	bool Stop(const TCHAR* pKey);

	/// <summary>
	/// 再生を一時停止する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>成否</returns>
	/// <remarks>続きから再生再開するときには<see cref="Resume(const TCHAR*)"/>を実行して下さい。</remarks>
	bool Pause(const TCHAR* pKey);

	/// <summary>
	/// 一時停止中の音声を続きから再生する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>成否</returns>
	/// <remarks><see cref="Pause(const TCHAR*)"/>で一時停止した音声の続きからの再生に使用します。</remarks>
	bool Resume(const TCHAR* pKey);

private:
	/* Variables ---------------------------------------------------------------------------------------- */
	std::unordered_map<TString, AudioHandler*> audioMap;
	IXAudio2* pXAudio2;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	SoundsManager(const SoundsManager&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	SoundsManager& operator=(const SoundsManager&) = delete;

	/* Functions ---------------------------------------------------------------------------------------- */
	bool ExistsKey(TString key) const;
};

}
#endif
