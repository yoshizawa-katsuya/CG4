#define NOMINMAX
#include "Enemy.h"
#include "imgui/imgui.h"
#include "Input.h"
#include <numbers>
#include <algorithm>
#include "Easing.h"
#include "MapChipField.h"

void Enemy::Initialize(Model* model)
{

	model_ = model;

	worldTransform_.Initialize();

	Vector3 startVelocity = { 0.025f,0.0f,0.0f };

	velocity_ = startVelocity;

}

void Enemy::Update()
{

	//移動入力
	Move();

	//衝突判定を初期化
	CollisionMapInfo collisionMapInfo;
	//移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	//マップ衝突チェック
	MapCollision(collisionMapInfo);

	//移動
	MoveAppli(collisionMapInfo);

	CeilingCollision(collisionMapInfo);

	GroundCollision(collisionMapInfo);

	WallCollision(collisionMapInfo);

	worldTransform_.UpdateMatrix();

}

void Enemy::Draw(Camera* camera)
{
	model_->Draw(worldTransform_, camera);
}

void Enemy::Move()
{

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = { std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> *3.0f / 2.0f };

		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = turnFirstRotationY_ * EaseOut(turnTimer_) + destinationRotationY * (1 - EaseOut(turnTimer_));
	}

}

void Enemy::MapCollision(CollisionMapInfo& info)
{

	MapCollisionUp(info);
	MapCollisionBottom(info);
	MapCollisionRight(info);
	MapCollisionLeft(info);

}

void Enemy::MapCollisionUp(CollisionMapInfo& info)
{


	if (info.move.y <= 0) {
		return;
	}

	//移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
			{ worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z }, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	//真上の当たり判定を行う
	bool hit = false;
	//左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}

	//右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}

	if (hit) {
		//めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		//めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2 + kBlank));
		//天井に当たったことを記録する
		info.isCeilingCollision = true;
	}

}

void Enemy::MapCollisionBottom(CollisionMapInfo& info)
{

	//下降あり?
	if (info.move.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
			{ worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z }, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}


	//ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + (kHeight / 2 + kBlank));
		// 地面に当たったことを記録する
		info.landing = true;
	}

}

void Enemy::MapCollisionRight(CollisionMapInfo& info)
{

	//右移動あり?
	if (info.move.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
			{ worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z }, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真右の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, (rect.left - worldTransform_.translation_.x) - (kWidth / 2 + kBlank));
		// 地面に当たったことを記録する
		info.isWallCollision = true;
	}


}

void Enemy::MapCollisionLeft(CollisionMapInfo& info)
{

	// 左移動あり?
	if (info.move.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
			CornerPosition({ worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z }, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真左の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 壁

	if (mapChipType == MapChipType::kWall) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::min(0.0f, (rect.right - worldTransform_.translation_.x) + (kWidth / 2 + kBlank));
		// 地面に当たったことを記録する
		info.isWallCollision = true;
	}

}

void Enemy::MoveAppli(const CollisionMapInfo& info)
{

	//移動
	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
	worldTransform_.translation_.z += info.move.z;

}

void Enemy::CeilingCollision(const CollisionMapInfo& info)
{
	//天井に当たった?
	if (info.isCeilingCollision) {
		velocity_.y = 0;
	}
}

void Enemy::GroundCollision(const CollisionMapInfo& info)
{

	//自キャラが設置状態?
	if (onGround_) {

		//ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
		else {

			// 移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] =
					CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;
			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition({ positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kBlank * 2, positionsNew[kLeftBottom].z });
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition({ positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kBlank * 2, positionsNew[kRightBottom].z });
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// ブロックにヒット?
			if (!hit) {
				//空中状態に切り替える
				onGround_ = false;
			}

		}

	}
	else {

		if (info.landing) {
			//着地状態に切り替える（落下を止める）
			onGround_ = true;
			//着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			//Y速度をゼロにする
			velocity_.y = 0.0f;
		}

	}

}

void Enemy::WallCollision(const CollisionMapInfo& info)
{

#pragma region 元のコード

	//壁接触による減速
	//if (info.isWallCollision) {
	//	velocity_.x *= (1.0f - kAttenuationWall);
	//}

#pragma endregion 元のコード

	if (info.isWallCollision) {
		velocity_.x *= -1.0f;
	}
	if (velocity_.x > 0.0f) {
		if (lrDirection_ != LRDirection::kRight) {
			lrDirection_ = LRDirection::kRight;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = 0.5f;
		}
	}

	if (velocity_.x < 0.0f) {
		if (lrDirection_ != LRDirection::kLeft) {
			lrDirection_ = LRDirection::kLeft;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = 0.5f;
		}
	}
}

Vector3 Enemy::CornerPosition(const Vector3& center, Corner corner)
{
	Vector3 offsetTable[kNumCorner] = {
		{+kWidth / 2.0f, -kHeight / 2.0f, 0},	//kRightBottom
		{-kWidth / 2.0f, -kHeight / 2.0f, 0},	//kLeftBottom
		{+kWidth / 2.0f, +kHeight / 2.0f, 0},	//kRightTop
		{-kWidth / 2.0f, +kHeight / 2.0f, 0},	//kLeftTop
	};

	return { center.x + offsetTable[static_cast<uint32_t>(corner)].x,
			center.y + offsetTable[static_cast<uint32_t>(corner)].y,
			center.z + offsetTable[static_cast<uint32_t>(corner)].z };
}