#pragma once
#include <xaudio2.h>
#include <fstream>
#include "GameEngine/Base/DirectX/DirectXCommon.h"
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
	
	Audio() = default;
	~Audio() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	Audio(const Audio& obj) = delete;
	Audio& operator=(const Audio& obj) = delete;


public: // ** 静的メンバ関数 ** //

	// インスタンスを取得
	static Audio* GetInstance();
	// 解放処理
	void Finalize();
	// 初期化
	void Initialize();

	SoundData LoadWave(const char* filename);
	void UnLoadWave(SoundData* soundData);

	void PlayWave(const SoundData& soundData,bool isLoop);
	void StopWave(const SoundData& soundData);
	void SetVolume(const SoundData& soundData, float volume);

public:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;


private:

	// インスタンス
	static Audio* instance;

};