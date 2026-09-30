#include "Item.h"
#include "Matrix.h"

using namespace KamataEngine;

void Item::Initialize(Model* model, Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

}

Vector3 Item::GetWorldPosition() { return worldTransform_.translation_; }

AABB Item::GetAABB() {

	Vector3 position = worldTransform_.translation_;

	AABB aabb;

	aabb.min = {position.x - kWidth / 2.0f, position.y - kHeight / 2.0f, position.z - kWidth / 2.0f};

	aabb.max = {position.x + kWidth / 2.0f, position.y + kHeight / 2.0f, position.z + kWidth / 2.0f};

	return aabb;
}

void Item::Update() {

	if (collected_) {
		return;
	}

	worldTransform_.rotation_.y += 0.03f;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void Item::Draw() {

	if (collected_) {
		return;
	}

	Model::PreDraw();

	model_->Draw(worldTransform_, *camera_);

	Model::PostDraw();
}
