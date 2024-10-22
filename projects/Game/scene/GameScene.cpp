#include "GameScene.h"
#include "Vector.h"
#include "dx12.h"
#include "imgui/imgui.h"
#include "ParticleManager.h"
#include "LevelDataLoader.h"
#include "Matrix.h"
#include "SceneManager.h"

GameScene::~GameScene() {
	
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();
	spritePlatform_ = SpritePlatform::GetInstance();
	modelPlatform_ = ModelPlatform::GetInstance();

	//bgm1_ = audio_->LoopSoundLoadWave("./resources/Sound/bgm01.wav");

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(dxCommon_);

	modelPlatform_->SetDirectionalLight(directionalLight_.get());

	// マップチップフィールドの生成
	mapChipField_ = std::make_unique<MapChipField>();
	//mapChipField_->LoadMapChipCsv("Resources/csv/demoStage.csv");
	mapChipField_->CreateRandomMapChip("stage");

	//カメラの生成
	camera_ = std::make_unique<Camera>();
	camera_->SetRotate({ 0.0f, 0.0f, 0.0f });
	camera_->SetTranslate({ 0.0f, 0.0f, -10.0f });

	//デバッグカメラの生成
	camera2_ = std::make_unique<Camera>();
	camera2_->SetRotate({ 0.0f, 0.0f, 0.0f });
	camera2_->SetTranslate({ 0.0f, 0.0f, -10.0f });
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(camera2_.get(), input_);

	//メインカメラの設定
	mainCamera_ = camera_.get();

	//モデルの生成
	modelPlayer_ = std::make_unique<Model>();
	modelPlayer_->Initialize(modelPlatform_);
	modelPlayer_->CreateModel("./resources/player", "Player.obj");
	
	modelEnemy_ = std::make_unique<Model>();
	modelEnemy_->Initialize(modelPlatform_);
	modelEnemy_->CreateModel("./resources/enemy", "enemy.obj");

	modelBlock_ = std::make_unique<Model>();
	modelBlock_->Initialize(modelPlatform_);
	modelBlock_->CreateModel("./resources/block", "block.obj");

	modelFloor_ = std::make_unique<Model>();
	modelFloor_->Initialize(modelPlatform_);
	modelFloor_->CreateModel("./resources/floor", "floor.obj");

	modelWall_ = std::make_unique<Model>();
	modelWall_->Initialize(modelPlatform_);
	modelWall_->CreateModel("./resources/wall", "wall.obj");

	modelSpring_ = std::make_unique<Model>();
	modelSpring_->Initialize(modelPlatform_);
	modelSpring_->CreateModel("./resources/spring", "spring.obj");

	modelThorn_ = std::make_unique<Model>();
	modelThorn_->Initialize(modelPlatform_);
	modelThorn_->CreateModel("./resources/thornPanel", "thornPanel.obj");

	modelWTS_ = std::make_unique<Model>();
	modelWTS_->Initialize(modelPlatform_);
	modelWTS_->CreateModel("./resources/sideThormWall", "sideThormWall.obj");

	modelWallThorn_ = std::make_unique<Model>();
	modelWallThorn_->Initialize(modelPlatform_);
	modelWallThorn_->CreateModel("./resources/thormWall", "thormWall.obj");

	modelGoal_ = std::make_unique<Model>();
	modelGoal_->Initialize(modelPlatform_);
	modelGoal_->CreateModel("./resources/goal", "goal.obj");
	
	/*
	//テクスチャハンドルの生成
	textureHandle_ = TextureManager::GetInstance()->Load("./resources/player/Player.png");

	//スプライトの生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(textureHandle_, spritePlatform_);
	*/

	//プレイヤーの初期化
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get());
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(12, 13);
	player_->SetTranslate(playerPosition);
	player_->SetMapChipField(mapChipField_.get());

	PopEnemyByMapChip();

	/*enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(modelPlayer_.get());
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(2, 7);
	enemy_->SetTranslate(enemyPosition);
	enemy_->SetMapChipField(mapChipField_.get());

	enemys_.push_back(enemy_.get());*/

	//マップの生成
	GeneratrBlocks();
	
	//audio_->SoundLoopPlayWave(bgm1_, 0.5f);

}

void GameScene::Update() {

	Vector3 pp = player_->GetPosition();

	Vector3 cp = camera_->GetTranslate();

	float diff = pp.y - cp.y;

	float movement = diff * 0.1f;

	camera_->SetTranslate({ 8.48f,cp.y + movement,-57.32f });

	//カメラの更新
	camera_->Update();

	if (isActiveDebugCamera_) {
		debugCamera_->Update();
	}

	//ブロックの更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformBlockLine : worldTransformBlocks_) {
		for (std::unique_ptr<WorldTransform>& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->UpdateMatrix();
		}
	}

	//足場の更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformFloorLine : worldTransformFloors_) {
		for (std::unique_ptr<WorldTransform>& worldTransformFloor : worldTransformFloorLine) {
			if (!worldTransformFloor) {
				continue;
			}
			worldTransformFloor->UpdateMatrix();
		}
	}

	//壁の更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformWallLine : worldTransformWalls_) {
		for (std::unique_ptr<WorldTransform>& worldTransformWall : worldTransformWallLine) {
			if (!worldTransformWall) {
				continue;
			}
			worldTransformWall->scale_.x = 0.5f;

			worldTransformWall->UpdateMatrix();
		}
	}

	//ばねの更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformSpringLine : worldTransformSprings_) {
		for (std::unique_ptr<WorldTransform>& worldTransformSpring : worldTransformSpringLine) {
			if (!worldTransformSpring) {
				continue;
			}
			//worldTransformSpring->scale_ = { 2.0f,2.0f,2.0f };

			worldTransformSpring->UpdateMatrix();
		}
	}

	//棘の更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformThornLine : worldTransformThorns_) {
		for (std::unique_ptr<WorldTransform>& worldTransformThorn : worldTransformThornLine) {
			if (!worldTransformThorn) {
				continue;
			}

			worldTransformThorn->UpdateMatrix();
		}
	}

	//棘壁( 片方だけver )の更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformWTSLine : worldTransformWTSs_) {
		for (std::unique_ptr<WorldTransform>& worldTransformWTS : worldTransformWTSLine) {
			if (!worldTransformWTS) {
				continue;
			}
			worldTransformWTS->scale_.x = 0.5f;

			worldTransformWTS->UpdateMatrix();
		}
	}

	//棘壁( 両方ver )の更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformWallThornLine : worldTransformWallThorns_) {
		for (std::unique_ptr<WorldTransform>& worldTransformWallThorn : worldTransformWallThornLine) {
			if (!worldTransformWallThorn) {
				continue;
			}
			worldTransformWallThorn->scale_.x = 0.5f;

			worldTransformWallThorn->UpdateMatrix();
		}
	}

	//ゴールの更新
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformGoalLine : worldTransformGoals_) {
		for (std::unique_ptr<WorldTransform>& worldTransformGoal : worldTransformGoalLine) {
			if (!worldTransformGoal) {
				continue;
			}
			//worldTransformGoal->scale_.x = 0.5f;

			worldTransformGoal->UpdateMatrix();
		}
	}

	//プレイヤーの更新
	player_->Update();

	//enemy_->Update();

	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	CheckCollision();

	CheckOutGoal();

	player_->SetIsUpMove(false);
	player_->SetIsDownMove(false);

#ifdef _DEBUG


	ImGui::Begin("Window");
	if (ImGui::TreeNode("camera")) {
		ImGui::DragFloat3("translate", &camera_->GetTranslate().x, 0.01f);
		ImGui::DragFloat3("rotate", &camera_->GetRotate().x, 0.01f);
		//ImGui::DragFloat3("scale", &cameratransform.scale.x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("directionalLight")) {
		ImGui::ColorEdit4("color", &directionalLight_->GetColor().x);
		ImGui::DragFloat3("direction", &directionalLight_->GetDirection().x, 0.01f);
		ImGui::DragFloat("intensity", &directionalLight_->GetIntensity(), 0.01f);

		ImGui::TreePop();
	}
	//メインカメラの切り替え
	if (ImGui::RadioButton("gameCamera", !isActiveDebugCamera_)) {
		isActiveDebugCamera_ = false;

		mainCamera_ = camera_.get();


	}
	if (ImGui::RadioButton("DebugCamera", isActiveDebugCamera_)) {
		isActiveDebugCamera_ = true;

		mainCamera_ = camera2_.get();

	}
	/*
	if (ImGui::Button("BGMstop")) {
		audio_->SoundStopWave(bgm1_);
	}
	*/
	ImGui::End();

#endif // _DEBUG

}

void GameScene::Draw() {

	//Spriteの背景描画前処理
	spritePlatform_->PreBackGroundDraw();

	//sprite_->Draw();

	//Modelの描画前処理
	modelPlatform_->PreDraw();
	//プレイヤーの描画
	player_->Draw(mainCamera_);

	//enemy_->Draw(mainCamera_);
	for (Enemy* enemy : enemys_) {
		enemy->Draw(mainCamera_);
	}

	//ブロックの描画
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformBlockLine : worldTransformBlocks_) {
		for (std::unique_ptr<WorldTransform>& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, mainCamera_);
		}
	}

	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformFloorLine : worldTransformFloors_) {
		for (std::unique_ptr<WorldTransform>& worldTransformFloor : worldTransformFloorLine) {
			if (!worldTransformFloor) {
				continue;
			}
			modelFloor_->Draw(*worldTransformFloor, mainCamera_);
		}
	}

	//壁の描画
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformWallLine : worldTransformWalls_) {
		for (std::unique_ptr<WorldTransform>& worldTransformWall : worldTransformWallLine) {
			if (!worldTransformWall) {
				continue;
			}
			modelWall_->Draw(*worldTransformWall, mainCamera_);
		}
	}

	//ばねの描画
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformSpringLine : worldTransformSprings_) {
		for (std::unique_ptr<WorldTransform>& worldTransformSpring : worldTransformSpringLine) {
			if (!worldTransformSpring) {
				continue;
			}
			modelSpring_->Draw(*worldTransformSpring, mainCamera_);
		}
	}

	//棘の描画
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformThornLine : worldTransformThorns_) {
		for (std::unique_ptr<WorldTransform>& worldTransformThorn : worldTransformThornLine) {
			if (!worldTransformThorn) {
				continue;
			}
			modelThorn_->Draw(*worldTransformThorn, mainCamera_);
		}
	}

	//棘壁( 片方だけver )の描画
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformWTSLine : worldTransformWTSs_) {
		for (std::unique_ptr<WorldTransform>& worldTransformWTS : worldTransformWTSLine) {
			if (!worldTransformWTS) {
				continue;
			}
			modelWTS_->Draw(*worldTransformWTS, mainCamera_);
		}
	}

	//棘壁( 両方ver )の描画
	for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformWallThornLine : worldTransformWallThorns_) {
		for (std::unique_ptr<WorldTransform>& worldTransformWallThorn : worldTransformWallThornLine) {
			if (!worldTransformWallThorn) {
				continue;
			}
			modelWallThorn_->Draw(*worldTransformWallThorn, mainCamera_);
		}
	}

	if (isOutGoal) {
		//ゴール描画
		for (std::vector<std::unique_ptr<WorldTransform>>& worldTransformGoalLine : worldTransformGoals_) {
			for (std::unique_ptr<WorldTransform>& worldTransformGoal : worldTransformGoalLine) {
				if (!worldTransformGoal) {
					continue;
				}
				modelGoal_->Draw(*worldTransformGoal, mainCamera_);
			}
		}
	}

	//Spriteの描画前処理
	spritePlatform_->PreDraw();
	
}

void GameScene::Finalize()
{

}

//void GameScene::GeneratrBlocks()
//{
//
//
//	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
//	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
//
//	//要素数を変更する
//	worldTransformBlocks_.resize(numBlockVirtical);
//	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
//		worldTransformBlocks_[i].resize(numBlockHorizontal);
//		worldTransformFloors_[i].resize(numBlockHorizontal);
//	}
//
//	worldTransformFloors_.resize(numBlockVirtical);
//	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
//	}
//
//	// キューブの生成
//	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
//		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
//			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
//				worldTransformBlocks_[i][j] = std::make_unique<WorldTransform>();
//				worldTransformBlocks_[i][j]->Initialize();
//				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
//			}
//
//			//床
//			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kFloor) {
//				worldTransformFloors_[i][j] = std::make_unique<WorldTransform>();
//				worldTransformFloors_[i][j]->Initialize();
//				worldTransformFloors_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
//			}
//
//		}
//	}
//
//
//}

void GameScene::GeneratrBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 配列を初期化
	worldTransformBlocks_.resize(numBlockVirtical);
	worldTransformFloors_.resize(numBlockVirtical);
	worldTransformWalls_.resize(numBlockVirtical);
	worldTransformSprings_.resize(numBlockVirtical);
	worldTransformThorns_.resize(numBlockVirtical);
	worldTransformWTSs_.resize(numBlockVirtical);
	worldTransformWallThorns_.resize(numBlockVirtical);
	worldTransformGoals_.resize(numBlockVirtical);

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
		worldTransformFloors_[i].resize(numBlockHorizontal);
		worldTransformWalls_[i].resize(numBlockHorizontal);
		worldTransformSprings_[i].resize(numBlockHorizontal);
		worldTransformThorns_[i].resize(numBlockHorizontal);
		worldTransformWTSs_[i].resize(numBlockHorizontal);
		worldTransformWallThorns_[i].resize(numBlockHorizontal);
		worldTransformGoals_[i].resize(numBlockHorizontal);
	}

	// キューブと床の生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			// ブロックの生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				worldTransformBlocks_[i][j] = std::make_unique<WorldTransform>();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}

			// 床の生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kFloor) {
				worldTransformFloors_[i][j] = std::make_unique<WorldTransform>();
				worldTransformFloors_[i][j]->Initialize();
				worldTransformFloors_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}

			//壁の生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kWall) {
				worldTransformWalls_[i][j] = std::make_unique<WorldTransform>();
				worldTransformWalls_[i][j]->Initialize();
				worldTransformWalls_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}

			//ばねの生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSpring) {
				worldTransformSprings_[i][j] = std::make_unique<WorldTransform>();
				worldTransformSprings_[i][j]->Initialize();
				worldTransformSprings_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}

			//棘の生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kThorn) {
				worldTransformThorns_[i][j] = std::make_unique<WorldTransform>();
				worldTransformThorns_[i][j]->Initialize();
				worldTransformThorns_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}

			//棘壁( 片方だけver )の生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kWallThornSide) {
				worldTransformWTSs_[i][j] = std::make_unique<WorldTransform>();
				worldTransformWTSs_[i][j]->Initialize();
				worldTransformWTSs_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}

			//棘壁( 両方ver )の生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kWallThorn) {
				worldTransformWallThorns_[i][j] = std::make_unique<WorldTransform>();
				worldTransformWallThorns_[i][j]->Initialize();
				worldTransformWallThorns_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}

			//ゴールの生成
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kGoal) {
				worldTransformGoals_[i][j] = std::make_unique<WorldTransform>();
				worldTransformGoals_[i][j]->Initialize();
				worldTransformGoals_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);

				Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(12, 13);
				player_->SetTranslate(playerPosition);
			}
		}
	}
}

void GameScene::CheckCollision()
{

	if (!player_->GetCanHit()) {
		return;
	}

	// プレイヤーの位置を取得
	Vector3 playerPosition = player_->GetPosition();

	// 敵が居る分回す
	for (Enemy* enemy : enemys_) {

		// 敵の位置を取得
		Vector3 vector3Diff = enemy->GetPosition() - playerPosition;

		// プレイヤーと敵の距離を計算
		float floatDiff = Length(vector3Diff);

		if (floatDiff < 1.0f) {

			// プレイヤーの上下移動のフラグが立っている状態で当たったら

			if (player_->GetIsUpMove()) {

				player_->SetIsUpMove(false);

				enemys_.remove(enemy);

				return;
			}
			else if (player_->GetIsDownMove()) {

				player_->SetIsDownMove(false);

				enemys_.remove(enemy);

				return;
			}

			// 上下移動フラグが立っていない状態で当たったら

			player_->IsHitEnemy();

			if (!player_->GetIsAlive()) {
				sceneManager_->ChengeScene("GAMEOVER");
			}

		}
	}
}

void GameScene::PopEnemyByMapChip()
{
	for (int i = 0; i < int(mapChipField_->GetNumBlockVirtical()); ++i) {
		for (int j = 0; j < int(mapChipField_->GetNumBlockHorizontal()); ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSpawnSpace) {

				/*enemy_ = std::make_unique<Enemy>();
				enemy_->Initialize(modelEnemy_.get());
				Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				enemy_->SetTranslate(enemyPosition);
				enemy_->SetMapChipField(mapChipField_.get());

				enemys_.push_back(enemy_.get());*/

				Enemy* enemy = new Enemy();

				enemy->Initialize(modelEnemy_.get());
				Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				enemy->SetTranslate(enemyPosition);
				enemy->SetMapChipField(mapChipField_.get());

				enemys_.push_back(enemy);
			}

		}
	}
}

void GameScene::CheckOutGoal()
{

	if (!enemys_.empty()) {
		return;
	}

	isOutGoal = true;

	CheckIsGoal();

}

void GameScene::CheckIsGoal()
{
	if (isOutGoal && player_->GetIsGoal()) {
		sceneManager_->ChengeScene("GAMECLEAR");
	}
}
