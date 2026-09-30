
#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "SkyDome.h"
#include "Item.h"
#include "Goal.h"
#include "math.h"
#include "vector"
class GameScene {

	// ゲームのフェーズ（型）
	enum class Phase {
		kFadeIn,  // フェードイン
		kPlay,    // ゲームプレイ
		kDeath,   // デス演出
		kFadeOut, // フェードアウト
	};

	// ゲームの現在のフェーズ（変数）
	Phase phase_;


private:

	CameraController* cameraController_ = nullptr;

	SkyDome* skyDome_ = nullptr;

	Player* player_ = nullptr;

	// ゴール
	std::vector<Goal*> goals_;

	std::list<Enemy*> enemies_;

	// アイテム
	std::vector<Item*> items_;

	DeathParticles* deathParticles_ = nullptr;

	MapChipField* mapChipField_;

	Fade* fade_;

	int collectedItemCount_ = 0;

	// ゴールを出現させるために必要なアイテム数
	static inline const int kRequiredItemCount = 3;

	static inline const float fadeTimer = 1.0f;

	// 3Dモデルデータ
	KamataEngine::Model* playerModel_ = nullptr;

	KamataEngine::Model* blockModel_ = nullptr;

	KamataEngine::Model* enemyModel_ = nullptr;

	KamataEngine::Model* deathParticleModel_ = nullptr;

	KamataEngine::Model* skyDomeModel_ = nullptr;

	KamataEngine::Model* itemModel_ = nullptr;

	KamataEngine::Model* goalModel_ = nullptr;

	// カメラ
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	static inline const uint32_t kEnemyCount = 5;

	float enemySpeed_ = 0.1f;

	bool isDebugCameraActive_ = false;

	// 終了フラグ
	bool finished_ = false;

	// ゴール出現済みか
	bool isGoalAppeared_ = false;

	// クリアしたか
	bool isClear_ = false;


public:
	enum Result { kNone, kClear, kGameOver };
	Result result_ = Result::kNone;
	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();

	bool IsAABBCollision(const AABB& aabb1, const AABB& aabb2);

	void GamePlayPhase();
	void DeathPhase();

	void ChangePhase();

	Result GetResult() const { return result_; }
	bool IsFinished() const { return finished_; }
};
