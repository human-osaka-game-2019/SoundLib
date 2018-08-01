//----------------------------------------------------------
// <filename>CompressedAudio.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef COMPRESSED_AUDIO_H
#define COMPRESSED_AUDIO_H

#include "IAudio.h"


struct AVFormatContext;
struct AVStream;
struct AVCodec;
struct AVCodecContext;
struct SwrContext;
struct AVPacket;
struct AVFrame;

namespace SoundLib {
namespace Audio {

/// <summary>
/// 音声圧縮ファイルデコードクラス
/// </summary>
class CompressedAudio : public IAudio {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CompressedAudio();

	/// <summary>
	/// ムーブコンストラクタ
	/// </summary>
	/// <param name="obj">ムーブ対象オブジェクト</param>
	CompressedAudio(CompressedAudio&& obj) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CompressedAudio();

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
	CompressedAudio& operator=(CompressedAudio&& obj) = default;

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
	AVFormatContext* pFormatContext;
	AVStream* pAudioStream;
	AVCodec* pCodec;
	AVCodecContext* pCodecContext;
	SwrContext* pSwr;
	WAVEFORMATEX waveFormatEx;
	AVPacket* pPacket;
	AVFrame* pFrame;
	bool hasReadToEnd;
	BYTE* pRemainingConvertedBuf;
	int remainingConvertedBufSize;

	/* Constructor / Destructor ------------------------------------------------------------------------- */
	CompressedAudio(const CompressedAudio&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	CompressedAudio& operator=(const CompressedAudio&) = delete;

	/* Functions ---------------------------------------------------------------------------------------- */
	bool CreateCodecContext();
	long ConvertPcmFormat(BYTE* pBuffer, long bufSize);
};

}
}
#endif
