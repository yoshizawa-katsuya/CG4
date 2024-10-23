#include "MyGame.h"
#include "SceneFactory.h"

void MyGame::Initialize()
{

	//基底クラスの初期化処理
	YKFramework::Initialize();

	//ゲームシーンの初期化
	//gameScene_ = new GameScene;
	//gameScene_->Initialize();

	//最初のシーンの生成
	//BaseScene* scene = new TitleScene();
	//シーンマネージャに最初にシーンをセット
	//sceneManager_->SetNextScene(scene);

	//シーンファクトリを生成し、マネージャにセット
	sceneFactory_ = new SceneFactory();
	sceneManager_->SetSceneFactory(sceneFactory_);
	//シーンマネージャに最初のシーンをセット
	sceneManager_->ChengeScene("TITLE");

	bgm1_ = audio_->LoopSoundLoadWave("./resources/Sound/BGM_01.wav");
	audio_->SoundLoopPlayWave(bgm1_, 1.0f);

}

void MyGame::Finalize()
{

	//解放処理
	//delete gameScene_;
	//gameScene_ = nullptr;


	YKFramework::Finalize();

}

void MyGame::Update()
{

	YKFramework::Update();

	//ゲームの処理
	//gameScene_->Update();


	YKFramework::EndFrame();

}

void MyGame::Draw()
{

	//描画開始
	dxCommon_->PreDraw();

	srvHeapManager_->PreDraw();

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	primitiveDrawer_->SetPipelineSet(dxCommon_->GetCommandList(), BlendMode::kBlendModeNone);
	/*
	dxCommon->GetCommandList()->SetGraphicsRootSignature(primitiveDrawer->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(primitiveDrawer->GetGrahicsPipelineState());	//PSOを設定
	*/
	//commandList_->IASetIndexBuffer(&indexBufferView);	//IBVを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//gameScene_->Draw();
	//titleScene_->Draw();
	sceneManager_->Draw();

#ifdef _DEBUG

	//実際のcommandListのImGuiの描画コマンドを積む
	imGuiManager_->Draw();

#endif // _DEBUG

	dxCommon_->PostDraw();


}
