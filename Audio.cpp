#include "Audio.h"

// staticメンバ変数で宣言したインスタンスを初期化
Audio* Audio::instance = nullptr;

// 開放する
void Audio::Finalize()
{
	
}

void Audio::Init() {
	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

SoundData Audio::LoadWave(const char* filename) {

	// ファイルオープン
	std::fstream file;
	file.open(filename, std::ios_base::binary);
	assert(file.is_open());

	// wav.データの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// TYPEがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り専用チャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// DAtaチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// ファイルを閉じる
	file.close();

	/// 読み込んだ音声データを返す
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;


}

void Audio::UnLoadWave(SoundData* soundData)
{
	// バッファのメモリを開放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::PlayWave(const SoundData& soundData) 
{
	HRESULT hr;

	// 波形フォーマットデータをもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSoundVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSoundVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	hr = pSoundVoice->SubmitSourceBuffer(&buf);
	hr = pSoundVoice->Start();
	
}

void Audio::StopWave(const SoundData& soundData) {
	
}