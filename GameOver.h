#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "WorldMatrix.h"
class GameOver {
private:
	bool finished_ = false;
	Fade* fade_ = nullptr;

	KamataEngine::Model* gameOverModel_ = nullptr;

	WorldMatrix* worldMatrix_ = nullptr;

	KamataEngine::WorldTransform worldTransformTitle_;

	KamataEngine::Camera camera_;

	uint32_t gameOverSoundDataHandle_ = 0;

	uint32_t gameOverVoiceHandle_ = 0;

	static inline const float fadeTimer = 1.0f;

public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	Phase phase_ = Phase::kFadeIn;

	~GameOver();
	void Initialize();

	void Update();

	void Draw();

	void Finalize();

	bool IsFinished() const { return finished_; }
};
