#define NOMINMAX
#include "Enemy.h"
#include "Matrix.h"
#include "algorithm"
#include "assert.h"
#include "math.h"
#include <numbers>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, Vector3& position, float walkSpeed) {
	
	kWalkSpeed_ = walkSpeed;
	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	moveDirection_ = -1.0f;

	velocity_ = {moveDirection_ * kWalkSpeed_, 0.0f, 0.0f};

	walkTimer_ = 0.0f;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
};

Vector3 Enemy::GetWorldPosition() {

	Vector3 worldPos;

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

bool Enemy::CheckWall() {

	if (mapChipField_ == nullptr) {
		return false;
	}

	Vector3 checkPosition = worldTransform_.translation_;

	// 進行方向側の端を見る
	checkPosition.x += moveDirection_ * (kWidth / 2.0f + kMin);

	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosition);

	MapChipType mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	return mapChipType == MapChipType::kBlock;
}

bool Enemy::CheckGround() {

	if (mapChipField_ == nullptr) {
		return false;
	}

	Vector3 checkPosition = worldTransform_.translation_;

	// 敵の進行方向の足元を見る
	checkPosition.x += moveDirection_ * (kWidth / 2.0f + 0.05f);

	checkPosition.y -= kHeight / 2.0f + 0.05f;

	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosition);

	MapChipType mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	return mapChipType == MapChipType::kBlock;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(const Player* player) { (void)player; }

void Enemy::Update() {

	if (mapChipField_ == nullptr) {
		return;
	}

	bool wall = CheckWall();
	bool ground = CheckGround();

	if (wall) {
		moveDirection_ *= -1.0f;
		velocity_.x = moveDirection_ * kWalkSpeed_;
	}

	if (!ground) {
		moveDirection_ *= -1.0f;
		velocity_.x = moveDirection_ * kWalkSpeed_;
	}

	// 移動方向に合わせて向きを変更
	if (moveDirection_ > 0.0f) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	} else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}

	worldTransform_.translation_.x += velocity_.x;

	// 以下はそのまま
	walkTimer_ += 1.0f / 60.0f;

	float param = std::sinf(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;

	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
};

void Enemy::Draw() {
	Model::PreDraw();
	model_->Draw(worldTransform_, *camera_);
	Model::PostDraw();
};