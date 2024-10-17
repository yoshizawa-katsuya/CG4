#pragma once
#include <cstdlib>
#include <memory>
#include <map>
#include "BaseScene.h"
#include "Audio.h"
#include "Input.h"
#include "ModelPlatform.h"
#include "SpritePlatform.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "Sprite.h"
#include "Player.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "ParticleEmitter.h"
#include "MapChipField.h"
#include "Enemy.h"

class GameScene : public BaseScene
{
public:

	~GameScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void Finalize() override;

private:

	//マップの生成
	void GeneratrBlocks();

	// プレイヤーと敵の当たり判定
	void CheckCollision();
	

	//デバイス
	DirectXCommon* dxCommon_;

	Audio* audio_;
	Input* input_;

	//LoopSoundData bgm1_;

	SpritePlatform* spritePlatform_;
	ModelPlatform* modelPlatform_;

	Camera* mainCamera_ = nullptr;

	std::unique_ptr<MapChipField> mapChipField_;

	//ステージの壁
	std::vector<std::vector<std::unique_ptr<WorldTransform>>> worldTransformBlocks_;
	//床
	std::vector<std::vector<std::unique_ptr<WorldTransform>>> worldTransformFloors_;
	//ばね
	std::vector<std::vector<std::unique_ptr<WorldTransform>>> worldTransformSprings_;
	//マップ上の動かない壁
	std::vector<std::vector<std::unique_ptr<WorldTransform>>> worldTransformWalls_;

	std::unique_ptr<Camera> camera_;
	std::unique_ptr<Camera> camera2_;

	std::unique_ptr<DebugCamera> debugCamera_;

	bool isActiveDebugCamera_ = false;

	//SoundData soundData1_;

	//平行光源
	std::unique_ptr<DirectionalLight> directionalLight_;

	std::unique_ptr<Model> modelPlayer_;
	std::unique_ptr<Model> modelBlock_;
	std::unique_ptr<Model> modelFloor_;
	std::unique_ptr<Model> modelWall_;

	//uint32_t textureHandle_;
	//std::unique_ptr<Sprite> sprite_;

	//プレイヤー
	std::unique_ptr<Player> player_;

	std::unique_ptr<Enemy> enemy_;

	std::list<Enemy*> enemys_;
	
	//bool IsVerticalMovement_ = false;
};

