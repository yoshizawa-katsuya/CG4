#include "TitleScene.h"
#include "imgui/imgui.h"
#include "SceneManager.h"

void TitleScene::Initialize()
{

	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();
	spritePlatform_ = SpritePlatform::GetInstance();
	modelPlatform_ = ModelPlatform::GetInstance();
	/*
	textureHandle_ = TextureManager::GetInstance()->Load("./resources/Title.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(textureHandle_, spritePlatform_);
	sprite_->SetPosition({ 100.0f, 100.0f });
	*/
	title1Sprite_ = TextureManager::GetInstance()->Load("./resources/Title1.png");
	title2Sprite_ = TextureManager::GetInstance()->Load("./resources/Title2.png");

	sprite1_ = std::make_unique<Sprite>();
	sprite1_->Initialize(title1Sprite_, spritePlatform_);
	sprite1_->SetPosition({ 0.0f, 0.0f });

	sprite2_ = std::make_unique<Sprite>();
	sprite2_->Initialize(title2Sprite_, spritePlatform_);
	sprite2_->SetPosition({ 0.0f, 0.0f });


	time = 40;

	//bgm1_ = audio_->LoopSoundLoadWave("./resources/Sound/BGM_01.wav");
	//audio_->SoundLoopPlayWave(bgm1_, 1.0f);

	isFadeOut = false;

	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update()
{

#ifdef _DEBUG
	/*
	ImGui::Begin("Window");

	ImGui::End();
	*/
#endif // _DEBUG

	fade_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		//シーン切り替え依頼
		if (!isFadeOut) {

			fade_->Start(Fade::Status::FadeOut, 1.0f);

		}
		
		isFadeOut = true;

	}

	if (isFadeOut && !fade_->GetContinue()) {
		sceneManager_->ChengeScene("GAMEPLAY");
	}

}

void TitleScene::Draw()
{

	//Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	spritePlatform_->PreDraw();

	
	time--;
	if (time <= 40) {
		sprite1_->Draw();
		if (time <= 20) {
			sprite2_->Draw();
		}
	}
	if (time == 0) {
		time = 40;
	}

	fade_->Draw();
}

void TitleScene::Finalize()
{

}
