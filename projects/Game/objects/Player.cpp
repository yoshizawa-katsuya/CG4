#define NOMINMAX
#include "Player.h"
#include "imgui/imgui.h"
#include "Input.h"
#include <numbers>
#include <algorithm>
#include "Easing.h"
#include "MapChipField.h"

void Player::Initialize(Model* model) {

	model_ = model;
	
	worldTransform_.Initialize();

	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };

	worldTransform_.UpdateMatrix();

	Vector3 startVelocity = { 0.075f,0.0f,0.0f };

	velocity_ = startVelocity;

	playerHP_ = 3;

	kMoveTimer = 0;

	lrDirection_ = LRDirection::kRight;

	turnTimer_ = 0.1f;
}

void Player::Update() {

	if (isAlive_) {

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

		worldTransform_.UpdateMatrix();

#ifdef _DEBUG

		ImGui::Begin("Player");
		if (ImGui::TreeNode("Model")) {
			ImGui::ColorEdit4("color", &model_->GetMaterialDataAddress().color.x);
			ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);

			ImGui::TreePop();
		}
		ImGui::End();

#endif // _DEBUG	

	}
}

void Player::Draw(Camera* camera) {

	if (isDraw) {
		model_->Draw(worldTransform_, camera);
	}
}

void Player::Move()
{
#pragma region 元のコード

	//// 移動入力
	//// 接地状態
	//if (onGround_) {

	//	// 左右移動操作()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

	//		// 左右加速
	//		Vector3 acceleration = {};
	//		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
	//			// 左移動中の右入力
	//			if (velocity_.x < 0.0f) {
	//				// 速度と逆方向に入力中は急ブレーキ
	//				velocity_.x *= (1.0f - kAttenuation);
	//			}
	//			acceleration.x += kAcceleration;
	//			if (lrDirection_ != LRDirection::kRight) {
	//				lrDirection_ = LRDirection::kRight;
	//				turnFirstRotationY_ = worldTransform_.rotatio
	//	if (Input::GetInstancen_.y;
	//				turnTimer_ = 1.0f;
	//			}
	//		}
	//		else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
	//			// 右入力中の左入力
	//			if (velocity_.x > 0.0f) {
	//				// 速度と逆方向に入力中は急ブレーキ
	//				velocity_.x *= (1.0f - kAttenuation);
	//			}
	//			acceleration.x -= kAcceleration;
	//			if (lrDirection_ != LRDirection::kLeft) {
	//				lrDirection_ = LRDirection::kLeft;
	//				turnFirstRotationY_ = worldTransform_.rotation_.y;
	//				turnTimer_ = 1.0f;
	//			}
	//		}
	//		// 加速/減速
	//		velocity_.x += acceleration.x;

	//		// 最大速度制限
	//		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	//	}
	//	else {

	//		// 非入力時には移動減衰をかける
	//		velocity_.x *= (1.0f - kAttenuation);
	//	}



	//	if (Input::GetInstance()->PushKey(DIK_UP)) {
	//		// ジャンプ加速
	//		velocity_.y += kJumpAcceleration;
	//		onGround_ = false;
	//	}

	//}
	//// 空中
	//else {

	//	// 落下速度
	//	velocity_.y += -kGravityAcceleration;
	//	// 落下速度制限
	//	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	//}

#pragma endregion 元のコード

	isGoal = false;

	if (kCoolTime_ > 0.0f) {

		kCoolTime_ -= kDeltaTiem;

		Blinking();

	}

	if (kCoolTime_ <= 0.0f && !canHit) {

		canHit = true;

		isDraw = true;
	}

	ChaeckSpaceKey();

	UpMove();

	DownMove();

}

void Player::MapCollision(CollisionMapInfo& info)
{

	MapCollisionUp(info);
	MapCollisionBottom(info);
	MapCollisionRight(info);
	MapCollisionLeft(info);

}

void Player::MapCollisionUp(CollisionMapInfo& info)
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

	//棘壁との当たり判定を行う
	bool thornWallHit = false;



	//棘の当たり判定
	bool thornHit = false;

	//ばねに当たったかどうか
	bool springHit = false;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}

	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}

	//ばね
	if (mapChipType == MapChipType::kSpring) {
		springHit = true;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}

	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}

	//ばね
	if (mapChipType == MapChipType::kSpring) {
		 springHit= true;
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

	if (thornWallHit) {
		isAlive_ = false;
	}
	if (thornHit) {
		isAlive_ = false;
	}
	if (springHit) {

		velocity_.y = 6.0f;

		//worldTransform_.translation_.y += 6.0f;
	}

}

void Player::MapCollisionBottom(CollisionMapInfo& info)
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

	//棘壁との当たり判定を行う
	bool thornWallHit = false;



	//棘の当たり判定
	bool thornHit = false;


	//ばねに当たったかどうか
	bool springHit = false;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}

	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}

	//ばね
	if (mapChipType == MapChipType::kSpring) {
		springHit = true;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}
	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}
	//ばね
	if (mapChipType == MapChipType::kSpring) {
		springHit = true;
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

	if (thornWallHit) {
		isAlive_ = false;
	}
	if (thornHit) {
		isAlive_ = false;
	}
	if (springHit) {
		worldTransform_.translation_.y += 6.0f;
	}

}

void Player::MapCollisionRight(CollisionMapInfo& info)
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

	//棘壁との当たり判定を行う
	bool thornWallHit = false;



	//棘の当たり判定
	bool thornHit = false;


	//ばねに当たったかどうか
	bool springHit = false;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}

	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}

	//ばね
	if (mapChipType == MapChipType::kSpring) {
		springHit = true;
	}

	
	if (mapChipType == MapChipType::kGoal) {
		isGoal = true;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}

	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}

	//ばね
	if (mapChipType == MapChipType::kSpring) {
		springHit = true;
	}

	if (mapChipType == MapChipType::kGoal) {
		isGoal = true;
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

	if (thornWallHit) {
		if (canHit) {
			IsHitEnemy();
		}
	}

	if (thornHit) {
		if (canHit) {
			IsHitEnemy();
		}
	}

	if (springHit) {
		worldTransform_.translation_.y += 6.0f;

		if (worldTransform_.translation_.y >= 24.0f) {

			worldTransform_.translation_.y = 24.0f;

		}
	}

}

void Player::MapCollisionLeft(CollisionMapInfo& info)
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

	//棘壁との当たり判定を行う
	bool thornWallHit = false;


	//棘の当たり判定
	bool thornHit = false;


	//ばねに当たったかどうか
	bool springHit = false;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}

	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}

	//ばね
	if (mapChipType == MapChipType::kSpring) {
		springHit = true;
	}

	if (mapChipType == MapChipType::kGoal) {
		isGoal = true;
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

	//棘壁
	if (mapChipType == MapChipType::kWallThorn) {
		thornWallHit = true;
	}

	//棘
	if (mapChipType == MapChipType::kThorn) {
		thornHit = true;
	}

	//ばね
	if (mapChipType == MapChipType::kSpring) {
		springHit = true;
	}

	if (mapChipType == MapChipType::kGoal) {
		isGoal = true;
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

	if (thornWallHit) {
		
		if (canHit) {
			IsHitEnemy();
		}

	}

	if (thornHit) {
		
		if (canHit) {
			IsHitEnemy();
		}

	}

	if (springHit) {
		worldTransform_.translation_.y += 6.0f;

		if (worldTransform_.translation_.y >= 24.0f) {

			worldTransform_.translation_.y = 24.0f;

		}
	}

}

void Player::MoveAppli(const CollisionMapInfo& info)
{
	//移動
	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
	worldTransform_.translation_.z += info.move.z;

}

void Player::CeilingCollision(const CollisionMapInfo& info)
{

	//天井に当たった?
	if (info.isCeilingCollision) {
		velocity_.y = 0;
	}
}

void Player::GroundCollision(const CollisionMapInfo& info)
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

			//棘壁との当たり判定を行う
			bool thornWallHit = false;

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

void Player::WallCollision(const CollisionMapInfo& info)
{

#pragma region 元のコード

	//壁接触による減速
	//if (info.isWallCollision) {
	//	velocity_.x *= (1.0f - kAttenuationWall);
	//}

#pragma endregion 元のコード

	if (info.isWallCollision) {
		velocity_.x *= -1.0f;

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
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner)
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

void Player::ChaeckSpaceKey()
{
	//// 前のフレームと今のフレームでスペースキーを押して居なかったらreturn
	//if (!Input::GetInstance()->PushKey(DIK_SPACE) && !Input::GetInstance()->IsPushKeyPre(DIK_SPACE)) {
	//	return;
	//}

	if (!kMoveTimer == 0) {
   		kMoveTimer--;
		return;
	}

	// 上下移動の境界時間
	int borderTime = 12;

	// スペースキーを押して、isPushSpaceがfalseだったら
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !isPushSpace && kMoveTimer == 0) {
		isPushSpace = true;

		kPushTime += 1;
	}

	if (isPushSpace) {

		// スペースキーを押しっぱなしだったら
		if (Input::GetInstance()->HoldKey(DIK_SPACE)) {
			kPushTime += 1;
		}

		if (kPushTime == borderTime) {

			isDownMove = true;

  			isPushSpace = false;

			kPushTime = 0;

			// インターバルをセット
			kMoveTimer = kMoveInterval;
		}

		// スペースキーを離した瞬間
		else if (Input::GetInstance()->ReleaseKey(DIK_SPACE)) {
			isPushSpace = false;
		}

	}

	if (!isPushSpace && !kPushTime == 0) {

		isUpMove = true;

  		kPushTime = 0;

		// インターバルをセット
		kMoveTimer = kMoveInterval;
	}

}

void Player::UpMove()
{

	if (isUpMove) {

		if (worldTransform_.translation_.y >= 24.0f) {
			return;
		}

		worldTransform_.translation_.y += 3.0f;

		//velocity_.y = 3.0f;
	}
}

void Player::DownMove()
{

	if (isDownMove) {
		if (worldTransform_.translation_.y <= 2.0f) {
			return;
		}

		worldTransform_.translation_.y -= 3.0f;

		//velocity_.y = -3.0f;
	}
}

void Player::SetIsUpMove(bool flag)
{
	isUpMove = flag;
}

void Player::SetIsDownMove(bool flag)
{
	isDownMove = flag;
}

void Player::IsHitEnemy()
{
	kCoolTime_ = kHitInterval_;

	--playerHP_;

	canHit = false;

	kBlinkingTime_ = 0.1f;

	if (playerHP_ <= 0) {

		isAlive_ = false;
	}
}

void Player::Blinking()
{
	kBlinkingTime_ -= kDeltaTiem;

	if (kBlinkingTime_ > 0.05f) {
		isDraw = false;
	}
	else if(kBlinkingTime_ < 0.05 && kBlinkingTime_ > 0.0f) {
		isDraw = true;
	}
	else if (kBlinkingTime_ <= 0.0f) {
		kBlinkingTime_ = 0.1f;
	}

}
