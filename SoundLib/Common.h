//----------------------------------------------------------
// <filename>Common.h</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <tchar.h>


namespace SoundLib {
/// <summary>
/// ライブラリ内共通処理クラス
/// </summary>
class Common {
public:
	/* Functions ---------------------------------------------------------------------------------------- */
	/// <summary>
	/// <see cref="wchar_t"/>文字列を<see cref="char"/>に変換する。
	/// </summary>
	/// <param name="pSrc">変換元文字列へのポインタ</param>
	/// <returns><see cref="wchar_t"/>に変換した文字列へのポインタ</returns>
	/// <remarks>
	/// <para>メソッド内で必要なサイズのメモリ確保を行い、変換後の文字列を生成します。</para>
	/// <para>呼び出し元では不要になったタイミングで返却値のポインタのメモリ解放を行って下さい。</para>
	/// </remarks>
	static const char* ToChar(const wchar_t* pSrc);

	/// <summary>
	/// デバッグログを出力する。
	/// </summary>
	/// <param name="pStr">出力文字列</param>
	/// <param name="args">置換して差し込む値</param>
	template<class... Args>
	static void OutputDebugString(const char* pStr, Args... args) {
#ifdef _DEBUG
		char c[256];
		sprintf_s(c, pStr, args...);
		::OutputDebugStringA(c);
#endif
	}
};
}

#endif
