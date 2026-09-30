#include "SkyDome.h"

using namespace KamataEngine;

void SkyDome::Initialize(Model* model, Camera* camera) {

	model_ = model;

	camera_ = camera;
	worldTransform_.Initialize();
}

void SkyDome::Update() {}

void SkyDome::Draw() {

	Model::PreDraw();
	model_->Draw(worldTransform_, *camera_);
	Model::PostDraw();
}