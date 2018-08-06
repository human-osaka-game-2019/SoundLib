//----------------------------------------------------------
// <filename>AudioHandler.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <xaudio2.h>
#include "IVoiceCallbackDelegate.h"
#include "Audio/IAudio.h"
#include "VoiceCallback.h"
#include "IAudioHandlerDelegate.h"


namespace SoundLib {
/* Enums -------------------------------------------------------------------------------------------- */
/// <summary>
/// 再生状況を示すステータス
/// </summary>
enum PlayingStatus {
	/// <summary>停止中</summary>
	Stopped,
	/// <summary>再生中</summary>
	Playing,
	/// <summary>一時停止中</summary>
	Pausing
};

/// <summary>
/// オーディオデータ操作クラス
/// </summary>
template <typename T>
class AudioHandler : public IVoiceCallbackDelegate {
public:
	/* Constants ---------------------------------------------------------------------------------------- */
	/// /// <summary><see cref="SetFrequencyRatio(float)"/>で設定可能な最大比率</summary>
	static const int MAX_FREQENCY_RATIO = 4;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">音声データの識別に使用する名前</param>
	/// <param name="pAudio">オーディオファイルデコードクラスのインスタンス</param>
	AudioHandler(std::basic_string<T> name, Audio::IAudio* pAudio);

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	AudioHandler(AudioHandler<T>&& obj) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~AudioHandler();

	/* Getters / Setters -------------------------------------------------------------------------------- */
	/// <summary>
	/// 再生状況を示すステータスを取得する。
	/// </summary>
	/// <returns>再生ステータス</returns>
	PlayingStatus GetStatus() const;

	/// <summary>
	/// ボリュームを取得する。
	/// </summary>
	/// <returns>ボリューム(XAudio2の定義に従う)</returns>
	float GetVolume() const;

	/// <summary>
	/// ボリュームを設定する。
	/// </summary>
	/// <param name="volume">ボリューム(XAudio2の定義に従う)</param>
	/// <returns>成否</returns>
	bool SetVolume(float volume);

	/// <summary>
	/// 再生速度とピッチの変化率を取得する。
	/// </summary>
	/// <returns>音源からの変化率</returns>
	/// <remarks>
	/// 1.0の場合、音源から変化なし。
	/// 2.0の場合、再生速度2倍で1オクターブ高音。
	/// 0.5の場合、再生速度1/2で1オクターブ低音。
	/// </remarks>
	/// <seealso cref="IXAudio2SourceVoice::GetFrequencyRatio()"/>
	float GetFrequencyRatio() const;

	/// <summary>
	/// 再生速度とピッチの変化率を設定する。
	/// </summary>
	/// <param name="ratio">音源からの変化率</param>
	/// <returns>成否</returns>
	/// <remarks>
	/// <para>
	/// 1.0の場合、音源から変化なし。
	/// 2.0の場合、再生速度2倍で1オクターブ高音。
	/// 0.5の場合、再生速度1/2で1オクターブ低音。
	/// </para>
	/// <para>設定可能最大値は<see cref="MAX_FREQENCY_RATIO"/>。</para>
	/// </remarks>
	/// <seealso cref="IXAudio2SourceVoice::SetFrequencyRatio(float, UINT32)"/>
	bool SetFrequencyRatio(float ratio);

	/* Operator Overloads ------------------------------------------------------------------------------- */
	/// <summary>
	/// ムーブ代入演算子のオーバーロード
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	/// <returns>ムーブ後のオブジェクト</returns>
	AudioHandler<T>& operator=(AudioHandler<T>&& obj) = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// XAudio2のソースボイスを生成する。
	/// </summary>
	/// <param name="rXAudio2">XAudio2オブジェクト</param>
	/// <returns>成否</returns>
	bool Prepare(IXAudio2& rXAudio2);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="isLoopPlayback">ループ再生を行うかどうか</param>
	/// <returns>成否</returns>
	bool Start(bool isLoopPlayback);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pDelegate">最後まで再生完了後に呼び出すコールバック関数を定義したオブジェクト</param>
	/// <returns>成否</returns>
	/// <remarks>C++から設定する場合用</remarks>
	bool Start(IAudioHandlerDelegate<T>* pDelegate);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="onPlayedToEndCallback">最後まで再生完了後に呼び出すコールバック関数</param>
	/// <returns>成否</returns>
	/// <remarks>C言語から設定する場合用</remarks>
	bool Start(void(*onPlayedToEndCallback)(const T* pName));

	/// <summary>
	/// 再生を停止する。
	/// </summary>
	void Stop();

	/// <summary>
	/// 再生を一時停止する。
	/// </summary>
	/// <returns>成否</returns>
	bool Pause();

	/// <summary>
	/// 一時停止中の音声を続きから再生する。
	/// </summary>
	/// <returns>成否</returns>
	bool Resume();

	/// <summary>
	/// XAusio2再生用バッファが空になったタイミングで呼び出されるコールバック関数。
	/// </summary>
	void OnBufferEnd();

	/// <summary>
	/// XAudio2再生用バッファの再生がすべて完了したタイミングで呼び出されるコールバック関数。
	/// </summary>
	void OnStreamEnd();

private:
	/* Constants ---------------------------------------------------------------------------------------- */
	static const int BUF_COUNT = 2;

	/* Variables ---------------------------------------------------------------------------------------- */
	std::basic_string<T> name;
	Audio::IAudio* pAudio;
	IXAudio2SourceVoice* pVoice;
	VoiceCallback* pVoiceCallback;
	XAUDIO2_BUFFER xAudioBuffer;
	BYTE** pReadBuffers;
	int bufferSize;
	int currentBufNum;
	bool isLoopPlayback;
	IAudioHandlerDelegate<T>* pDelegate;
	void(*onPlayedToEndCallback)(const T* pName);
	PlayingStatus status;
	bool isRequiredToStop;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	AudioHandler(const AudioHandler<T>&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	AudioHandler<T>& operator=(const AudioHandler<T>&) = delete;

	/* Functions ---------------------------------------------------------------------------------------- */
	void Push();
	bool Start();
	bool Stop(bool clearsCallback);
	void ExecutePlayedToEndCallback();
};
}

#endif
