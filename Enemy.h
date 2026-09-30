#pragma once
#include "KamataEngine.h"
#include "Matrix.h"
#include "MapChipField.h"

class Player;

class Enemy {

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 移動速度
	 float kWalkSpeed_ = 0.1f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	MapChipField* mapChipField_ = nullptr;

	// 移動方向
	 float moveDirection_ = -1.0f; 

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;

	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 45.0f;

	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	// 経過時間
	float walkTimer_ = 0.0f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// めり込み防止用
	static inline const float kMin = 0.05f;

private:
	// 壁判定
	bool CheckWall();

	// 進行方向に床があるか
	bool CheckGround();

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3& position, float kWalkSpeed);
	void Update();
	void Draw();
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	KamataEngine::Vector3 GetWorldPosition();

	AABB GetAABB();

	void OnCollision(const Player* player);
};
