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


#define SOUND_LIB_LIGHT

namespace SoundLib {
/// <summary>
/// サウンド統括クラス
/// </summary>
template <typename T>
class SoundsManagerTmpl {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SoundsManagerTmpl();

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	SoundsManagerTmpl(SoundsManagerTmpl<T>&& obj) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SoundsManagerTmpl();

	/* Getters / Setters -------------------------------------------------------------------------------- */
	/// <summary>
	/// 再生状況を示すステータスを取得する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>再生ステータス</returns>
	PlayingStatus GetStatus(const T* pKey) const;

	/// <summary>
	/// ボリュームを取得する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>ボリューム(0:無音　100:音源ボリューム)</returns>
	uint8_t GetVolume(const T* pKey) const;

	/// <summary>
	/// ボリュームを設定する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="volume">ボリューム(0:無音　100:音源ボリューム)</param>
	/// <returns>成否</returns>
	bool SetVolume(const T* pKey, uint8_t volume);

	/// <summary>
	/// 再生速度とピッチの変化率を取得する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>音源からの変化率</returns>
	/// <remarks>
	/// 1.0の場合、音源から変化なし。
	/// 2.0の場合、再生速度2倍で1オクターブ高音。
	/// 0.5の場合、再生速度1/2で1オクターブ低音。
	/// </remarks>
	/// <seealso cref="IXAudio2SourceVoice::GetFrequencyRatio()"/>
	float GetFrequencyRatio(const T* pKey) const;

	/// <summary>
	/// 再生速度とピッチの変化率を設定する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="ratio">音源からの変化率</param>
	/// <returns>成否</returns>
	/// <remarks>
	/// <para>
	/// 1.0の場合、音源から変化なし。
	/// 2.0の場合、再生速度2倍で1オクターブ高音。
	/// 0.5の場合、再生速度1/2で1オクターブ低音。
	/// </para>
	/// <para>設定可能最大値は<see cref="AudioHandler::MAX_FREQENCY_RATIO"/>。</para>
	/// </remarks>
	/// <seealso cref="IXAudio2SourceVoice::SetFrequencyRatio(float, UINT32)"/>
	bool SetFrequencyRatio(const T* pKey, float ratio);

	/* Operator Overloads ------------------------------------------------------------------------------- */
	/// <summary>
	/// ムーブ代入演算子のオーバーロード
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	/// <returns>ムーブ後のオブジェクト</returns>
	SoundsManagerTmpl<T>& operator=(SoundsManagerTmpl<T>&& obj) = default;

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
	bool AddFile(const T* pFilePath, const T* pKey);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="isLoopPlayback">ループ再生を行うかどうか</param>
	/// <returns>成否</returns>
	bool Start(const T* pKey, bool isLoopPlayback = false);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="pDelegate">最後まで再生完了後に呼び出すコールバック関数を定義したオブジェクト</param>
	/// <returns>成否</returns>
	/// <remarks>C++から利用する場合用</remarks>
	bool Start(const T* pKey, ISoundsManagerDelegate<T>* pDelegate);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <param name="onPlayedToEndCallback">最後まで再生完了後に呼び出すコールバック関数</param>
	/// <returns>成否</returns>
	/// <remarks>C言語から利用する場合用</remarks>
	bool Start(const T* pKey, void(*onPlayedToEndCallback)(const T* pKey));

	/// <summary>
	/// 再生を停止する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>成否</returns>
	/// <remarks>後で続きから再生する必要がある場合は、<see cref="Pause(const T*)"/>を使用して下さい。</remarks>
	bool Stop(const T* pKey);

	/// <summary>
	/// 再生を一時停止する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>成否</returns>
	/// <remarks>続きから再生再開するときには<see cref="Resume(const T*)"/>を実行して下さい。</remarks>
	bool Pause(const T* pKey);

	/// <summary>
	/// 一時停止中の音声を続きから再生する。
	/// </summary>
	/// <param name="pKey">音声ファイルを識別するキー</param>
	/// <returns>成否</returns>
	/// <remarks><see cref="Pause(const T*)"/>で一時停止した音声の続きからの再生に使用します。</remarks>
	bool Resume(const T* pKey);

private:
	/* Variables ---------------------------------------------------------------------------------------- */
	std::unordered_map<std::basic_string<T>, AudioHandler<T>*> audioMap;
	IXAudio2* pXAudio2;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	SoundsManagerTmpl(const SoundsManagerTmpl<T>&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	SoundsManagerTmpl<T>& operator=(const SoundsManagerTmpl<T>&) = delete;

	/* Functions ---------------------------------------------------------------------------------------- */
	bool ExistsKey(const T* pKey, bool isErrWhenNotExists = true) const;
	bool JudgeAudio(const char* pFilePath, Audio::IAudio** ppAudio) const;
	void OutputAudioInfo(Audio::IAudio* pAudio, const char* pKey) const;
	bool AddToMap(Audio::IAudio* pAudio, const T* pKey);
	void OutputStrWithKey(const char* pStr, const T* pKey) const;
};

#ifdef UNICODE
using SoundsManager = SoundsManagerTmpl<wchar_t>;
#else
using SoundsManager = SoundsManagerTmpl<char>;
#endif
}
#endif
