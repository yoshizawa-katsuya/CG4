#pragma once
#include "Model.h"
#include "WorldTransform.h"
class Camera;
class MapChipField;

class Enemy {
private:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool isCeilingCollision = false;
		bool landing = false;
		bool isWallCollision = false;
		Vector3 move;
	};

	enum Corner {
		kRightBottom,	//右下
		kLeftBottom,	//左下
		kRightTop,		//右上
		kLeftTop,		//左上

		kNumCorner		//要素数

	};

public:

	void Initialize(Model* model);

	void Update();

	void Draw(Camera* camera);

	//移動
	void Move();

	//マップ衝突判定
	void MapCollision(CollisionMapInfo& info);
	void MapCollisionUp(CollisionMapInfo& info);
	void MapCollisionBottom(CollisionMapInfo& info);
	void MapCollisionRight(CollisionMapInfo& info);
	void MapCollisionLeft(CollisionMapInfo& info);

	//判定結果を反映して移動させる
	void MoveAppli(const CollisionMapInfo& info);

	//天井に接触している場合の処理
	void CeilingCollision(const CollisionMapInfo& info);

	//地面に接触している場合の処理
	void GroundCollision(const CollisionMapInfo& info);

	//壁に接触している場合の処理
	void WallCollision(const CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void SetTranslate(Vector3& translate) { worldTransform_.translation_ = { translate.x,translate.y - 0.5f,translate.z }; }

	// ポジション
	Vector3 GetPosition() { return worldTransform_.translation_; }

private:
	//Transform変数を作る
	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	enum  class LRDirection {
		kRight,
		kLeft,
	};

	//向き
	LRDirection lrDirection_ = LRDirection::kRight;

	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.03f;

	//接地状態フラグ
	bool onGround_ = true;

	Vector3 velocity_ = {};

	//加速度
	const float kAcceleration = 0.01f;
	//速度減衰
	const float kAttenuation = 0.1f;
	//横移動の限界速度
	const float kLimitRunSpeed = 10.0f;
	//重力加速度
	const float kGravityAcceleration = 0.03f;
	//落下時の限界速度
	const float kLimitFallSpeed = 1.0f;
	//ジャンプ時の初速度
	const float kJumpAcceleration = 0.5f;

	//壁に接触時の速度減衰比率
	const float kAttenuationWall = 1.0f;
	//着地時の速度減衰率
	const float kAttenuationLanding = 0.0f;


	//キャラクターの当たり判定サイズ
	const float kWidth = 1.0f;
	const float kHeight = 1.0f;

	//余白
	const float kBlank = 0.01f;
	//マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
};