#include "KamataEngine.h"
#include <array>
#include <numbers>
#pragma once
class DeathParticles {

private:
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	static inline const uint32_t kNumparticles = 8;

	// ワールド変換データ
	std::array<KamataEngine::WorldTransform, kNumparticles> worldTransforms_;

	// 色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;

	// 色の数値
	KamataEngine::Vector4 color_;

	// 存続時間（消滅までの時間）<秒>
	static inline const float kDuration = 1.0f;

	// 移動の速さ
	static inline const float kSpeed = 0.1f;

	static inline const float kAngleUnit = (2.0f * std::numbers::pi_v<float>) / kNumparticles;

	// 終了フラグ
	bool isFinished_ = false;

	// 経過時間カウント
	float counter_ = 0.0f;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	bool IsFinished() const { return isFinished_; }
};
