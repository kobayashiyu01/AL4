#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "Matrix.h"
#include "algorithm"
#include "assert.h"
#include "math.h"
#include <numbers>

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {

	assert(model);

	// 引数として受け取ったデータをメンバ変数にセット

	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, // kLeftTop
	};
	Vector3 result{};

	return {center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z};
}

Vector3 Player::GetWorldPosition() {

	Vector3 worldPos;

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

bool Player::IsOnGround() {

	std::array<Vector3, kNumCorner> positions;

	// 足元を少しだけ下にずらす
	Vector3 checkPosition = worldTransform_.translation_ + Vector3(0.0f, -kMin, 0.0f);

	for (uint32_t i = 0; i < positions.size(); ++i) {
		positions[i] = CornerPosition(checkPosition, static_cast<Corner>(i));
	}

	MapChipField::IndexSet indexSet;

	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positions[kLeftBottom]);

	if (mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		return true;
	}

	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positions[kRightBottom]);

	if (mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		return true;
	}

	return false;
}

void Player::MoveKeys() {
	// 移動入力

	if (onGround_) {

		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x = kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					//
					turnFirstRotationY = worldTransform_.rotation_.y;

					//
					turnTimer_ = kTurnTime;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					//
					turnFirstRotationY = worldTransform_.rotation_.y;

					//
					turnTimer_ = kTurnTime;
				}
			}
			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->TriggerKey(DIK_UP)) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}

	} else {

		velocity_.y += -kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	

	// 旋回制御
	if (kTurnTime > 0.0f) {

		turnTimer_ -= 1.0f / 60.0f;

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		};

		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = Lerp(turnFirstRotationY, destinationRotationY, kTurnTime);
		worldTransform_.rotation_.y = destinationRotationY;
	}
}
void Player::IsHitMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_.x += info.movement_.x;
	worldTransform_.translation_.y += info.movement_.y;
	worldTransform_.translation_.z += info.movement_.z;
}

void Player::IsHitWall(const CollisionMapInfo& info) {

	if (info.IsWallContact_) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::IsHitCeiling(const CollisionMapInfo& info) {

	// 天井に当たった？
	if (info.IsCeilingCollision_) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::MapUpCollision(CollisionMapInfo& info) {

	if (info.movement_.y <= 0) {
		return;
	}
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真上の当たり判定
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex + static_cast<int32_t>(1));
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex + static_cast<int32_t>(1));
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		MapChipField::IndexSet indexSetNow;
		std::array<Vector3, kNumCorner> positionsOld;
		for (uint32_t i = 0; i < positionsOld.size(); ++i) {
			positionsOld[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
		}

		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionsOld[kLeftTop]);

		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			info.movement_.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - kHeight);
			info.IsCeilingCollision_ = true;
		}
	}
}

void Player::MapDownCollision(CollisionMapInfo& info) {
	if (info.movement_.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex - static_cast<int32_t>(1));
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex - static_cast<int32_t>(1));
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		MapChipField::IndexSet indexSetNow;
		std::array<Vector3, kNumCorner> positionsOld;

		for (uint32_t i = 0; i < positionsOld.size(); ++i) {
			positionsOld[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
		}
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionsOld[kLeftBottom]);

		// if (indexSetNow.yIndex != indexSet.yIndex) {

		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float landingY = rect.top + (kHeight / 2.0f);

		float yMovement = landingY - worldTransform_.translation_.y;

		// float yMovement = total + (kHeight / 2.0f) - kMin;

		info.movement_.y = yMovement;
		info.IsLanding_ = true;
		//}
	}
}

void Player::SwitchingInstallationMode(const CollisionMapInfo& info) {

	// 着地した
	if (info.IsLanding_) {
		onGround_ = true;
		velocity_.y = 0.0f;
		return;
	}

	// 上昇中
	if (velocity_.y > 0.0f) {
		onGround_ = false;
		return;
	}

	// 下降中または速度0
	// 足元にブロックがないなら空中
	if (!IsOnGround()) {
		onGround_ = false;
	}

	
}


void Player::MapLeftCollision(CollisionMapInfo& info) {

	if (info.movement_.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement_, static_cast<Corner>(i));
	}

	bool hit = false;

	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock /*&& mapChipTypeNext != MapChipType::kBlock*/) {
		hit = true;
	}

	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);

	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock/*&& mapChipTypeNext != MapChipType::kBlock*/) {
		hit = true;
	}

	if (hit) {

		// 左上の判定位置を使用
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);

		// マップ外の場合はここでは処理しない
		if (indexSet.xIndex < 0) {
			return;
		}

		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float total = rect.right - worldTransform_.translation_.x;

		float xMovement = total + (kWidth / 2.0f) + kMin;

		info.movement_.x = std::min(0.0f, xMovement);

		info.IsWallContact_ = true;
	}
}
void Player::MapRightCollision(CollisionMapInfo& info) {

	if (info.movement_.x <= 0.0f) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet = {};

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);

	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock /*&& mapChipTypeNext != MapChipType::kBlock*/) {
		hit = true;
	}

	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);

	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	mapChipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock /*&& mapChipTypeNext != MapChipType::kBlock*/) {
		hit = true;
	}

	if (hit) {

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);

		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float total = rect.left - worldTransform_.translation_.x;

		float xMovement = total - (kWidth / 2.0f) + kMin;

		info.movement_.x = std::min(0.0f, xMovement);

		info.IsWallContact_ = true;
	}
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;

	isDead_ = true;
}

void Player::MapCollision(CollisionMapInfo& info) {

	MapUpCollision(info);

	MapDownCollision(info);

	MapRightCollision(info);

	MapLeftCollision(info);
}
void Player::Update() {

	MoveKeys();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;

	// 移動量に速度の値をコピー
	collisionMapInfo.movement_ = velocity_;

	MapCollision(collisionMapInfo);

	IsHitMove(collisionMapInfo);

	IsHitCeiling(collisionMapInfo);

	SwitchingInstallationMode(collisionMapInfo);

	IsHitWall(collisionMapInfo);

	// 行列更新
	// 行列を更新して定数バッファに転送
	Matrix4x4 translationMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	Matrix4x4 rotateMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	Matrix4x4 scaleMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	Model::PreDraw();
	if (!isDead_) {
		model_->Draw(worldTransform_, *camera_);
	}

	Model::PostDraw();
}