//----------------------------------------------------------
// <filename>Mp3Audio.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef MP3_AUDIO_H
#define MP3_AUDIO_H

#include "IAudio.h"
#include <mmreg.h>
#include <msacm.h>


namespace SoundLib {
namespace Audio {
/// <summary>
/// mp3ファイルデコードクラス
/// </summary>
class Mp3Audio : public IAudio {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Mp3Audio();

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	Mp3Audio(Mp3Audio&& obj) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Mp3Audio();

	/* Getters / Setters -------------------------------------------------------------------------------- */
	/// <summary>
	/// PCMの情報を格納したオブジェクト
	/// </summary>
	/// <returns>PCMの情報を格納したオブジェクト</returns>
	const WAVEFORMATEX* GetWaveFormatEx() const;

	/// <summary>
	/// ファイルフォーマットを示す文字列を取得する。
	/// </summary>
	/// <returns>ファイルフォーマット</returns>
	std::string GetFormatName() const;

	/// <summary>
	/// デコード前のチャンネル数を取得する。
	/// </summary>
	/// <returns>チャンネル数</returns>
	int GetChannelCount() const;

	/// <summary>
	/// デコード前のサンプリングレートを取得する。
	/// </summary>
	/// <returns>サンプリングレート</returns>
	int GetSamplingRate() const;

	/// <summary>
	/// デコード前のビットレートを取得する。
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
	Mp3Audio& operator=(Mp3Audio&& obj) = default;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// ファイルを解析し、デコード準備を行う。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>成否</returns>
	bool Load(std::string filePath);

	/// <summary>
	/// ファイルを順次読み込み、デコードした値を格納する。
	/// </summary>
	/// <param name="pBuffer">デコードしたデータを格納するバッファ</param>
	/// <param name="bufSize">バッファサイズ</param>
	/// <returns>格納データバイト数</returns>
	long Read(BYTE* pBuffer, long bufSize);

	/// <summary>
	/// ファイルポインタを音声データの先頭に戻す。
	/// </summary>
	void Reset();

private:
	/* Variables ---------------------------------------------------------------------------------------- */
	HANDLE hFile;
	DWORD offset; // MP3データの位置
	DWORD mp3DataSize; // MP3データのサイズ
	int channelCount;
	int bitsPerSample;
	WORD sampleRate;
	WAVEFORMATEX waveFormatEx;
	ACMSTREAMHEADER ash;
	HACMSTREAM has;
	bool hasReadToEnd;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	Mp3Audio(const Mp3Audio&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	Mp3Audio& operator=(const Mp3Audio&) = delete;

	/* Functions ---------------------------------------------------------------------------------------- */
	DWORD GetDataSize();
	WORD GetBitRate(BYTE* pHeader, int version) const;
	WORD GetSampleRate(BYTE* pHeader, int version) const;
};

}
}
#endif
