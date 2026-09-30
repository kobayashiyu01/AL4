#include "GameScene.h"
#include "Matrix.h"
#include "math.h"

using namespace KamataEngine;

void GameScene::GenerateBlocks() {

	// 要素数
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHrizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVertical);

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapchipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j]->matWorld_ = MakeAffineMatrix(worldTransformBlocks_[i][j]->scale_, worldTransformBlocks_[i][j]->rotation_, worldTransformBlocks_[i][j]->translation_);
				worldTransform->TransferMatrix();
				worldTransformBlocks_[i][j]->TransferMatrix();
			}
		}
	}
}

bool GameScene::IsAABBCollision(const AABB& aabb1, const AABB& aabb2) {
	return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);
}

GameScene::~GameScene() {

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete playerModel_;
	delete blockModel_;
	delete player_;
	delete debugCamera_;
	delete skyDomeModel_;
	delete mapChipField_;
	delete deathParticles_;
	delete deathParticleModel_;
	delete skyDome_;
	delete itemModel_;
	delete goalModel_;
	for (Goal* goal : goals_) {
		delete goal;
	}
	goals_.clear();

	for (Item* item : items_) {
		delete item;
	}
	items_.clear();
	for (Enemy* enemy_ : enemies_) {
		delete enemy_;
	}
	enemies_.clear();
}
void GameScene::Initialize() {

	collectedItemCount_ = 0;
	isGoalAppeared_ = false;
	isClear_ = false;
	finished_ = false;
	result_ = Result::kNone;

	// カメラの初期化
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// camera_.farZ = 1000.0f;

	worldTransform_.Initialize();
	camera_.Initialize();

	// ゲームプレイフェーズから開始
	phase_ = Phase::kFadeIn;

	// 3Dモデルデータの生成

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	player_ = new Player();
	playerModel_ = Model::CreateFromOBJ("player_2", true);
	
	// Model
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	player_->Initialize(playerModel_, &camera_, playerPosition);

	skyDome_ = new SkyDome();
	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	skyDome_->Initialize(skyDomeModel_, &camera_);

	blockModel_ = Model::CreateFromOBJ("block");

	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);
	deathParticles_ = nullptr;

	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	for (int i = 0; i < kEnemyCount; ++i) {

		Enemy* newEnemy = new Enemy();

		const int enemyX[kEnemyCount] = {5, 31, 54, 60, 90};
		// ブロックの判定上マップチップを一段下げる
		const int enemyY[kEnemyCount] = {7, 19, 16, 19, 5};
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(enemyX[i], enemyY[i]);
		enemyPosition.y += 0.5f + 0.4f;
		if (i == 0) {
			enemySpeed_ = 0.05f;
		} else if (i == 1) {
			enemySpeed_ = 0.08f;
		} else if (i == 2) {
			enemySpeed_ = 0.1f;
		} else if (i == 3) {
			enemySpeed_ = 0.15f;
		} else if (i == 4) {
			enemySpeed_ = 0.2f;
		}
		newEnemy->Initialize(enemyModel_, &camera_, enemyPosition, enemySpeed_);
		newEnemy->SetMapChipField(mapChipField_);
		enemies_.push_back(newEnemy);
	}

	itemModel_ = Model::CreateFromOBJ("item");
	for (int i = 0; i < kRequiredItemCount; ++i) {
		Item* newItem = new Item();
		// ゲーム用
		const int itemX[kRequiredItemCount] = {1, 42, 98};
		const int itemY[kRequiredItemCount] = {6, 17, 4};
		// デバッグ用
		/*const int itemX[kRequiredItemCount] = {34, 35, 36};
		const int itemY[kRequiredItemCount] = {7, 7, 7};*/
		Vector3 itemPosition = mapChipField_->GetMapChipPositionByIndex(itemX[i], itemY[i]);
		// itemPosition.y += 0.5f + 0.4f;
		newItem->Initialize(itemModel_, &camera_, itemPosition);
		items_.push_back(newItem);
	}

	goalModel_ = Model::CreateFromOBJ("goal");
	const int goalX[4] = {97, 98, 97, 98};
	const int goalY[4] = {15, 15, 16, 16};

	for (int i = 0; i < 4; ++i) {

		Goal* newGoal = new Goal();

		Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(goalX[i], goalY[i]);

		newGoal->Initialize(goalModel_, &camera_, goalPosition);

		goals_.push_back(newGoal);
	}

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTimer);

	cameraController_ = new CameraController();
	cameraController_->Initialize(&camera_);
	cameraController_->SetTarget(player_);
	cameraController_->SetMovableArea({10.2f, 88.7f, 5.5f, 13.4f});
	cameraController_->Reset();

	player_->SetMapChipField(mapChipField_);
}

void GameScene::CheckAllCollisions() {
#pragma region
	{
		// 判定対象1と2の座標
		AABB aabb1, aabb2;

		// 自キャラの座標
		aabb1 = player_->GetAABB();

		// 自キャラと敵弾すべての当たり判定
		for (Enemy* enemy_ : enemies_) {

			// 敵弾の座標
			aabb2 = enemy_->GetAABB();

			// AABB同士の交差判定
			if (IsAABBCollision(aabb1, aabb2)) {
				// 自キャラの衝突時関数を呼び出す
				player_->OnCollision(enemy_);
				// 敵の衝突時関数を呼び出す
				enemy_->OnCollision(player_);
			}
		}
	}

	AABB playerAABB = player_->GetAABB();

	for (Item* item : items_) {

		if (item->IsCollected()) {
			continue;
		}

		if (IsAABBCollision(playerAABB, item->GetAABB())) {

			item->Collect();

			collectedItemCount_++;

			// 必要な数のアイテムを集めたらゴール出現
			if (collectedItemCount_ >= kRequiredItemCount) {

				isGoalAppeared_ = true;

				for (Goal* goal : goals_) {
					goal->SetActive(true);
				}
			}
		}
	}

	// =========================
	// プレイヤーとゴール
	// =========================
	if (isGoalAppeared_) {

		for (Goal* goal : goals_) {

			if (!goal->IsActive()) {
				continue;
			}

			if (IsAABBCollision(playerAABB, goal->GetAABB())) {

				isClear_ = true;
				result_ = Result::kClear;

				break;
			}
		}
	}

#pragma endregion
}

void GameScene::GamePlayPhase() {

	skyDome_->Update();

	player_->Update();

	for (Goal* goal : goals_) {
		goal->Update();
	}

	for (Item* item : items_) {
		item->Update();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// ブロック更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->TransferMatrix();
		}
	}

	CheckAllCollisions();

	// =========================
	// クリア
	// =========================

	if (isClear_) {

		result_ = Result::kClear;

		// ★ ここでフェードアウト開始
		fade_->Start(Fade::Status::FadeOut, fadeTimer);

		phase_ = Phase::kFadeOut;

		return;
	}

	// =========================
	// ゲームオーバー
	// =========================

	if (player_->IsDead()) {

		result_ = Result::kGameOver;

		Vector3 deathParticlesPosition = player_->GetWorldPosition();

		deathParticles_ = new DeathParticles();

		deathParticles_->Initialize(deathParticleModel_, &camera_, deathParticlesPosition);

		// ★ すぐフェードアウトせず、
		// 死亡演出を見せる
		phase_ = Phase::kDeath;

		return;
	}

	cameraController_->Update();
}

void GameScene::DeathPhase() {

	if (deathParticles_ != nullptr) {
		deathParticles_->Update();
	}

	cameraController_->Update();
}
void GameScene::ChangePhase() {

	switch (phase_) {

	case Phase::kFadeIn:

		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}

		break;

	case Phase::kPlay:

		GamePlayPhase();

		break;

	case Phase::kDeath:

		DeathPhase();

		if (deathParticles_ != nullptr && deathParticles_->IsFinished()) {

			// 死亡演出が終わったらフェードアウト
			fade_->Start(Fade::Status::FadeOut, fadeTimer);

			phase_ = Phase::kFadeOut;
		}

		break;

	case Phase::kFadeOut:

		fade_->Update();

		if (fade_->IsFinished()) {

			finished_ = true;
		}

		break;
	}
}
void GameScene::Update() { ChangePhase(); }

void GameScene::Draw() {

	skyDome_->Draw();
	player_->Draw();

	for (Goal* goal : goals_) {
		goal->Draw();
	}

	if (deathParticles_ != nullptr) {
		deathParticles_->Draw();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	for (Item* item : items_) {
		if (!item->IsCollected()) {
			item->Draw();
		}
	}

	Model::PreDraw();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();

	// ★ フェード
	if (phase_ == Phase::kFadeIn || phase_ == Phase::kFadeOut) {
		fade_->Draw();
	}
}
