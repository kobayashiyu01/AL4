#include "TitleScene.h"
#include "Matrix.h"
#include "math.h"
#include <numbers>
using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete fade_;
	delete worldMatrix_;
}

void TitleScene::Initialize() {

	/*playerModel_ = Model::CreateFromOBJ("player", true);
	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> / 2.0f;*/

	titleModel_ = Model::CreateFromOBJ("titleFont", true);
	worldTransformTitle_.Initialize();

	camera_.Initialize();
	camera_.translation_.z = -10.0f;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTimer);

	worldMatrix_ = new WorldMatrix();
}

void TitleScene::Update() {

	/*if (Input::GetInstance()->PushKey(DIK_SPACE)) {
	    finished_ = true;
	}*/

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
			// worldMatrix_->WorldTransformUpdate(worldTransformPlayer_);
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

void TitleScene::Draw() {
	switch (phase_) {
	case Phase::kFadeIn:
		Model::PreDraw();
		//titleModel_->Draw(worldTransformTitle_, camera_);
		Model::PostDraw();
		fade_->Draw();

		break;
	case Phase::kMain:

		Model::PreDraw();
		//titleModel_->Draw(worldTransformTitle_, camera_);
		Model::PostDraw();
		break;
	case Phase::kFadeOut:

		fade_->Draw();
		break;
	}
}