#include "Hook.h"
#include "algorithm"
#include "math.h"
#include <numbers>

using namespace KamataEngine;
void Hook::Initialize(Model* model, Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	velocity_ = {0.0f, 0.0f, 0.0f};
	state = Hook::State::kInactive;
	worldTransform_.TransferMatrix();
}

void Hook::Fire(Direction direction,  Vector3& position) {

	 // 発射時点のプレイヤー位置を設定
	worldTransform_.translation_ = position;

	// フックの発射位置を更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	state = State::kFlying;

	if (direction == Direction::kUp) {
		velocity_ = {0.0f, 0.2f, 0.0f};
	} else if (direction == Direction::kDown) {
		velocity_ = {0.0f, -0.2f, 0.0f};
	}
}

void Hook::Stop() {
	if (state != State::kFlying) {
		return;
	}

	// フックの移動を停止
	velocity_ = {0.0f, 0.0f, 0.0f};

	// 停止状態に変更
	state = State::kStopped;
}

void Hook::Reset() {
	state = State::kInactive;
	velocity_ = {0.0f, 0.0f, 0.0f};
}

void Hook::Update() {

	if (state != State::kFlying) {
		return;
	}

	Vector3 nextPosition = worldTransform_.translation_;

	nextPosition.y += velocity_.y;

	Vector3 offsetPosition = nextPosition;

	//if (velocity_.y > 0.0f) {
	//	// 上向きに飛んでいる場合
	//	offsetPosition.y;
	//} else if (velocity_.y < 0.0f) {
	//	// 下向きに飛んでいる場合
	//	offsetPosition.y;
	//}

	if (mapChipField_ != nullptr) {
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(offsetPosition);

		if (mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {

			velocity_ = {0.0f, 0.0f, 0.0f};
			state = State::kAttached;

			worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

			worldTransform_.TransferMatrix();
			return;
		}
	}

	worldTransform_.translation_ = nextPosition;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

}

void Hook::Draw() {

	if (state != State::kFlying && state != State::kStopped && state != State::kAttached) {
		return;
	}

	Model::PreDraw();
	model_->Draw(worldTransform_, *camera_);
	Model::PostDraw();
}
