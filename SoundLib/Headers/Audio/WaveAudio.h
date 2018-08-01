//----------------------------------------------------------
// <filename>WaveAudio.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef WAVE_AUDIO_H
#define WAVE_AUDIO_H

#include "IAudio.h"


namespace SoundLib {
namespace Audio {
/// <summary>
/// WAVEファイル読み込みクラス
/// </summary>
class WaveAudio : public IAudio {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WaveAudio();

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	WaveAudio(WaveAudio&& obj) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WaveAudio();

	/* Getters / Setters -------------------------------------------------------------------------------- */
	/// <summary>
	/// PCMの情報を取得する。
	/// </summary>
	/// <returns>PCMの情報を格納したオブジェクト</returns>
	const WAVEFORMATEX* GetWaveFormatEx() const;

	/// <summary>
	/// ファイルフォーマットを示す文字列を取得する。
	/// </summary>
	/// <returns>ファイルフォーマット</returns>
	std::string GetFormatName() const;

	/// <summary>
	/// チャンネル数を取得する。
	/// </summary>
	/// <returns>チャンネル数</returns>
	int GetChannelCount() const;

	/// <summary>
	/// サンプリングレートを取得する。
	/// </summary>
	/// <returns>サンプリングレート</returns>
	int GetSamplingRate() const;

	/// <summary>
	/// ビットレートを取得する。
	/// </summary>
	/// <returns>ビットレート</returns>
	int GetBitsPerSample() const;

	/// <summary>
	/// ファイル末尾まで読み込みが終わったかどうかを示す値をを取得する。
	/// </summary>
	/// <returns>
	/// <list type="bullet">
	/// <item><c>true</c>:ファイルポインタがファイル末尾にある状態</item>
	/// <item><c>false</c>:ファイルポインタがファイル末尾にない状態</item>
	/// </list>
	/// </returns>
	bool HasReadToEnd() const;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	/// <summary>
	/// ムーブ代入演算子のオーバーロード
	/// </summary>
	/// <param name="obj">ムーブ対象おオブジェクト</param>
	/// <returns>ムーブ後のオブジェクト</returns>
	WaveAudio& operator=(WaveAudio&& obj) = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// ファイルを解析し、読み込み準備を行う。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>成否</returns>
	bool Load(std::string filePath);

	/// <summary>
	/// ファイルを順次読み込む。
	/// </summary>
	/// <param name="pBuffer">読み込んだデータを格納するバッファ</param>
	/// <param name="bufSize">バッファサイズ</param>
	/// <returns>格納データバイト数</returns>
	long Read(BYTE* pBuffer, long bufSize);

	/// <summary>
	/// ファイルポインタを音声データの先頭に戻す。
	/// </summary>
	void Reset();

private:
	/* Variables ---------------------------------------------------------------------------------------- */
	HMMIO hMmio;
	WAVEFORMATEX waveFormatEx;
	long offset;
	long dataSize;
	long restSize;
	bool hasReadToEnd;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	WaveAudio(const WaveAudio&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	WaveAudio& operator=(const WaveAudio&) = delete;
};

}
}
#endif
