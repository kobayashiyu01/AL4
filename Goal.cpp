#include "Goal.h"


using namespace KamataEngine;

void Goal::Initialize(Model* model, Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	// ブロックより奥に配置
	worldTransform_.translation_.z = 0.5f;
	
	active_ = false;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

AABB Goal::GetAABB() {

	Vector3 position = worldTransform_.translation_;

	AABB aabb;

	aabb.min = {position.x - kWidth / 2.0f, position.y - kHeight / 2.0f, position.z - kWidth / 2.0f};

	aabb.max = {position.x + kWidth / 2.0f, position.y + kHeight / 2.0f, position.z + kWidth / 2.0f};

	return aabb;
}

void Goal::Update() {

	if (!active_) {
		return;
	}

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void Goal::Draw() {

	if (!active_) {
		return;
	}

	Model::PreDraw();

	model_->Draw(worldTransform_, *camera_);

	Model::PostDraw();
}

