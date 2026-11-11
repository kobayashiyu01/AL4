#include "GameOver.h"
#include "math.h"
#include <numbers>

using namespace KamataEngine;

GameOver::~GameOver() {

	delete fade_;
	delete worldMatrix_;
}
void GameOver::Initialize() {
	gameOverModel_ = Model::CreateFromOBJ("gameOverFont", true);
	worldTransformTitle_.Initialize();

	// BGM
	gameOverSoundDataHandle_ = Audio::GetInstance()->LoadWave("bgm/gameOverbgm.mp3");

	gameOverVoiceHandle_ = Audio::GetInstance()->PlayWave(gameOverSoundDataHandle_, true,0.5f);

	camera_.Initialize();
	camera_.translation_.z = -10.0f;
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTimer);
	worldMatrix_ = new WorldMatrix();
}

void GameOver::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			worldMatrix_->WorldTransformUpdate(worldTransformTitle_);
			camera_.UpdateMatrix();
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, fadeTimer);
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

void GameOver::Finalize() {
	if (gameOverVoiceHandle_ != 0) {
		Audio::GetInstance()->StopWave(gameOverVoiceHandle_);
		
	}
}

void GameOver::Draw() {
	switch (phase_) {
	case Phase::kFadeIn:

		fade_->Draw();

		break;
	case Phase::kMain:
		Model::PreDraw();
		gameOverModel_->Draw(worldTransformTitle_, camera_);
		Model::PostDraw();
		break;
	case Phase::kFadeOut:
		fade_->Draw();
		break;
	}
}