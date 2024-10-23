#pragma once
#include "BaseScene.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "Input.h"
#include "TextureManager.h"
#include "ModelPlatform.h"
#include "SpritePlatform.h"
#include "Sprite.h"

class TitleScene : public BaseScene
{
public:

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void Finalize() override;

private:

	//デバイス
	DirectXCommon* dxCommon_;

	Audio* audio_;
	Input* input_;

	SpritePlatform* spritePlatform_;
	ModelPlatform* modelPlatform_;

	LoopSoundData bgm1_;
	
	//uint32_t textureHandle_;

	//std::unique_ptr<Sprite> sprite_;

	uint32_t title1Sprite_ = 0;

	std::unique_ptr<Sprite> sprite1_ = nullptr;

	uint32_t title2Sprite_ = 0;

	std::unique_ptr<Sprite> sprite2_ = nullptr;

	int time = 40;
};

