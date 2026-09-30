#pragma once
#include "KamataEngine.h"
class Player;

class CameraController {

	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

public:
	void Initialize(KamataEngine::Camera* camera);
	void Update();
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	void SetMovableArea(const Rect& area) { movableArea_ = area; }

private:
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	Player* target_ = nullptr;
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

		// カメラの目標座標
	KamataEngine::Vector3 targetPos_ = {};

	Rect movableArea_ = {10.2f, 88.7f, 5.5f, 13.4f};


	//static inline const float kInterpolationRate = 0.5f;
	static inline const float kInterpolationSpeed = 0.3f;
	static inline const float kVelocityBias = 0.5f;
	static inline const Rect margin = {-10.0f, 10.0f, 0.0f, 0.0f};
};



