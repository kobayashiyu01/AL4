#include "KamataEngine.h"
#include "MapChipField.h"
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
		kStopped,
		kAttached,
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	void Fire(Direction direction,KamataEngine::Vector3& position);

	void Stop();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void Reset();

	State GetState() const { return state; }

	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
	
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	MapChipField* mapChipField_ = nullptr;

	KamataEngine::Vector3 position_;

	KamataEngine::Vector3 velocity_;

	Hook::State state = Hook::State::kInactive;

	// bool isActive_ = false;
};
