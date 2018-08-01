//----------------------------------------------------------
// <filename>IAudio.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef IAUDIO_H
#define IAUDIO_H

#include <windows.h>
#include <string>
#include <mmsystem.h>
#include <mmreg.h>
#include "../Common.h"


namespace SoundLib {
namespace Audio {

/// <summary>
/// オーディオファイルデコードクラス用インターフェース
/// </summary>
class IAudio {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IAudio() = default;

	/* Getters / Setters -------------------------------------------------------------------------------- */
	/// <summary>
	/// PCMの情報を格納したオブジェクト
	/// </summary>
	/// <returns>PCMの情報を格納したオブジェクト</returns>
	virtual const WAVEFORMATEX* GetWaveFormatEx() const = 0;

	/// <summary>
	/// ファイルフォーマットを示す文字列を取得する。
	/// </summary>
	/// <returns>ファイルフォーマット</returns>
	virtual std::string GetFormatName() const = 0;

	/// <summary>
	/// デコード前のチャンネル数を取得する。
	/// </summary>
	/// <returns>チャンネル数</returns>
	virtual int GetChannelCount() const = 0;

	/// <summary>
	/// デコード前のサンプリングレートを取得する。
	/// </summary>
	/// <returns>サンプリングレート</returns>
	virtual int GetSamplingRate()const = 0;

	/// <summary>
	/// デコード前のビットレートを取得する。
	/// </summary>
	/// <returns>ビットレート</returns>
	virtual int GetBitsPerSample() const = 0;

	/// <summary>
	/// ファイル末尾まで読み込みが終わったかどうかを示す値をを取得する。
	/// </summary>
	/// <returns>
	/// <list type="bullet">
	/// <item><c>true</c>:ファイルポインタがファイル末尾にある状態</item>
	/// <item><c>false</c>:ファイルポインタがファイル末尾にない状態</item>
	/// </list>
	/// </returns>
	virtual bool HasReadToEnd() const = 0;

	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// ファイルを解析し、デコード準備を行う。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>成否</returns>
	virtual bool Load(std::string filePath) = 0;

	/// <summary>
	/// ファイルを順次読み込み、デコードした値を格納する。
	/// </summary>
	/// <param name="pBuffer">デコードしたデータを格納するバッファ</param>
	/// <param name="bufSize">バッファサイズ</param>
	/// <returns>格納データバイト数</returns>
	virtual long Read(BYTE* pBuffer, long bufSize) = 0;

	/// <summary>
	/// ファイルポインタを音声データの先頭に戻す。
	/// </summary>
	virtual void Reset() = 0;
};

}
}

#endif
