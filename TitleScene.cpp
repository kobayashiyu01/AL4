#include "TitleScene.h"
#include "Matrix.h"
#include "math.h"
#include <numbers>
using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete sprite_;
	delete fade_;
	delete worldMatrix_;
}

void TitleScene::Initialize() {

	titleModel_ = Model::CreateFromOBJ("titleFont", true);
	worldTransformTitle_.Initialize();
	textureHandle_ = TextureManager::Load("title.png");

	// BGM
	titleSoundDataHandle_ = Audio::GetInstance()->LoadWave("bgm/titlebgm.mp3");

	titleVoiceHandle_ = Audio::GetInstance()->PlayWave(titleSoundDataHandle_, true,0.5f);

	sprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});
	camera_.Initialize();
	camera_.translation_.z = -10.0f;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTimer);

	worldMatrix_ = new WorldMatrix();
}

void TitleScene::Update() {


	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		// タイトルを上下に揺らす
		titleTimer_ += 1.0f / 60.0f;

		worldTransformTitle_.translation_.y = titleBaseY_ + std::sinf(titleTimer_ * kTitleMoveSpeed) * kTitleMoveRange;

		// 行列を更新
		worldTransformTitle_.matWorld_ = MakeAffineMatrix(worldTransformTitle_.scale_, worldTransformTitle_.rotation_, worldTransformTitle_.translation_);

		worldTransformTitle_.TransferMatrix();
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

void TitleScene::Finalize() {
	
		Audio::GetInstance()->StopWave(titleVoiceHandle_);
}

void TitleScene::Draw() {
	switch (phase_) {
	case Phase::kFadeIn:
		Model::PreDraw();
		//titleModel_->Draw(worldTransformTitle_, camera_);
		Model::PostDraw();
		
		Sprite::PreDraw();
		sprite_->Draw();
		Sprite::PostDraw();

		fade_->Draw();

		break;
	case Phase::kMain:

		Model::PreDraw();
		//titleModel_->Draw(worldTransformTitle_, camera_);
		Model::PostDraw();

		Sprite::PreDraw();
		sprite_->Draw();
		Sprite::PostDraw();

		break;
	case Phase::kFadeOut:
		Sprite::PreDraw();
		sprite_->Draw();
		Sprite::PostDraw();
		fade_->Draw();
		break;
	}
}