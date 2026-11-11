#include "ClearScene.h"
#include "GameOver.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "OperatingInstructions.h"
#include "TitleScene.h"

TitleScene* titleScene = nullptr;
OperatingInstructions* operatingScene = nullptr;
GameScene* gameScene = nullptr;
ClearScene* clearScene = nullptr;
GameOver* gameOverScene = nullptr;

enum class Scene {

	kUnkown = 0,

	kTitle,
	kOperating,
	kGame,
	kClear,
	kGameOver
};

Scene scene = Scene::kUnkown;

void ChangeScene() {
	switch (scene) {

	case Scene::kTitle:

		if (titleScene->IsFinished()) {

			// 古いタイトルシーンのBGMなどを停止
			titleScene->Finalize();

			// 古いタイトルシーンを解放
			delete titleScene;
			titleScene = nullptr;

			// シーン変更
			scene = Scene::kOperating;

			// 新しいシーンを生成して初期化
			operatingScene = new OperatingInstructions;
			operatingScene->Initialize();
		}
		break;

	case Scene::kOperating:
		if (operatingScene->IsFinished()) {
			
			// シーン変更
			scene = Scene::kGame;

			// 旧シーンの開放
			delete operatingScene;
			operatingScene = nullptr;

			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;

	case Scene::kGame:
		if (gameScene->IsFinished()) {

			if (gameScene->GetResult() == GameScene::Result::kClear) {

				// GameScene → GameClear
				gameScene->Finalize();
				scene = Scene::kClear;

				delete gameScene;
				gameScene = nullptr;

				clearScene = new ClearScene;
				clearScene->Initialize();

			} else if (gameScene->GetResult() == GameScene::Result::kGameOver) {

				// GameScene → GameOver
				gameScene->Finalize();
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
			clearScene->Finalize();
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
			gameOverScene->Finalize();
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

	case Scene::kOperating:
		operatingScene->Update();
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

	case Scene::kOperating:
		operatingScene->Draw();
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
	/*KamataEngine::*/ Initialize(L"LE2C_13_コバヤシ_ユウ_ふっかー");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

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

		// 現在シーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	Finalize();
	delete titleScene;
	delete operatingScene;
	delete gameScene;
	delete clearScene;
	delete gameOverScene;
	return 0;
}
