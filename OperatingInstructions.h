#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "WorldMatrix.h"
class OperatingInstructions {
private:

	bool finished_ = false;
	Fade* fade_ = nullptr;

	uint32_t textureHandle_ = 0u;

	KamataEngine::Sprite* sprite_ = nullptr;

	static inline const float fadeTimer = 1.0f;

public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	Phase phase_ = Phase::kFadeIn;

	~OperatingInstructions();

	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }
};
