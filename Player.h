#pragma once
#include "Hook.h"
#include "KamataEngine.h"
#include "Matrix.h"
// #include"MapChipField.h"

class MapChipField;
class Enemy;
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上

	kNumCorner // 要素数
};

class Player {

	enum class LRDirection {

		kRight,
		kLeft,
	};

	struct CollisionMapInfo {
		bool IsCeilingCollision_ = false;
		bool IsLanding_ = false;
		bool IsWallContact_ = false;
		KamataEngine::Vector3 movement_;
	};

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::WorldTransform ropeWorldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* hookModel_ = nullptr;
	KamataEngine::Model* ropeModel_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	LRDirection lrDirection_ = LRDirection::kRight;

	MapChipField* mapChipField_ = nullptr;

	Hook* hook_ = nullptr;

	// 旋回開始時の角度
	float turnFirstRotationY = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間＜秒＞
	static inline const float kTurnTime = 0.3f;

	static inline const float kAcceleration = 0.05f;
	static inline const float kAttenuation = 0.3f;
	static inline const float kLimitRunSpeed = 0.2f;

	bool onGround_ = true;

	static inline const float kBlank = 1.0f;
	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kJumpAcceleration = 0.5f;
	static inline const float kAttenuationLanding = 0.2f;
	static inline const float kAttenuationWall = 0.2f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kMin = 0.05f;
	static inline const float kGroundCheck = 0.05f;
	const KamataEngine::Vector3& Getvelocity() const { return velocity_; }

	// デスフラグ
	bool isDead_ = false;

	// 振り子用の変数
	float swingAngle_ = 0.0f;         // 振り子の角度
	float swingAngleVelocity_ = 0.0f; // 角速度
	float swingLength_ = 0.0f;        // 支点からプレイヤーまでの距離
	bool isSwingInitialized_ = false; // 振り子の初期化済み判定
	bool isReturning_ = false;
	bool isLandingFromSwing_ = false;

public:
	~Player();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void MoveKeys();

	void Update();

	void UpdateRope();

	void UpdateSwing();

	void Draw();

	void SetMapChipField(MapChipField* mapChipField);

	void MapCollision(CollisionMapInfo& info);

	void MapUpCollision(CollisionMapInfo& info);
	void MapDownCollision(CollisionMapInfo& info);
	void MapRightCollision(CollisionMapInfo& info);
	void MapLeftCollision(CollisionMapInfo& info);
	void IsHitMove(const CollisionMapInfo& info);
	void IsHitWall(const CollisionMapInfo& info);

	void IsHitCeiling(const CollisionMapInfo& info);

	void SwitchingInstallationMode(const CollisionMapInfo& info);

	void OnCollision(const Enemy* enemy);

	void LaunchFromSwing();

	bool IsOnGround();

	bool IsHookAttached() const;

	bool IsOutOfMap() const;

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	KamataEngine::Vector3& GetVelocity() { return velocity_; }

	KamataEngine::Vector3 GetWorldPosition();

	AABB GetAABB();

	// デスフラグのgetter
	bool IsDead() const { return isDead_; };
};
