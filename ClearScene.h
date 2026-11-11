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

	uint32_t clearSoundDataHandle_ = 0;

	uint32_t clearVoiceHandle_ = 0;

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

	void Finalize();

	bool IsFinished() const { return finished_; }
};
