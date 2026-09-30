#pragma once
#include "KamataEngine.h"
#include "Matrix.h"

class Goal {

private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	bool active_ = false;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void SetActive(bool active) { active_ = active; }

	bool IsActive() const { return active_; }

	void Update();

	void Draw();

	AABB GetAABB();
};
