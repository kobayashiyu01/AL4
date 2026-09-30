#include "ClearScene.h"
#include "math.h"
#include <numbers>

using namespace KamataEngine;
ClearScene::~ClearScene() {
	delete fade_;
	delete worldMatrix_;
}

	void ClearScene::Initialize() {
	clearModel_ = Model::CreateFromOBJ("gameClearFont", true);
	worldTransformTitle_.Initialize();
	camera_.Initialize();
	camera_.translation_.z = -10.0f;
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTimer);
	worldMatrix_ = new WorldMatrix();
}

void ClearScene::Update() {
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

void ClearScene::Draw() {
	switch (phase_) {
	case Phase::kFadeIn:

		fade_->Draw();

		break;
	case Phase::kMain:
		Model::PreDraw();
		clearModel_->Draw(worldTransformTitle_, camera_);
		Model::PostDraw();
		break;
	case Phase::kFadeOut:
		fade_->Draw();
		break;
	}
}