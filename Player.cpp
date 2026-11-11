#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "Matrix.h"
#include "algorithm"
#include "assert.h"
#include "math.h"
#include <numbers>

using namespace KamataEngine;

Player::~Player() { delete hook_; }

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {

	// assert(model);

	// 引数として受け取ったデータをメンバ変数にセット

	model_ = model;

	camera_ = camera;

	hook_ = new Hook();

	hookModel_ = Model::CreateFromOBJ("hook", true);
	Vector3 hookPosition = position;
	hookPosition.y += 0.5f;

	hook_->Initialize(hookModel_, camera_, hookPosition);

	ropeModel_ = Model::CreateFromOBJ("rope", true);
	ropeWorldTransform_.Initialize();
	// ropeWorldTransform_.scale_ = {1.0f, 1.0f, 0.5f};

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void Player::SetMapChipField(MapChipField* mapChipField) {
	mapChipField_ = mapChipField;

	if (hook_ != nullptr) {
		hook_->SetMapChipField(mapChipField);
	}
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
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_D)) {

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x = kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					turnFirstRotationY = worldTransform_.rotation_.y;

					turnTimer_ = kTurnTime;
				}
			} else if (Input::GetInstance()->PushKey(DIK_A)) {

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

		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}

	} else {

		// フックがブロックに刺さっているか確認
		// フックが接続されているかどうかを保存する。変数に条件を保存する書き方
		bool isHookAttached = hook_ != nullptr && hook_->GetState() == Hook::State::kAttached;

		if (isHookAttached) {
			// フックに接続している間は重力を無効にする
			velocity_.y = 0.0f;
		} else {
			// 通常時は重力を適用
			velocity_.y += -kGravityAcceleration;
			velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		}
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
	if (mapChipType == MapChipType::kBlock /*&& mapChipTypeNext != MapChipType::kBlock*/) {
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

void Player::UpdateRope() {

	if (hook_ == nullptr || ropeModel_ == nullptr) {
		return;
	}

	// フックが飛んでいる、止まっている、
	// またはブロックに引っ掛かっているときだけ表示する
	Hook::State hookState = hook_->GetState();

	if (hookState != Hook::State::kFlying && hookState != Hook::State::kStopped && hookState != Hook::State::kAttached) {
		return;
	}

	Vector3 start = worldTransform_.translation_; // ロープが出る地点
	Vector3 end = hook_->GetPosition();           // ロープが止まる地点

	float dx = end.x - start.x;
	float dy = end.y - start.y;
	float dz = end.z - start.z;

	float length = std::sqrt(dx * dx + dy * dy + dz * dz);

	if (length <= 0.001f) {
		return;
	}

	// ロープの中心を両端の中間にする
	ropeWorldTransform_.translation_ = {(start.x + end.x) * 0.5f, (start.y + end.y) * 0.5f, (start.z + end.z) * 0.5f};

	// 2D横スクロール用の回転
	ropeWorldTransform_.rotation_.z = -std::atan2(dx, dy);

	// 元のロープの長さが2の場合
	ropeWorldTransform_.scale_.y = length * 0.5f;

	ropeWorldTransform_.matWorld_ = MakeAffineMatrix(ropeWorldTransform_.scale_, ropeWorldTransform_.rotation_, ropeWorldTransform_.translation_);

	ropeWorldTransform_.TransferMatrix();
}

void Player::UpdateSwing() {
	if (hook_ == nullptr || hook_->GetState() != Hook::State::kAttached) {

		isSwingInitialized_ = false;
		swingAngleVelocity_ = 0.0f;
		return;
	}

	// フックの接続位置を支点にする
	Vector3 hookPosition = hook_->GetPosition();

	// 初めて接続したときに角度と長さを設定
	if (!isSwingInitialized_) {

		float dx = worldTransform_.translation_.x - hookPosition.x;
		float dy = worldTransform_.translation_.y - hookPosition.y;

		swingLength_ = std::sqrt(dx * dx + dy * dy);

		if (swingLength_ < 0.1f) {
			swingLength_ = 0.1f;
		}

		// 真下を0度として角度を求める
		swingAngle_ = std::atan2(dx, -dy);

		swingAngleVelocity_ = 0.0f;
		isSwingInitialized_ = true;
	}

	// 左右の入力で振り子を動かす
	const float swingPower = 0.001f;

	if (Input::GetInstance()->PushKey(DIK_A)) {
		swingAngleVelocity_ -= swingPower;
	}

	if (Input::GetInstance()->PushKey(DIK_D)) {
		swingAngleVelocity_ += swingPower;
	}

	// 振り子の自然な揺れ
	const float gravity = 0.015f;

	swingAngleVelocity_ += -(gravity / swingLength_) * std::sin(swingAngle_);

	// 揺れが徐々に収まるようにする
	swingAngleVelocity_ *= 0.995f;

	// 約45度
	// const float maxSwingAngle = 0.785398f;
	// 約65度
	const float maxSwingAngle = 1.134464f;

	// 角度を更新
	swingAngle_ += swingAngleVelocity_;

	// 右側の限界
	if (swingAngle_ > maxSwingAngle) {
		swingAngle_ = maxSwingAngle;

		// 限界に達したら、それ以上進まないようにする
		if (swingAngleVelocity_ > 0.0f) {
			swingAngleVelocity_ = 0.0f;
		}
	}

	// 左側の限界
	if (swingAngle_ < -maxSwingAngle) {
		swingAngle_ = -maxSwingAngle;

		if (swingAngleVelocity_ < 0.0f) {
			swingAngleVelocity_ = 0.0f;
		}
	}

	// E/Qでロープの長さを変更する
	const float hookMoveSpeed = 0.05f;

	if (Input::GetInstance()->PushKey(DIK_E)) {
		swingLength_ -= hookMoveSpeed;
	}

	if (Input::GetInstance()->PushKey(DIK_Q)) {
		swingLength_ += hookMoveSpeed;
	}

	swingLength_ = std::max(swingLength_, 1.0f);

	// 振り子による移動先を計算
	Vector3 targetPosition = worldTransform_.translation_;

	targetPosition.x = hookPosition.x + std::sin(swingAngle_) * swingLength_;

	targetPosition.y = hookPosition.y - std::cos(swingAngle_) * swingLength_;

	// 現在位置から移動先までの距離
	float directionX = targetPosition.x - worldTransform_.translation_.x;
	float directionY = targetPosition.y - worldTransform_.translation_.y;

	float distance = std::sqrt(directionX * directionX + directionY * directionY);

	// 1回の移動を小さく分割する
	const float maxStep = 0.1f;

	int stepCount = static_cast<int>(std::ceil(distance / maxStep));

	if (stepCount < 1) {
		stepCount = 1;
	}

	float stepX = directionX / stepCount;
	float stepY = directionY / stepCount;

	bool isHitWall = false;

	for (int i = 0; i < stepCount; i++) {
		CollisionMapInfo collisionMapInfo;
		collisionMapInfo.movement_ = {stepX, stepY, 0.0f};

		MapCollision(collisionMapInfo);
		IsHitMove(collisionMapInfo);

		if (collisionMapInfo.IsWallContact_ || collisionMapInfo.IsCeilingCollision_ || collisionMapInfo.IsLanding_) {

			isHitWall = true;
			break;
		}
	}
	// 実際のプレイヤー位置とフックの位置から再計算
	float dx = worldTransform_.translation_.x - hookPosition.x;
	float dy = worldTransform_.translation_.y - hookPosition.y;

	// 実際の位置から距離と角度を再計算
	float actualLength = std::sqrt(dx * dx + dy * dy);

	swingAngle_ = std::atan2(dx, -dy);

	// 壁に当たった場合は、壁に向かう勢いを弱める
	if (isHitWall) {
		swingAngleVelocity_ *= 0.2f;
	}

	// 実際の距離をロープの長さに反映
	swingLength_ = actualLength;

	// 最低限の長さを維持
	swingLength_ = std::max(swingLength_, 1.0f);

	UpdateRope();
}

void Player::LaunchFromSwing() {
	if (hook_ == nullptr || hook_->GetState() != Hook::State::kAttached) {
		return;
	}

	// 振り子の角速度をプレイヤーの移動速度に変換
	velocity_.x = std::cos(swingAngle_) * swingLength_ * swingAngleVelocity_;

	velocity_.y = std::sin(swingAngle_) * swingLength_ * swingAngleVelocity_;

	// 勢いを少し強める
	const float launchPower = 2.0f;
	velocity_.x *= launchPower;
	velocity_.y *= launchPower;

	// フックを解除
	hook_->Reset();

	// 振り子の初期化を解除
	isSwingInitialized_ = false;
}

bool Player::IsHookAttached() const { return hook_ != nullptr && hook_->GetState() == Hook::State::kAttached; }

bool Player::IsOutOfMap() const {

	const Vector3& position = worldTransform_.translation_;

	// マップの下に落ちた場合
	if (position.y < -2.0f) {
		return true;
	}

	// マップの左右の範囲外に出た場合
	if (position.x < -1.0f || position.x > 100.0f) {
		return true;
	}

	return false;
}

void Player::Update() {

	MoveKeys();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (hook_->GetState() == Hook::State::kAttached) {
			// 振り子の勢いを利用して飛び出す
			LaunchFromSwing();
		} else if (hook_->GetState() == Hook::State::kInactive) {
			// フックを発射
			Vector3 hookPosition = worldTransform_.translation_;
			hookPosition.y += 0.5f;
			hook_->Fire(Hook::Direction::kUp, hookPosition);
		} else {
			// 飛行中のフックを停止・回収
			hook_->Reset();
		}
	}

	hook_->Update();
	// フックがブロックに接続しているか判定
	bool isHookAttached = hook_ != nullptr && hook_->GetState() == Hook::State::kAttached;

	// フック中の移動処理
	if (isHookAttached) {
		UpdateSwing();
	}

	// 通常のブロック衝突判定は常に実行
	CollisionMapInfo collisionMapInfo;
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
	UpdateRope();
	UpdateSwing();
}

void Player::Draw() {

	if (hook_ != nullptr && hookModel_ != nullptr) {
		Hook::State hookState = hook_->GetState();

		if (hookState == Hook::State::kFlying || hookState == Hook::State::kStopped || hookState == Hook::State::kAttached) {
			hook_->Draw();
		}
	}

	Model::PreDraw();

	// ロープを描画
	if (ropeModel_ != nullptr && hook_ != nullptr) {
		Hook::State hookState = hook_->GetState();

		if (hookState == Hook::State::kFlying || hookState == Hook::State::kStopped || hookState == Hook::State::kAttached) {
			ropeModel_->Draw(ropeWorldTransform_, *camera_);
		}
	}
	if (!isDead_) {
		model_->Draw(worldTransform_, *camera_);
	}

	Model::PostDraw();
}