# SoundLib

## 概要
C/C++Windowsゲーム用音声再生ライブラリ。  
指定されたファイルのストリーミング再生を行います。  
複数の音声の同時再生が可能です。

## 対応フォーマット
FULL版はffmpegを組み込んでいるので多くのフォーマットに対応していますが、その分libのサイズが大きく、メモリ使用量も多いです。  
wavとmp3のみの再生を行う場合はLIGHT版がおすすめです。

### FULL版
- WAV
- MP3
- [ffmpegで対応しているフォーマット](http://fixedpoint.jp/ffmpeg/general.html#SEC3)
### LIGHT版
- WAV
- MP3

## 環境構築
プロジェクトのプロパティを以下のように設定して下さい。
- includeフォルダをプロジェクトのインクルードディレクトリに追加。
- SoundLib.libを追加の依存ファイルに追加。
- コード生成のランタイムライブラリを、Debug版の場合はマルチスレッドデバッグ(/MTd)、Release版の場合はマルチスレッド(/MT)に設定。

SoundLib.libは32bit、64bitそれぞれに対応するDebug版とRelease版が存在します。  
構築するプロジェクトに合わせたものを使用して下さい。

## 使用方法
### Include
#### C++
SoundsManager.hを`#include`に設定して下さい。 
```cpp
#include <SoundsManager.h>
``` 

#### C言語
Cプログラムから呼び出す場合は、ラッパーとなるSoundLibCWrapper.hを`#include`に追加して下さい。
```C
#include <SoundLibCWrapper.h>
```

### 初期化
まず初期化を行った後に音声ファイルの登録を行います。  
複数ファイルの登録が可能です。
#### C++
```Cpp
SoundLib::SoundsManager soundsManager;

// 初期化
// SoundsManagerインスタンス生成後に1度のみ行う。
bool isSuccess = soundsManager.Initialize();

// 音声ファイルオープン
// 第2引数は音声ファイルを識別するための任意の文字列をキーとして指定する。
// この後の操作関数の呼び出し時には、ここで設定したキーを指定して音声を識別する。
const TCHAR* filePath = _T("music.wav");
isSuccess = soundsManager.AddFile(filePath, _T("bgm"));
```

#### C言語
```C
// 初期化
// 最初に1度のみ行う。
bool isSuccess = SoundLibCWrapper_Initialize();

// 音声ファイルオープン
// 第2引数は音声ファイルを識別するための任意の文字列をキーとして指定する。
// この後の操作関数の呼び出し時には、ここで設定したキーを指定して音声を識別する。
const TCHAR* filePath = _T("music.wav");
isSuccess = SoundLibCWrapper_AddFile(filePath, _T("bgm"));
```

`SoundLibCWrapper_Initialize()`を呼び出して初期化を行った場合、最後に必ず解放処理を行って下さい。
```C
// 解放
SoundLibCWrapper_Free();
```



### 再生・停止
#### C++
```Cpp
// 頭から再生
// 一時停止中の音声に対して当関数を実行した場合も頭からの再生となる。
// 第2引数にtrueを渡すとループ再生になる。
bool isSuccess = soundsManager.Start(_T("bgm"), true);

// 一時停止
isSuccess = soundsManager.Pause(_T("bgm"));

// 一時停止中の音声を続きから再生
isSuccess = soundsManager.Resume(_T("bgm"));

// 再生停止
isSuccess = soundsManager.Stop(_T("bgm"));
```

#### C言語
```C
// 頭から再生
// 一時停止中の音声に対して当関数を実行した場合も頭からの再生となる。
// 第2引数にfalseを渡すと1曲分再生する。
bool isSuccess = SoundLibCWrapper_Start(_T("bgm"), false);

// 一時停止
isSuccess = SoundLibCWrapper_Pause(_T("bgm"));

// 一時停止中の音声を続きから再生
isSuccess = SoundLibCWrapper_Resume(_T("bgm"));

// 再生停止
isSuccess = SoundLibCWrapper_Stop(_T("bgm"));

// SoundLibCWrapper_Start()の第2引数にtrueを渡すとループ再生になる。
isSuccess = SoundLibCWrapper_Start(_T("bgm"), true);
```

### 再生状態取得
#### C++
以下の関数で再生状態を取得できます。
```Cpp
SoundLib::PlayingStatus status = soundsManager.GetStatus(_T("bgm"));
```

再生状態は以下の列挙型で定義しています。
```Cpp
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
```

#### C言語
以下の関数で再生状態を取得できます。
```C
enum SoundPlayingStatus status = SoundLibCWrapper_GetStatus(_T("bgm"));
```

再生状態は以下の列挙型で定義しています。
```C
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
```

### ボリューム
再生中でも設定可能です。
#### C++
```Cpp
//　ボリューム変更
// 0(無音)～100(原音量)の間で設定可能
soundsManager.SetVolume(_T("bgm"), 50);

// ボリューム取得
uint8_t volume = soundsManager.GetVolume(_T("bgm"))
```

#### C言語
```C
//　ボリューム変更
// 0(無音)～100(原音量)の間で設定可能
SoundLibCWrapper_SetVolume(_T("bgm"), 50);

// ボリューム取得
uint8_t volume = SoundLibCWrapper_GetVolume(_T("bgm"))
```

### 再生速度・ピッチ変更
再生速度とピッチの変化率の設定ができます。  
再生中でも設定可能です。  

変化率は以下のように定義します。
- 1.0の場合、音源から変化なし。
- 2.0の場合、再生速度2倍で1オクターブ高音。
- 0.5の場合、再生速度1/2で1オクターブ低音。

最大値は4.0です。

#### C++
```Cpp
// 再生速度・ピッチ変更
bool isSuccess = soundsManager.SetFrequencyRatio(_T("bgm"), 0.5f);

// 再生速度・ピッチ取得
float ratio = soundsManager.GetFrequencyRatio(_T("bgm"))
```

#### C言語
```C
// 再生速度・ピッチ変更
bool isSuccess = SoundLibCWrapper_SetFrequencyRatio(_T("bgm"), 0.5f);

// 再生速度・ピッチ取得
float ratio = SoundLibCWrapper_GetFrequencyRatio(_T("bgm"))
```

### 再生終了イベント
再生終了時に呼び出すイベントハンドラを定義できます。  

再生終了イベントハンドラ内で曲再生を行うと、音が鳴らない等不具合が発生する可能性があります。  
再生終了イベント内での処理は、フラグを立てるなどの最小限の処理に留め、主処理はメインスレッドで行うようにして下さい。

#### C++
C++プログラムで使用する場合は、`ISoundsManagerDelegate`インターフェースを実装して処理を定義して下さい。
```Cpp
class Foo : public ISoundsManagerDelegate {
public:
	void Play() {
		bool isSuccess = soundsManager.Start(_T("bgm"), this);
	}

	void OnPlayedToEnd(std::basic_string<TCHAR> key) {
		// 再生終了後に行う処理
	}
}
```

#### C言語
Cプログラムで使用する場合は、関数ポインタを第2引数に設定して下さい。
```C
void foo() {
	bool isSuccess = SoundLibCWrapper_Start(_T("bgm"), OnPlayedToEnd);

	void OnPlayedToEnd(const TCHAR* pKey) {
		// 再生終了後に行う処理
	}
}
```

### 同一サウンドの多重再生
同一サウンドを多重再生する場合、同じファイルを別のキーで複数回登録して使用して下さい。
#### C++
```Cpp
const TCHAR* filePath = _T("music.wav");
// 1つ目の登録
bool isSuccess = soundsManager.AddFile(filePath, _T("bgm1"));
// 同じファイルを異なるキーで登録
isSuccess = soundsManager.AddFile(filePath, _T("bgm2"));

// 1つ目の再生
isSuccess = soundsManager.Start(_T("bgm1"));
// 2つ目を重ねて再生
isSuccess = soundsManager.Start(_T("bgm2"));
```

#### C言語
```C
const TCHAR* filePath = _T("music.wav");
// 1つ目の登録
bool isSuccess = SoundLibCWrapper_AddFile(filePath, _T("bgm1"));
// 同じファイルを異なるキーで登録
isSuccess = SoundLibCWrapper_AddFile(filePath, _T("bgm2"));

// 1つ目の再生
isSuccess = SoundLibCWrapper_Start(_T("bgm1"), false);
// 2つ目を重ねて再生
isSuccess = SoundLibCWrapper_Start(_T("bgm2"), false);
```


## 使用ライブラリ
再生にはXAudio2を使用しています。  
mp3のデコードにはACMを使用しています。  
その他の音声ファイルのデコードには[ffmpeg4.0.2](https://www.ffmpeg.org/)を使用しています。
