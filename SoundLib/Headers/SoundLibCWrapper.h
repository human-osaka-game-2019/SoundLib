//----------------------------------------------------------
// <filename>SoundsLibCWrapper.h</filename>
// <author>Masami Sugao</author>
// <date>2018/08/09</date>
//----------------------------------------------------------
#ifndef SOUNDS_LIB_C_WRAPPER_H
#define SOUNDS_LIB_C_WRAPPER_H

#include <stdint.h>
#include "SoundsManager.h"


/* Enums -------------------------------------------------------------------------------------------- */
/// <summary>
/// 再生状況を示すステータス
/// </summary>
enum SoundPlayingStatus {
	/// <summary>停止中</summary>
	SoundPlayingStatus_Stopped,
	/// <summary>再生中</summary>
	SoundPlayingStatus_Playing,
	/// <summary>一時停止中</summary>
	SoundPlayingStatus_Pausing
};

/* Functions ---------------------------------------------------------------------------------------- */
/// <summary>
/// XAudio2の初期化処理を行う。
/// </summary>
/// <returns>成否</returns>
/// <remarks>ライブラリ使用時はまず当メソッドを呼び出して下さい。</remarks>
bool SoundLibCWrapper_InitializeA();

/// <summary>
/// XAudio2の初期化処理を行う。
/// </summary>
/// <returns>成否</returns>
/// <remarks>ライブラリ使用時はまず当メソッドを呼び出して下さい。</remarks>
bool SoundLibCWrapper_InitializeW();

#ifdef UNICODE
#define SoundLibCWrapper_Initialize SoundLibCWrapper_InitializeW
#else
#define SoundLibCWrapper_Initialize SoundLibCWrapper_InitializeA
#endif


/// <summary>
/// 解放処理
/// </summary>
/// <remarks><c>SoundLibCWrapper_Initialize</c>を呼び出した後は、必ず当関数を呼び出して解放処理を行って下さい。</remarks>
void SoundLibCWrapper_Free();


/// <summary>
/// 再生状況を示すステータスを取得する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>再生ステータス</returns>
enum SoundPlayingStatus SoundLibCWrapper_GetStatusA(const char* pKey);

/// <summary>
/// 再生状況を示すステータスを取得する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>再生ステータス</returns>
enum SoundPlayingStatus SoundLibCWrapper_GetStatusW(const wchar_t* pKey);

#ifdef UNICODE
#define SoundLibCWrapper_GetStatus SoundLibCWrapper_GetStatusW
#else
#define SoundLibCWrapper_GetStatus SoundLibCWrapper_GetStatusA
#endif


/// <summary>
/// ボリュームを取得する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>ボリューム(0:無音　100:音源ボリューム)</returns>
uint8_t SoundLibCWrapper_GetVolumeA(const char* pKey);

/// <summary>
/// ボリュームを取得する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>ボリューム(0:無音　100:音源ボリューム)</returns>
uint8_t SoundLibCWrapper_GetVolumeW(const wchar_t* pKey);

#ifdef UNICODE
#define SoundLibCWrapper_GetVolume SoundLibCWrapper_GetVolumeW
#else
#define SoundLibCWrapper_GetVolume SoundLibCWrapper_GetVolumeA
#endif


/// <summary>
/// ボリュームを設定する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <param name="volume">ボリューム(0:無音　100:音源ボリューム)</param>
/// <returns>成否</returns>
bool SoundLibCWrapper_SetVolumeA(const char* pKey, uint8_t volume);

/// <summary>
/// ボリュームを設定する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <param name="volume">ボリューム(0:無音　100:音源ボリューム)</param>
/// <returns>成否</returns>
bool SoundLibCWrapper_SetVolumeW(const wchar_t* pKey, uint8_t volume);

#ifdef UNICODE
#define SoundLibCWrapper_SetVolume SoundLibCWrapper_SetVolumeW
#else
#define SoundLibCWrapper_SetVolume SoundLibCWrapper_SetVolumeA
#endif


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
float SoundLibCWrapper_GetFrequencyRatioA(const char* pKey);

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
float SoundLibCWrapper_GetFrequencyRatioW(const wchar_t* pKey);

#ifdef UNICODE
#define SoundLibCWrapper_GetFrequencyRatio SoundLibCWrapper_GetFrequencyRatioW
#else
#define SoundLibCWrapper_GetFrequencyRatio SoundLibCWrapper_GetFrequencyRatioA
#endif


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
bool SoundLibCWrapper_SetFrequencyRatioA(const char* pKey, float ratio);

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
bool SoundLibCWrapper_SetFrequencyRatioW(const wchar_t* pKey, float ratio);

#ifdef UNICODE
#define SoundLibCWrapper_SetFrequencyRatio SoundLibCWrapper_SetFrequencyRatioW
#else
#define SoundLibCWrapper_SetFrequencyRatio SoundLibCWrapper_SetFrequencyRatioA
#endif


/// <summary>
/// 音声ファイルを追加する。
/// </summary>
/// <param name="pFilePath">ファイルパス</param>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
bool SoundLibCWrapper_AddFileA(const char* pFilePath, const char* pKey);

/// <summary>
/// 音声ファイルを追加する。
/// </summary>
/// <param name="pFilePath">ファイルパス</param>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
bool SoundLibCWrapper_AddFileW(const wchar_t* pFilePath, const wchar_t* pKey);

#ifdef UNICODE
#define SoundLibCWrapper_AddFile SoundLibCWrapper_AddFileW
#else
#define SoundLibCWrapper_AddFile SoundLibCWrapper_AddFileA
#endif


/// <summary>
/// ファイルの先頭から再生を行う。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <param name="isLoopPlayback">ループ再生を行うかどうか</param>
/// <returns>成否</returns>
bool SoundLibCWrapper_StartA(const char* pKey, bool isLoopPlayback = false);

/// <summary>
/// ファイルの先頭から再生を行う。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <param name="isLoopPlayback">ループ再生を行うかどうか</param>
/// <returns>成否</returns>
bool SoundLibCWrapper_StartW(const wchar_t* pKey, bool isLoopPlayback = false);

/// <summary>
/// ファイルの先頭から再生を行う。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <param name="onPlayedToEndCallback">最後まで再生完了後に呼び出すコールバック関数</param>
/// <returns>成否</returns>
/// <remarks>C言語から利用する場合用</remarks>
bool SoundLibCWrapper_StartA(const char* pKey, void(*onPlayedToEndCallback)(const char* pKey));

/// <summary>
/// ファイルの先頭から再生を行う。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <param name="onPlayedToEndCallback">最後まで再生完了後に呼び出すコールバック関数</param>
/// <returns>成否</returns>
/// <remarks>C言語から利用する場合用</remarks>
bool SoundLibCWrapper_StartW(const wchar_t* pKey, void(*onPlayedToEndCallback)(const wchar_t* pKey));

#ifdef UNICODE
#define SoundLibCWrapper_Start SoundLibCWrapper_StartW
#else
#define SoundLibCWrapper_Start SoundLibCWrapper_StartA
#endif


/// <summary>
/// 再生を停止する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
/// <remarks>後で続きから再生する必要がある場合は、<see cref="Pause(const T*)"/>を使用して下さい。</remarks>
bool SoundLibCWrapper_StopA(const char* pKey);

/// <summary>
/// 再生を停止する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
/// <remarks>後で続きから再生する必要がある場合は、<see cref="Pause(const T*)"/>を使用して下さい。</remarks>
bool SoundLibCWrapper_StopW(const wchar_t* pKey);

#ifdef UNICODE
#define SoundLibCWrapper_Stop SoundLibCWrapper_StopW
#else
#define SoundLibCWrapper_Stop SoundLibCWrapper_StopA
#endif


/// <summary>
/// 再生を一時停止する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
/// <remarks>続きから再生再開するときには<see cref="Resume(const T*)"/>を実行して下さい。</remarks>
bool SoundLibCWrapper_PauseA(const char* pKey);

/// <summary>
/// 再生を一時停止する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
/// <remarks>続きから再生再開するときには<see cref="Resume(const T*)"/>を実行して下さい。</remarks>
bool SoundLibCWrapper_PauseW(const wchar_t* pKey);

#ifdef UNICODE
#define SoundLibCWrapper_Pause SoundLibCWrapper_PauseW
#else
#define SoundLibCWrapper_Pause SoundLibCWrapper_PauseA
#endif


/// <summary>
/// 一時停止中の音声を続きから再生する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
/// <remarks><see cref="Pause(const T*)"/>で一時停止した音声の続きからの再生に使用します。</remarks>
bool SoundLibCWrapper_ResumeA(const char* pKey);

/// <summary>
/// 一時停止中の音声を続きから再生する。
/// </summary>
/// <param name="pKey">音声ファイルを識別するキー</param>
/// <returns>成否</returns>
/// <remarks><see cref="Pause(const T*)"/>で一時停止した音声の続きからの再生に使用します。</remarks>
bool SoundLibCWrapper_ResumeW(const wchar_t* pKey);

#ifdef UNICODE
#define SoundLibCWrapper_Resume SoundLibCWrapper_ResumeW
#else
#define SoundLibCWrapper_Resume SoundLibCWrapper_ResumeA
#endif

#endif
