#pragma once
#include "KamataEngine.h"
#include "Matrix.h"
class Item {

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	bool collected_ = false;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	KamataEngine::Vector3 GetWorldPosition();

	AABB GetAABB();

	bool IsCollected() const { return collected_; }

	void Collect() { collected_ = true; }
};
