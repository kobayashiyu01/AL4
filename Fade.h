#pragma once
#include "KamataEngine.h"

class Fade {

private:
	KamataEngine::Sprite* sprite_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	static inline const float kWindowWidth = 1280.0f;
	static inline const float kWindowHeight = 720.0f;

	float duration_ = 0.0f;
	float counter_ = 0.0f;

	

public:
	// フェード状態
	enum class Status {

		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};
	Status status_ = Status::None;
	void Initialize();
	void Update();
	void Draw();

	void Start(Status status, float duration);

	// フェード開始
	void Stop();
	
	// フェード修了判定
	bool IsFinished() const;
};
