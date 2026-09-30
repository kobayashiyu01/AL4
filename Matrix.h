#pragma once
#include "KamataEngine.h"
#include "math.h"
struct AABB {
	KamataEngine::Vector3 min; //!< 最小
	KamataEngine::Vector3 max; //!< 最大
};

Matrix4x4 MakeRotateXMatrix(float radian);

Matrix4x4 MakeRotateYMatrix(float radian);

Matrix4x4 MakeRotateZMatrix(float radian);

KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const Matrix4x4& matrix);

Matrix4x4 Invers(const Matrix4x4& m);

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

float Lerp(float start, float end, float t);
