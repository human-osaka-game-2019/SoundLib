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
class AudioHandler : public IVoiceCallbackDelegate {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">音声データの識別に使用する名前</param>
	/// <param name="pAudio">オーディオファイルデコードクラスのインスタンス</param>
	AudioHandler(TString name, Audio::IAudio* pAudio);

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	AudioHandler(AudioHandler&& obj) = default;

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

	/* Operator Overloads ------------------------------------------------------------------------------- */
	/// <summary>
	/// ムーブ代入演算子のオーバーロード
	/// </summary>
	/// <param name="obj">ムーブ対象おオブジェクト</param>
	/// <returns>ムーブ後のオブジェクト</returns>
	AudioHandler& operator=(AudioHandler&& obj) = default;

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
	void Start(bool isLoopPlayback);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="pDelegate">最後まで再生完了後に呼び出すコールバック関数を定義したオブジェクト</param>
	/// <remarks>C++から設定する場合用</remarks>
	void Start(IAudioHandlerDelegate* pDelegate);

	/// <summary>
	/// ファイルの先頭から再生を行う。
	/// </summary>
	/// <param name="onPlayedToEndCallback">最後まで再生完了後に呼び出すコールバック関数</param>
	/// <remarks>C言語から設定する場合用</remarks>
	void Start(void(*onPlayedToEndCallback)(const TCHAR* pName));

	/// <summary>
	/// 再生を停止する。
	/// </summary>
	void Stop();

	/// <summary>
	/// 再生を一時停止する。
	/// </summary>
	void Pause();

	/// <summary>
	/// 一時停止中の音声を続きから再生する。
	/// </summary>
	void Resume();

	/// <summary>
	/// XAusio2再生用バッファが空になったタイミングで呼び出されるコールバック関数。
	/// </summary>
	void BufferEndCallback();

private:
	/* Constants ---------------------------------------------------------------------------------------- */
	static const int BUF_COUNT = 2;

	/* Variables ---------------------------------------------------------------------------------------- */
	TString name;
	Audio::IAudio * pAudio;
	IXAudio2SourceVoice* pVoice;
	VoiceCallback* pVoiceCallback;
	XAUDIO2_BUFFER xAudioBuffer;
	BYTE** pReadBuffers;
	int bufferSize;
	int currentBufNum;
	bool isLoopPlayback;
	IAudioHandlerDelegate* pDelegate;
	void(*onPlayedToEndCallback)(const TCHAR* pName);
	PlayingStatus status;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	AudioHandler(const AudioHandler&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	AudioHandler& operator=(const AudioHandler&) = delete;

	/* Functions ---------------------------------------------------------------------------------------- */
	void Push();
	void Start();
	void Stop(bool clearsCallback);
};
}

#endif
