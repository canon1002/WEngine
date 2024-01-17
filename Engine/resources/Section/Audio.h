#pragma once
#include <xaudio2.h>
#include <fstream>
#include "../../base/DirectXCommon.h"
#pragma comment(lib,"xaudio2.lib")



// チャンクヘッダ
struct ChunkHeader
{
	char id[4];
	int32_t size;
};

// RIFFヘッダチャンク
struct RiffHeader
{
	ChunkHeader chunk;
	char type[4];
};

//FMTチャンク
struct FormatChunk
{
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

// 音声データ
struct SoundData
{
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;

};

class Audio
{
private:
	Audio();
	~Audio();

public: // ** 静的メンバ関数 ** //

	// コピーコンストラクタと演算子オーバーロードの禁止
	Audio(const Audio& obj) = delete;
	Audio& operator=(const Audio& obj) = delete;

	// インスタンスを取得


	static Audio* GetWaveInstance() {
		// 関数内staticは初めて通ったときのみ実行される
		if (instance == nullptr) {
			instance = new Audio;
		}
		return instance;
	}

	// 開放する
	static void Finalize();


	void Init();

	SoundData LoadWave(const char* filename);
	void UnLoadWave(SoundData* soundData);

	void PlayWave(const SoundData& soundData);
	void StopWave(const SoundData& soundData);

public:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

private:

	// インスタンス
	static Audio* instance;

};