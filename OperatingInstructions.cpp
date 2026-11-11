#include "OperatingInstructions.h"
#include "Matrix.h"
#include "math.h"
#include <numbers>
using namespace KamataEngine;

OperatingInstructions::~OperatingInstructions() { 
	delete sprite_;
	delete fade_;
}

void OperatingInstructions::Initialize() {

	textureHandle_ = TextureManager::Load("operatingInstructions.png");

	sprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTimer);

}

void OperatingInstructions::Update() {

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
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

void OperatingInstructions::Draw() {
	switch (phase_) {
	case Phase::kFadeIn:

		Sprite::PreDraw();
		sprite_->Draw();
		Sprite::PostDraw();
		fade_->Draw();

		break;
	case Phase::kMain:

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
