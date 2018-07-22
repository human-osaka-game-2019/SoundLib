//----------------------------------------------------------
// <filename>CompressedAudio.cpp</filename>
// <author>Masami Sugao</author>
// <date>2018/07/16</date>
//----------------------------------------------------------
#include "CompressedAudio.h"
extern "C" {
#include <libavutil/opt.h>
}
#include "../Common.h"


namespace SoundLib {
namespace Audio {
/* Constructor / Destructor ------------------------------------------------------------------------- */
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
	delete[] this->pRemainingConvertedBuf;
	this->pRemainingConvertedBuf = nullptr;
	this->remainingConvertedBufSize = 0;

	av_frame_free(&this->pFrame);
	av_packet_free(&this->pPacket);
	swr_free(&this->pSwr);
	avformat_close_input(&this->pFormatContext);
	avcodec_free_context(&this->pCodecContext);
	avformat_free_context(this->pFormatContext);
}


/* Getters / Setters -------------------------------------------------------------------------------- */
const WAVEFORMATEX* CompressedAudio::GetWaveFormatEx() const {
	return &this->waveFormatEx;
}

std::string CompressedAudio::GetFormatName() const {
	return this->pCodecContext->codec->long_name;
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


/* Public Functions  -------------------------------------------------------------------------------- */
bool CompressedAudio::Load(std::string filePath) {
	char pErrDescription[500];
	int result = avformat_open_input(&this->pFormatContext, filePath.c_str(), nullptr, nullptr);
	if (result < 0) {
		av_strerror(result, pErrDescription, 500);
		OutputDebugStringEx("cannot open file. filename=%s, result=%08x description=%s\n", filePath.c_str(), AVERROR(result), pErrDescription);
		return false;
	}

	result = avformat_find_stream_info(this->pFormatContext, nullptr);
	if (result < 0) {
		av_strerror(result, pErrDescription, 500);
		OutputDebugStringEx("avformat_find_stream_info error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
		return false;
	}

	for (uint32_t i = 0; i < this->pFormatContext->nb_streams; ++i) {
		if (this->pFormatContext->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
			this->pAudioStream = this->pFormatContext->streams[i];
			break;
		}
	}
	if (this->pAudioStream == nullptr) {
		OutputDebugStringEx("stream not found. filename=%s\n", filePath.c_str());
		return false;
	}

	this->pCodec = avcodec_find_decoder(this->pAudioStream->codecpar->codec_id);
	if (this->pCodec == nullptr) {
		OutputDebugStringEx("avcodec_find_decoder codec not found. codec_id=%d\n", this->pAudioStream->codecpar->codec_id);
		return false;
	}

	if (!CreateCodecContext()) {
		return false;
	}

	this->pSwr = swr_alloc();
	if (pSwr == nullptr) {
		OutputDebugStringEx("swr_alloc error.\n");
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
	result = swr_init(this->pSwr);
	if (result < 0) {
		av_strerror(result, pErrDescription, 500);
		OutputDebugStringEx("swr_init error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
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

long CompressedAudio::Read(BYTE* pBuffer, long bufSize) {
	long filledLength = 0;
	char pErrDescription[500];

	// 前回バッファに詰め切れなかった分をまず移送
	if (this->remainingConvertedBufSize > 0) {
		memcpy(pBuffer, this->pRemainingConvertedBuf, this->remainingConvertedBufSize);
		filledLength += this->remainingConvertedBufSize;
		this->remainingConvertedBufSize = 0;
		delete[] this->pRemainingConvertedBuf;
	}

	while (bufSize > filledLength && !this->hasReadToEnd) {
		int result = av_read_frame(this->pFormatContext, this->pPacket);
		if (result < 0) {
			if (result == AVERROR_EOF) {
				// flush decoder
				if (result = avcodec_send_packet(this->pCodecContext, nullptr) != 0) {
					av_strerror(result, pErrDescription, 500);
					OutputDebugStringEx("avcodec_send_packet error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
				}
				this->hasReadToEnd = true;
			} else {
				av_strerror(result, pErrDescription, 500);
				OutputDebugStringEx("av_read_frame eerror. result=%08x description=%s\n", AVERROR(result), pErrDescription);
				break;
			}
		}

		if (this->pPacket->stream_index != this->pAudioStream->index) {
			av_packet_unref(this->pPacket);
			continue;
		}

		// decode ES
		if (!this->hasReadToEnd && (result = avcodec_send_packet(this->pCodecContext, this->pPacket)) < 0) {
			av_strerror(result, pErrDescription, 500);
			OutputDebugStringEx("avcodec_send_packet error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
			break;
		}

		do {
			result = avcodec_receive_frame(this->pCodecContext, this->pFrame);
			if (result < 0) {
				if (result != AVERROR(EAGAIN) && result != AVERROR_EOF) {
					av_strerror(result, pErrDescription, 500);
					OutputDebugStringEx("avcodec_receive_frame error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
				}
				break;
			}

			// fltp PCMデータをs16 PCMデータに変換
			long readSize = ConvertPcmFormat(pBuffer + filledLength, bufSize - filledLength);
			filledLength += readSize;
		} while (bufSize > filledLength);

		av_packet_unref(this->pPacket);
	}

	// avcodec_receive_frame()内でav_frame_unref()が呼ばれるので、av_frame_unref()はループを抜けてからのみの呼び出しでOK
	av_frame_unref(this->pFrame);

	av_packet_unref(this->pPacket);
	
	return filledLength;
}

void CompressedAudio::Reset() {
	// 読み込み位置を最初に戻す
	int result = av_seek_frame(this->pFormatContext, this->pAudioStream->index, 0, AVSEEK_FLAG_BACKWARD);
	if (result < 0) {
		char pErrDescription[500];
		av_strerror(result, pErrDescription, 500);
		OutputDebugStringEx("av_seek_frame error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
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


/* Private Functions  ------------------------------------------------------------------------------- */
bool CompressedAudio::CreateCodecContext() {
	char pErrDescription[500];

	avcodec_free_context(&this->pCodecContext);
	this->pCodecContext = avcodec_alloc_context3(this->pCodec);
	if (this->pCodecContext == nullptr) {
		OutputDebugStringEx("avcodec_alloc_context3 error.\n");
		return false;
	}

	int result = avcodec_parameters_to_context(this->pCodecContext, this->pAudioStream->codecpar);
	if (result < 0) {
		av_strerror(result, pErrDescription, 500);
		OutputDebugStringEx("avcodec_parameters_to_context error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
		return false;
	}

	result = avcodec_open2(this->pCodecContext, this->pCodec, nullptr);
	if (result < 0) {
		av_strerror(result, pErrDescription, 500);
		OutputDebugStringEx("avcodec_open2 error. result=%08x description=%s\n", AVERROR(result), pErrDescription);
		return false;
	}

	return true;
}

long CompressedAudio::ConvertPcmFormat(BYTE* pBuffer, long bufSize) {
	long copyableSize = 0;
	char pErrDescription[500];

	int convertableByteSize = this->pFrame->nb_samples * this->waveFormatEx.nChannels * (16 / 8);
	BYTE* pSwrBuf = new BYTE[convertableByteSize];
	int readCount = swr_convert(this->pSwr, &pSwrBuf, this->pFrame->nb_samples, (const uint8_t**)this->pFrame->extended_data, this->pFrame->nb_samples);
	if (readCount < 0) {
		av_strerror(readCount, pErrDescription, 500);
		OutputDebugStringEx("swr_convert error. result=%08x description=%s\n", AVERROR(readCount), pErrDescription);
	} else {
		long readSize = readCount * this->waveFormatEx.nChannels * (16 / 8);
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
