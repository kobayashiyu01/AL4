#define NOMINMAX
#include "DeathParticles.h"
#include "MapChipField.h"
#include "Matrix.h"
#include "algorithm"
#include "assert.h"
#include "math.h"
#include <numbers>

using namespace KamataEngine;

void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
		worldTransform.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {

	// 終了なら何もしない
	if (isFinished_) {
		return;
	}
	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;

		// 終了扱いにする
		isFinished_ = true;
	}

	/*float alpha = 0.0f;
	    if (counter_ == 0) {
	        alpha = 1.0f;

	    } else if (counter_ == kDuration) {
	        alpha = 0.0f;
	    }*/

	color_.w = std::clamp(1.0f - (counter_ / kDuration), 0.0f, 1.0f);

	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
	for (uint32_t i = 0; i < kNumparticles; ++i) {

		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0.0f, 0.0f};

		// 回転角を計算する
		float angle = kAngleUnit * i;

		// Z軸回り回転角
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);

		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(velocity, matrixRotation);

		// 移動処理
		worldTransforms_[i].translation_.x += velocity.x;
		worldTransforms_[i].translation_.y += velocity.y;
		worldTransforms_[i].translation_.z += velocity.z;
	}

	// 行列更新
	// 行列を更新して定数バッファに転送

	for (WorldTransform& worldTransform : worldTransforms_) {
		Matrix4x4 translationMatrix = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		Matrix4x4 rotateMatrix = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		Matrix4x4 scaleMatrix = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.TransferMatrix();
	}
}

void DeathParticles::Draw() {

	// 終了なら何もしない
	if (isFinished_) {
		return;
	}
	
	for (WorldTransform& worldTransform : worldTransforms_) {
		Model::PreDraw();
		model_->Draw(worldTransform, *camera_, &objectColor_);
		Model::PostDraw();
	}
	
}
