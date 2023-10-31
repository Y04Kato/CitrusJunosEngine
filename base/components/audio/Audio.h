#pragma once
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>
#include <wrl.h>

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

class Audio{
public:
	static Audio* GetInstance();

	void Initialize();

	//音声データの読み込み
	SoundData SoundLoadWave(const char* filename);

	//音声データ解放
	void SoundUnload(SoundData* soundData);

	//音声再生
	void SoundPlayWave(const SoundData& soundData, float AudioVolume, bool isLoop);

	void Finalize();

	Audio(const Audio& obj) = delete;
	Audio& operator=(const Audio& obj) = delete;

private:
	Audio() = default;
	~Audio() = default;

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
};