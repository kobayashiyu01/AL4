#include "ClearScene.h"
#include "GameOver.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
ClearScene* clearScene = nullptr;
GameOver* gameOverScene = nullptr;

enum class Scene {

	kUnkown = 0,

	kTitle,
	kGame,
	kClear,
	kGameOver
};

Scene scene = Scene::kUnkown;

void ChangeScene() {
	switch (scene) {

	case Scene::kTitle:
		if (titleScene->IsFinished()) {

			// シーン変更
			scene = Scene::kGame;

			// 旧シーンの開放
			delete titleScene;
			titleScene = nullptr;

			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {

			if (gameScene->GetResult() == GameScene::Result::kClear) {

				// GameScene → GameClear
				scene = Scene::kClear;

				delete gameScene;
				gameScene = nullptr;

				clearScene = new ClearScene;
				clearScene->Initialize();

			} else if (gameScene->GetResult() == GameScene::Result::kGameOver) {

				// GameScene → GameOver
				scene = Scene::kGameOver;

				delete gameScene;
				gameScene = nullptr;

				gameOverScene = new GameOver;
				gameOverScene->Initialize();
			}
		}
		break;

	case Scene::kClear:
		if (clearScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの開放
			delete clearScene;
			clearScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;

	case Scene::kGameOver:

		if (gameOverScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameOverScene;
			gameOverScene = nullptr;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {

	case Scene::kTitle:
		titleScene->Update();

		break;
	case Scene::kGame:
		gameScene->Update();
		printf("GameScene");
		break;

	case Scene::kClear:
		clearScene->Update();
		break;
	case Scene::kGameOver:
		gameOverScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {

	case Scene::kTitle:
		
		titleScene->Draw();

		break;
	case Scene::kGame:
		gameScene->Draw();
		break;

	case Scene::kClear:
		clearScene->Draw();
		break;

	case Scene::kGameOver:
		gameOverScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// KamataEngine::←を省略できるようにする
	using namespace KamataEngine;
	/*KamataEngine::*/ Initialize(L"LE2D_11_コバヤシ_ユウ_Sand Maze");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();




	gameScene = new GameScene;
	gameScene->Initialize();

	// 最初のシーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	// ↓これ

	while (true) {
		// ↓書いても大丈夫  ////////ただし、ヘッダーファイルにusing namespaceを書いてはいけない//////
		if (KamataEngine::Update()) {
			break;
		}

		// シーン切り替え
		ChangeScene();

		// 現在シーンの更新
		UpdateScene();

		// 描画開始
		dxCommon->PreDraw();

		/*gameScene->Draw();
		titleScene->Draw();*/

		// 現在シーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	Finalize();
	delete gameScene;
	delete titleScene;
	delete clearScene;
	delete gameOverScene;
	return 0;
}
