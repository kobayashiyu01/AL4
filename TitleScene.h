#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "WorldMatrix.h"
class TitleScene {

private:
	bool finished_ = false;
	Fade* fade_ = nullptr;

	KamataEngine::Model* titleModel_ = nullptr;

	WorldMatrix* worldMatrix_ = nullptr;

	KamataEngine::WorldTransform worldTransformTitle_;

	KamataEngine::Camera camera_;

	KamataEngine::Model* playerModel_ = nullptr;

	KamataEngine::WorldTransform worldTransformPlayer_;

	static inline const float fadeTimer = 1.0f;

	float titleTimer_ = 0.0f;

	// 上下に動く幅
	static inline const float kTitleMoveRange = 0.2f;

	// 上下運動の速さ
	static inline const float kTitleMoveSpeed = 2.0f;

	// 元のY座標
	float titleBaseY_ = 0.0f;

public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	Phase phase_ = Phase::kFadeIn;

	~TitleScene();
	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }
};
