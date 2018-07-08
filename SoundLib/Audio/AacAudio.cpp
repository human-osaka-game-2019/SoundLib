#include "AacAudio.h"
#include "../Common.h"
extern "C" {
#include <libavutil/opt.h>
}

namespace SoundLib {
namespace Audio {

AacAudio::AacAudio() : pFormatContext(nullptr), pAudioStream(nullptr), pCodecContext(nullptr), pSwr(nullptr), pPacket(nullptr) {}

AacAudio::~AacAudio() {
	avformat_close_input(&this->pFormatContext);
	avcodec_free_context(&this->pCodecContext);
	avformat_free_context(this->pFormatContext);
}

bool AacAudio::Load(const TCHAR* pFilePath) {
	char errDescription[500];
	int ret = avformat_open_input(&this->pFormatContext, pFilePath, nullptr, nullptr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx("cannot open file. filename=%s, ret=%08x description=%s\n", pFilePath, AVERROR(ret), errDescription);
		return false;
	}

	ret = avformat_find_stream_info(this->pFormatContext, nullptr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx("avformat_find_stream_info error. ret=%08x description=%s\n", AVERROR(ret), errDescription);
		return false;
	}
	av_dump_format(this->pFormatContext, 0, pFilePath, false);

	for (unsigned int i = 0; i < this->pFormatContext->nb_streams; ++i) {
		if (this->pFormatContext->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
			this->pAudioStream = this->pFormatContext->streams[i];
			break;
		}
	}
	if (this->pAudioStream == nullptr) {
		OutputDebugStringEx("stream not found. filename=%s\n", pFilePath);
		return false;
	}

	AVCodec* pCodec = avcodec_find_decoder(this->pAudioStream->codecpar->codec_id);
	if (pCodec == nullptr) {
		OutputDebugStringEx("avcodec_find_decoder codec not found. codec_id=%d\n", this->pAudioStream->codecpar->codec_id);
		return false;
	}

	this->pCodecContext = avcodec_alloc_context3(pCodec);
	if (this->pCodecContext == nullptr) {
		OutputDebugStringEx("avcodec_alloc_context3 error.\n");
		return false;
	}

	if (avcodec_parameters_to_context(this->pCodecContext, this->pAudioStream->codecpar) < 0) {
		return false;
	}

	ret = avcodec_open2(this->pCodecContext, pCodec, nullptr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx("avcodec_open2 error. ret=%08x description=%s\n", AVERROR(ret), errDescription);
		return false;
	}

	this->pSwr = swr_alloc();
	if (pSwr == nullptr) {
		OutputDebugStringEx("swr_alloc error.\n");
		return false;
	}
	av_opt_set_int(this->pSwr, "in_channel_layout", this->pAudioStream->codecpar->channel_layout, 0);
	av_opt_set_int(this->pSwr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(this->pSwr, "in_sample_rate", this->pAudioStream->codecpar->sample_rate, 0);
	av_opt_set_int(this->pSwr, "out_sample_rate", 44100, 0);
	av_opt_set_sample_fmt(this->pSwr, "in_sample_fmt", (AVSampleFormat)this->pAudioStream->codecpar->format, 0);
	av_opt_set_sample_fmt(this->pSwr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	ret = swr_init(this->pSwr);
	if (ret < 0) {
		av_strerror(ret, errDescription, 500);
		OutputDebugStringEx("swr_init error. ret=%08x description=%s\n", AVERROR(ret), errDescription);
		return false;
	}

	this->pPacket = av_packet_alloc();

	this->waveFormatEx = { 0 };
	this->waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	this->waveFormatEx.nChannels = 2;
	this->waveFormatEx.wBitsPerSample = 16;
	this->waveFormatEx.nSamplesPerSec = 44100;
	this->waveFormatEx.nBlockAlign = this->waveFormatEx.wBitsPerSample / 8 * this->waveFormatEx.nChannels;
	this->waveFormatEx.nAvgBytesPerSec = this->waveFormatEx.nSamplesPerSec * this->waveFormatEx.nBlockAlign;

	return true;
}

long AacAudio::Read(BYTE* pBuffer, DWORD bufSize) {
	DWORD bufRead = 0;	// バッファを読み込んだサイズ
	char errDescription[500];

	while (bufSize > bufRead) {
		int ret = av_read_frame(this->pFormatContext, this->pPacket);
		if (ret < 0) {
			if (ret == AVERROR_EOF) {
				return bufRead;
			} else {
				av_strerror(ret, errDescription, 500);
				OutputDebugStringEx("av_read_frame eerror. ret=%08x description=%s\n", AVERROR(ret), errDescription);
				return bufRead;
			}
		}

		if (this->pPacket->stream_index != this->pAudioStream->index) {
			av_packet_unref(this->pPacket);
			continue;
		}

		// decode ES
		if ((ret = avcodec_send_packet(this->pCodecContext, this->pPacket)) < 0) {
			av_strerror(ret, errDescription, 500);
			OutputDebugStringEx("avcodec_send_packet error. ret=%08x description=%s\n", AVERROR(ret), errDescription);
			return bufRead;
		}
		AVFrame* pFrame = av_frame_alloc();
		if ((ret = avcodec_receive_frame(this->pCodecContext, pFrame)) < 0) {
			if (ret != AVERROR(EAGAIN)) {
				av_strerror(ret, errDescription, 500);
				OutputDebugStringEx("avcodec_receive_frame error. ret=%08x description=%s\n", AVERROR(ret), errDescription);
				return bufRead;
			}
		} else {
			int convertableByteSize = pFrame->nb_samples * this->waveFormatEx.nChannels * (16 / 8);
			int remainingBufferSize = bufSize - bufRead;
			int convertSampleCount = (convertableByteSize > remainingBufferSize ? remainingBufferSize : convertableByteSize) / this->waveFormatEx.nChannels / (16 / 8);
			BYTE* pSwrBuf = new BYTE[convertableByteSize];
			ret = swr_convert(this->pSwr, &pSwrBuf, convertSampleCount, (const uint8_t**)pFrame->extended_data, pFrame->nb_samples);
			if (ret < 0) {
				av_strerror(ret, errDescription, 500);
				OutputDebugStringEx("swr_convert error. ret=%08x description=%s\n", AVERROR(ret), errDescription);
				return bufRead;
			}
			int readSize = ret * this->waveFormatEx.nChannels * (16 / 8);
			memcpy(pBuffer + bufRead, pSwrBuf, readSize);
			bufRead += readSize;

			delete[] pSwrBuf;
		}
		av_packet_unref(this->pPacket);
		av_frame_free(&pFrame);
	}
	
	return bufRead;
}

const WAVEFORMATEX* AacAudio::GetWaveFormatEx() {
	return &this->waveFormatEx;
}

void AacAudio::Reset() {
	// 読み込み位置を最初に戻す
	int result = av_seek_frame(this->pFormatContext, this->pAudioStream->index, 0, AVSEEK_FLAG_BACKWARD);
	if (result < 0) {
		char errDescription[500];
		av_strerror(result, errDescription, 500);
		OutputDebugStringEx("av_seek_frame error. ret=%08x description=%s\n", AVERROR(result), errDescription);
	}
	avcodec_flush_buffers(this->pCodecContext);
}

}
}
