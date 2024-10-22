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
}

void TitleScene::Update()
{

#ifdef _DEBUG
	/*
	ImGui::Begin("Window");

	if (ImGui::TreeNode("Sprite")) {
		ImGui::DragFloat2("tranlate", &sprite_->GetPosition().x, 0.01f);
		ImGui::DragFloat2("size", &sprite_->GetSize().x, 0.01f);
		ImGui::SliderAngle("rotate", &sprite_->GetRotation());
		ImGui::ColorEdit4("color", &sprite_->GetColor().x);
		ImGui::DragFloat2("anchprPoint", &sprite_->GetAnchorPoint().x, 0.01f);
		ImGui::Checkbox("IsFlipX", &sprite_->GetIsFlipX());
		ImGui::Checkbox("IsFlipY", &sprite_->GetIsFlipY());
		ImGui::DragFloat2("textureLeftTop", &sprite_->GetTextureLeftTop().x, 0.01f);
		ImGui::DragFloat2("textureSize", &sprite_->GetTextureSize().x, 0.01f);

		ImGui::TreePop();
	}

	ImGui::End();
	*/
#endif // _DEBUG

	if (input_->TriggerKey(DIK_SPACE)) {
		//シーン切り替え依頼
		sceneManager_->ChengeScene("GAMEPLAY");
	}

}

void TitleScene::Draw()
{

	//Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	//spritePlatform_->PreDraw();

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

}

void TitleScene::Finalize()
{

}
