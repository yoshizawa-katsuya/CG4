#pragma once
#include "DirectXCommon.h"
#include "PrimitiveDrawer.h"

class SpritePlatform
{
public:

	//シングルトンインスタンスの取得
	static SpritePlatform* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon, PrimitiveDrawer* primitiveDrawer);

	//背景描画前設定
	void PreBackGroundDraw();

	//共通描画設定
	void PreDraw();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:

	SpritePlatform() = default;
	~SpritePlatform() = default;
	SpritePlatform(SpritePlatform&) = default;
	SpritePlatform& operator=(SpritePlatform&) = default;

	DirectXCommon* dxCommon_;

	PrimitiveDrawer* primitiveDrawer_;

};

