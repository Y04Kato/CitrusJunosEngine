/**
 * @file Audio.cpp
 * @brief サウンド関係の初期化及び管理を行う
 * @author KATO
 * @date 未記録
 */

#include "Audio.h"
#include <cassert>

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}

void Audio::Initialize() {
	HRESULT result;

	//Xaudio2エンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);

	//Media Foundation
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

SoundData Audio::SoundLoad(const char* filename) {
	std::wstring path = ConvertString(filename);

	IMFSourceReader* pMFSourceReader{ nullptr };
	MFCreateSourceReaderFromURL(path.c_str(), NULL, &pMFSourceReader);

	IMFMediaType* pMFMediaType{ nullptr };
	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	WAVEFORMATEX* waveFormat{ nullptr };
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	std::vector<BYTE> mediaData;

	while (1) {

		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0, nullptr,
			&dwStreamFlags, nullptr,
			&pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();

	}

	SoundData soundData = {};

	std::unique_ptr<BYTE[]> pBuffer = std::make_unique<BYTE[]>(mediaData.size());
	memcpy(pBuffer.get(), mediaData.data(), mediaData.size());

	soundData.wfex = *waveFormat;
	soundData.pBuffer = std::move(pBuffer);
	soundData.bufferSize = static_cast<uint32_t>(mediaData.size());

	//CoTaskMemFree(waveFormat);
	pMFMediaType->Release();
	pMFSourceReader->Release();

	return soundData;
}

void Audio::SoundUnload(SoundData* soundData) {
	soundData->pBuffer.reset();
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::SoundPlayWave(const SoundData& soundData, float AudioVolume, bool isLoop) {
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_.Get()->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生中データ
	std::unique_ptr<PlaySoundData> voice = std::make_unique<PlaySoundData>();
	voice->soundData = const_cast<SoundData*>(&soundData);
	voice->sourceVoice = pSourceVoice;
	// 再生中データコンテナに登録
	voices_.insert(voice.get());

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer.get();
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//ループフラグがtrueならループさせる
	if (isLoop == true) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->SetVolume(AudioVolume);
	result = pSourceVoice->Start();

	//スマートポインタの所有権を手動で渡す
	voices_.insert(voice.release());
}

void Audio::SoundStopWave(SoundData* soundData){
	//再生中の音声リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](PlaySoundData* voice) { return voice->soundData == soundData; });
	//検索した音声データの停止
	if (it != voices_.end()) {
		(*it)->sourceVoice->DestroyVoice();

		voices_.erase(it);
	}
}


void Audio::Finalize() {
	xAudio2_.Reset();
	MFShutdown();
}