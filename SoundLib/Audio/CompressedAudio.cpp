#include "CompressedAudio.h"
extern "C" {
#include <libavutil/opt.h>
}
#include "../Common.h"


namespace SoundLib {
namespace Audio {

CompressedAudio::CompressedAudio() : pFormatContext(nullptr), 
									 pAudioStream(nullptr), 
									 pCodec(nullptr), 
									 pCodecContext(nullptr),
									 pSwr(nullptr), 
									 pPacket(nullptr), 
									 pFrame(nullptr), 
									 hasReadToEnd(false), 
									 pRemainingConvertedBuf(nullptr),
									 remainingConvertedBufSize(0) {}

CompressedAudio::~CompressedAudio() {
	if (this->remainingConvertedBufSize > 0) {
		delete[] this->pRemainingConvertedBuf;
		this->remainingConvertedBufSize = 0;
	}

	av_frame_free(&this->pFrame);
	av_packet_free(&this->pPacket);
	swr_free(&this->pSwr);
	avformat_close_input(&this->pFormatContext);
	avcodec_free_context(&this->pCodecContext);
	avformat_free_context(this->pFormatContext);
}


const WAVEFORMATEX* CompressedAudio::GetWaveFormatEx() const {
	return &this->waveFormatEx;
}

TString CompressedAudio::GetFormatName() const {
	return TString(this->pCodecContext->codec->long_name);
}

int CompressedAudio::GetChannelCount() const {
	return this->pCodecContext->channels;
}

int CompressedAudio::GetSamplingRate() const {
	return this->pCodecContext->sample_rate;
}

int CompressedAudio::GetBitsPerSample() const {
	return this->pCodecContext->bits_per_coded_sample;
}

bool CompressedAudio::HasReadToEnd() const {
	return this->hasReadToEnd;
}


bool CompressedAudio::Load(TString filePath) {
	char errDescription[500];
	int ret = avformat_open_input(&this->pFormatContext, filePath.c_str(), nullptr, nullptr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx(_T("cannot open file. filename=%s, ret=%08x description=%s\n"), filePath.c_str(), AVERROR(ret), errDescription);
		return false;
	}

	ret = avformat_find_stream_info(this->pFormatContext, nullptr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx(_T("avformat_find_stream_info error. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
		return false;
	}

	for (unsigned int i = 0; i < this->pFormatContext->nb_streams; ++i) {
		if (this->pFormatContext->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
			this->pAudioStream = this->pFormatContext->streams[i];
			break;
		}
	}
	if (this->pAudioStream == nullptr) {
		OutputDebugStringEx(_T("stream not found. filename=%s\n"), filePath.c_str());
		return false;
	}

	this->pCodec = avcodec_find_decoder(this->pAudioStream->codecpar->codec_id);
	if (this->pCodec == nullptr) {
		OutputDebugStringEx(_T("avcodec_find_decoder codec not found. codec_id=%d\n"), this->pAudioStream->codecpar->codec_id);
		return false;
	}

	if (!CreateCodecContext()) {
		return false;
	}

	this->pSwr = swr_alloc();
	if (pSwr == nullptr) {
		OutputDebugStringEx(_T("swr_alloc error.\n"));
		return false;
	}

	if (this->pCodecContext->channel_layout == 0) {
		this->pCodecContext->channel_layout = av_get_default_channel_layout(this->pCodecContext->channels);
	}
	av_opt_set_int(this->pSwr, "in_channel_layout", this->pCodecContext->channel_layout, 0);
	av_opt_set_int(this->pSwr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(this->pSwr, "in_sample_rate", this->pCodecContext->sample_rate, 0);
	av_opt_set_int(this->pSwr, "out_sample_rate", 44100, 0);
	av_opt_set_sample_fmt(this->pSwr, "in_sample_fmt", (AVSampleFormat)this->pAudioStream->codecpar->format, 0);
	av_opt_set_sample_fmt(this->pSwr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	ret = swr_init(this->pSwr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx(_T("swr_init error. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
		return false;
	}

	this->pPacket = av_packet_alloc();
	this->pFrame = av_frame_alloc();

	this->waveFormatEx = { 0 };
	this->waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	this->waveFormatEx.nChannels = 2;
	this->waveFormatEx.wBitsPerSample = 16;
	this->waveFormatEx.nSamplesPerSec = 44100;
	this->waveFormatEx.nBlockAlign = this->waveFormatEx.wBitsPerSample / 8 * this->waveFormatEx.nChannels;
	this->waveFormatEx.nAvgBytesPerSec = this->waveFormatEx.nSamplesPerSec * this->waveFormatEx.nBlockAlign;

	return true;
}

long CompressedAudio::Read(BYTE* pBuffer, DWORD bufSize) {
	DWORD bufRead = 0;	// バッファを読み込んだサイズ
	char errDescription[500];

	// 前回バッファに詰め切れなかった分をまず移送
	if (this->remainingConvertedBufSize > 0) {
		memcpy(pBuffer, this->pRemainingConvertedBuf, this->remainingConvertedBufSize);
		bufRead += this->remainingConvertedBufSize;
		this->remainingConvertedBufSize = 0;
		delete[] this->pRemainingConvertedBuf;
	}

	while (bufSize > bufRead && !this->hasReadToEnd) {
		int ret = av_read_frame(this->pFormatContext, this->pPacket);
		if (ret < 0) {
			if (ret == AVERROR_EOF) {
				// flush decoder
				if (ret = avcodec_send_packet(this->pCodecContext, nullptr) != 0) {
					av_strerror(ret, errDescription, 500);
					OutputDebugStringEx(_T("avcodec_send_packet error. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
				}
				this->hasReadToEnd = true;
			} else {
				av_strerror(ret, errDescription, 500);
				OutputDebugStringEx(_T("av_read_frame eerror. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
				break;
			}
		}

		if (this->pPacket->stream_index != this->pAudioStream->index) {
			av_packet_unref(this->pPacket);
			continue;
		}

		// decode ES
		if (!this->hasReadToEnd && (ret = avcodec_send_packet(this->pCodecContext, this->pPacket)) < 0) {
			av_strerror(ret, errDescription, 500);
			OutputDebugStringEx(_T("avcodec_send_packet error. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
			break;
		}

		do {
			ret = avcodec_receive_frame(this->pCodecContext, this->pFrame);
			if (ret < 0) {
				if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
					av_strerror(ret, errDescription, 500);
					OutputDebugStringEx(_T("avcodec_receive_frame error. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
				}
				break;
			}

			// fltp PCMデータをs16 PCMデータに変換
			int readSize = ConvertPcmFormat(pBuffer + bufRead, bufSize - bufRead);
			bufRead += readSize;
		} while (bufSize > bufRead);

		av_packet_unref(this->pPacket);
	}

	// avcodec_receive_frame()内でav_frame_unref()が呼ばれるので、av_frame_unref()はループを抜けてからのみの呼び出しでOK
	av_frame_unref(this->pFrame);

	av_packet_unref(this->pPacket);
	
	return bufRead;
}

void CompressedAudio::Reset() {
	// 読み込み位置を最初に戻す
	int result = av_seek_frame(this->pFormatContext, this->pAudioStream->index, 0, AVSEEK_FLAG_BACKWARD);
	if (result < 0) {
		char errDescription[500];
		av_strerror(result, errDescription, 500);
		OutputDebugStringEx(_T("av_seek_frame error. ret=%08x description=%s\n"), AVERROR(result), errDescription);
	}

	if (this->remainingConvertedBufSize > 0) {
		this->remainingConvertedBufSize = 0;
		delete this->pRemainingConvertedBuf;
	}

	// 下記のメソッドでコンテキストをクリアすることもできるが、作り直す方が安全という情報があるので作り直す。
	// avcodec_flush_buffers(this->pCodecContext);
	CreateCodecContext();

	this->hasReadToEnd = false;
}


bool CompressedAudio::CreateCodecContext() {
	char errDescription[500];

	avcodec_free_context(&this->pCodecContext);
	this->pCodecContext = avcodec_alloc_context3(this->pCodec);
	if (this->pCodecContext == nullptr) {
		OutputDebugStringEx(_T("avcodec_alloc_context3 error.\n"));
		return false;
	}

	int ret = avcodec_parameters_to_context(this->pCodecContext, this->pAudioStream->codecpar);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx(_T("avcodec_parameters_to_context error. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
		return false;
	}

	ret = avcodec_open2(this->pCodecContext, this->pCodec, nullptr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx(_T("avcodec_open2 error. ret=%08x description=%s\n"), AVERROR(ret), errDescription);
		return false;
	}

	return true;
}

int CompressedAudio::ConvertPcmFormat(BYTE* pBuffer, DWORD bufSize) {
	int copyableSize = 0;
	char errDescription[500];

	int convertableByteSize = this->pFrame->nb_samples * this->waveFormatEx.nChannels * (16 / 8);
	BYTE* pSwrBuf = new BYTE[convertableByteSize];
	int readCount = swr_convert(this->pSwr, &pSwrBuf, this->pFrame->nb_samples, (const uint8_t**)this->pFrame->extended_data, this->pFrame->nb_samples);
	if (readCount < 0) {
		av_strerror(readCount, errDescription, 500);
		OutputDebugStringEx(_T("swr_convert error. ret=%08x description=%s\n"), AVERROR(readCount), errDescription);
	} else {
		int readSize = readCount * this->waveFormatEx.nChannels * (16 / 8);
		copyableSize = bufSize > readSize ? readSize : bufSize;
		memcpy(pBuffer, pSwrBuf, copyableSize);

		if (copyableSize < readSize) {
			// バッファに詰め切れないデータを次回用に保持しておく
			this->remainingConvertedBufSize = readSize - copyableSize;
			this->pRemainingConvertedBuf = new BYTE[this->remainingConvertedBufSize];
			memcpy(this->pRemainingConvertedBuf, pSwrBuf + copyableSize, this->remainingConvertedBufSize);
		}
	}

	delete[] pSwrBuf;
	return copyableSize;
}

}
}
