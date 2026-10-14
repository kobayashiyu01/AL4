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

void Hook::Fire(Direction direction) {

	state = State::kFlying;
	if (direction == Direction::kUp) {

		velocity_ = {0.0f, 0.2f, 0.0f}; // フックを上方に飛ばす

	} else if (direction == Direction::kDown) {
		velocity_ = {0.0f, -0.2f, 0.0f}; // フックを停止させる
	}
}

void Hook::Update() {

	if (state != State::kFlying) {
		return;
	}

	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Hook::Draw() {
	
	if (state != State::kFlying) {
		return;
	}
	Model::PreDraw();
	model_->Draw(worldTransform_, *camera_);
	Model::PostDraw();
}
