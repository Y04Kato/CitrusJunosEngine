#pragma once
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>
#include <wrl.h>
#include <set>
#include <convertString/ConvertString.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

struct ChunkHeader {
	char id[4];//チャンク毎のID
	int32_t size;//チャンクサイズ
};

struct RiffHeader {
	ChunkHeader chunk;//"RIFF"
	char type[4];//"WAVE"
};

struct FormatChunk {
	ChunkHeader chunk;//"fmt "
	WAVEFORMATEX fmt;//波形フォーマット
};

struct SoundData {
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;
};

//再生中の音声データ
struct PlaySoundData {
	SoundData* soundData = {};
	IXAudio2SourceVoice* sourceVoice = nullptr;
};

class Audio{
public:
	static Audio* GetInstance();

	void Initialize();

	//音声データの読み込み
	SoundData SoundLoadMp3(const char* filename);
	SoundData SoundLoadWave(const char* filename);

	//音声データ解放
	void SoundUnload(SoundData* soundData);

	//音声再生
	void SoundPlayWave(const SoundData& soundData, float AudioVolume, bool isLoop);

	//音声停止
	void SoundStopWave(SoundData* soundData);

	void Finalize();

	Audio(const Audio& obj) = delete;
	Audio& operator=(const Audio& obj) = delete;

private:
	Audio() = default;
	~Audio() = default;

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	std::set<PlaySoundData*> voices_;
};