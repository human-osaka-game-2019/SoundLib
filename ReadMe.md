# SoundLib

## 概要
C/C++Windowsゲーム用音声再生ライブラリ。  
指定されたファイルのストリーミング再生を行います。  
複数の音声の同時再生が可能です。

## 対応フォーマット
- WAV
- MP3
- [ffmpegで対応しているフォーマット](http://fixedpoint.jp/ffmpeg/general.html#SEC3)

## 環境構築
プロジェクトのプロパティを以下のように設定して下さい。
- includeフォルダをプロジェクトのインクルードディレクトリに追加。
- SoundLib.libを追加の依存ファイルに追加。
- コード生成のランタイムライブラリをマルチスレッド(/MT)、又はマルチスレッドデバッグ(/MTd)に設定。

SoundLib.libは32bit、64bitそれぞれに対応するDebug版とRelease版が存在します。  
構築するプロジェクトに合わせたものを使用して下さい。

## 使用方法
### 初期化
まず初期化を行った後に音声ファイルの登録を行います。  
複数ファイルの登録が可能です。
```C
SoundLib::SoundsManager soundsManager;

// 初期化
// SoundsManagerインスタンス生成後に1度のみ行う。
soundsManager.Initialize();

// 音声ファイルオープン
// 第2引数は音声ファイルを識別するための任意の文字列をキーとして指定する。
// この後の操作関数の呼び出し時には、ここで設定したキーを指定して音声を識別する。
const TCHAR* filePath = _T("music.wav");
bool isSuccess = soundsManager.AddFile(filePath, _T("bgm"));
```

### 再生・停止
```C
// 頭から再生
// 一時停止中の音声に対して当関数を実行した場合も頭からの再生となる。
// 第2引数にtrueを渡すとループ再生になる。
bool isSuccess = soundsManager.Start(_T("bgm"), true);

// 一時停止
bool isSuccess = soundsManager.Pause(_T("bgm"));

// 一時停止中の音声を続きから再生
bool isSuccess = soundsManager.Resume(_T("bgm"));

// 再生停止
bool isSuccess = soundsManager.Stop(_T("bgm"));
```

### 再生状態取得
以下の関数で再生状態を取得できます。
```C
SoundLib::PlayingStatus status = soundsManager.GetStatus(_T("bgm"));
```

再生状態は以下の列挙型で定義しています。
```C
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

### ボリューム
再生中でも設定可能です。
```C
//　ボリューム変更
// 0(無音)～100(原音量)の間で設定可能
soundsManager.SetVolume(_T("bgm"), 50);

// ボリューム取得
uint8_t volume = soundsManager.GetVolume(_T("bgm"))
```

### 再生速度・ピッチ変更
再生速度とピッチの変化率の設定ができます。  
再生中でも設定可能です。  

変化率は以下のように定義します。
- 1.0の場合、音源から変化なし。
- 2.0の場合、再生速度2倍で1オクターブ高音。
- 0.5の場合、再生速度1/2で1オクターブ低音。
```C
// 再生速度・ピッチ変更
bool isSuccess = soundsManager.SetFrequencyRatio(_T("bgm"), 0.5f);

// 再生速度・ピッチ取得
float ratio = soundsManager.GetFrequencyRatio(_T("bgm"))
```

### 再生終了イベント
再生終了時に呼び出すイベントハンドラを定義できます。  
```C
bool isSuccess = soundsManager.Start(_T("bgm"), OnPlayedToEnd);

void OnPlayedToEnd(const TCHAR* pKey) {
	// 再生終了後に行う処理
}
```

C++プログラムで使用する場合は、`ISoundsManagerDelegate`インターフェースを実装して処理を定義して下さい。
```CPP
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

再生終了イベントハンドラ内で曲再生を行うと、音が鳴らない等不具合が発生する可能性があります。  
再生終了イベント内での処理は、フラグを立てるなどの最小限の処理に留め、主処理はメインスレッドで行うようにして下さい。

## 使用ライブラリ
再生にはXAudio2を使用しています。  
mp3のデコードにはACMを使用しています。  
その他の音声ファイルのデコードには[ffmpeg4.0.2](https://www.ffmpeg.org/)を使用しています。
