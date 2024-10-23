#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "SrvHeapManager.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "SpritePlatform.h"
#include "Model.h"
#include "ModelPlatform.h"
#include "PrimitiveDrawer.h"
#include "ParticleManager.h"
#include "Input.h"
#include "dx12.h"
#include "D3DResourceLeakChecker.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include <format>
#include <dxgidebug.h>
#include <dxcapi.h>
#define _USE_MATH_DEFINES
#include <math.h>

//ゲーム全体
class YKFramework
{
public:

	virtual ~YKFramework() = default;

	//初期化
	virtual void Initialize();

	//終了
	virtual void Finalize();

	//更新
	virtual void Update();

	void EndFrame();

	//描画
	virtual void Draw() = 0;

	//実行
	void Run();

	//終了フラグのチェック
	virtual bool GetIsEndReqest() { return isEndReqest_; }

protected:


	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	Audio* audio_ = nullptr;
	SrvHeapManager* srvHeapManager_ = nullptr;
	std::unique_ptr<ImGuiManager> imGuiManager_;
	Input* input_ = nullptr;
	PrimitiveDrawer* primitiveDrawer_ = nullptr;
	SpritePlatform* spritePlatform_ = nullptr;
	ModelPlatform* modelPlatform_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

	//ゲーム終了フラグ
	bool isEndReqest_ = false;

};

