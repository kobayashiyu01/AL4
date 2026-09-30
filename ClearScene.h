#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "WorldMatrix.h"
class ClearScene {
private:
	bool finished_ = false;
	Fade* fade_ = nullptr;

	KamataEngine::Model* clearModel_ = nullptr;

	WorldMatrix* worldMatrix_ = nullptr;

	KamataEngine::WorldTransform worldTransformTitle_;

	KamataEngine::Camera camera_;

	static inline const float fadeTimer = 1.0f;

public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	Phase phase_ = Phase::kFadeIn;

	~ClearScene();
	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }
};
