#define NOMINMAX
#include "CameraController.h"
#include "Matrix.h"
#include "Player.h"
#include "algorithm"
#include "math.h"
#include <numbers>
using namespace KamataEngine;




void CameraController::Initialize(Camera* camera) {
	// カメラの初期化
	camera_ = camera;
};

void CameraController::Update() {
	if (camera_ == nullptr || target_ == nullptr) {
		return;
	}

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	targetPos_.x = targetWorldTransform.translation_.x + targetOffset_.x + target_->GetVelocity().x * kVelocityBias;
	targetPos_.y = targetWorldTransform.translation_.y + targetOffset_.y + target_->GetVelocity().y * kVelocityBias;
	targetPos_.z = targetWorldTransform.translation_.z + targetOffset_.z + target_->GetVelocity().z;

	// 座標補完っを使ってゆったり追従
	camera_->translation_.x = Lerp(camera_->translation_.x, targetPos_.x, kInterpolationSpeed);
	camera_->translation_.y = Lerp(camera_->translation_.y, targetPos_.y, kInterpolationSpeed);
	camera_->translation_.z = Lerp(camera_->translation_.z, targetPos_.z, kInterpolationSpeed);

	// 追従対象が画面外に行かないようにする処理
	camera_->translation_.x = std::clamp(camera_->translation_.x, targetWorldTransform.translation_.x + margin.left, targetWorldTransform.translation_.x + margin.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, targetWorldTransform.translation_.y + margin.bottom, targetWorldTransform.translation_.y + margin.top);

	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);

	// 行列を更新
	camera_->UpdateMatrix();
};

void CameraController::Reset() {

	if (camera_ == nullptr || target_ == nullptr) {
		return;
	}

	// 追従対象のワールドトランスフォーム
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
	camera_->translation_.x = std::clamp(camera_->translation_.x, targetWorldTransform.translation_.x + margin.left, targetWorldTransform.translation_.x + margin.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, targetWorldTransform.translation_.y + margin.bottom, targetWorldTransform.translation_.y + margin.top);

	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);
	camera_->UpdateMatrix();
}