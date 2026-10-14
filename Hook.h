#include "KamataEngine.h"
#include "Matrix.h"
#pragma once
class Hook {
public:
	
	enum class Direction {
		kUp,
		kDown,
	};

	enum class State {
		kInactive,
		kFlying,
		kAttached,
	};
	

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	void Fire(Direction direction);
	

private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 position_;

	KamataEngine::Vector3 velocity_;

Hook::State state = Hook::State::kInactive;
	//bool isActive_ = false;
};
