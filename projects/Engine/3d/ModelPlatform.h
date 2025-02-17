#pragma once
#include "DirectXCommon.h"
#include "PrimitiveDrawer.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <array>
#include "SrvHeapManager.h"
class Camera;

class ModelPlatform
{
public:

	//シングルトンインスタンスの取得
	static ModelPlatform* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon, PrimitiveDrawer* primitiveDrawer, SrvHeapManager* srvHeapManager);

	void EndFrame();

	//共通描画設定
	void PreDraw();

	void SkinPreDraw();

	void InstancingPreDraw();

	void LinePreDraw();

	void LineDraw(const Matrix4x4& worldMatrix1, const Matrix4x4& worldMatrix2, Camera* camera);

	void SpherePreDraw();

	void SphereDraw(const Matrix4x4& worldMatrix, Camera* camera);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	SrvHeapManager* GetSrvHeapManager() const { return srvHeapManager_; }

	void SetDirectionalLight(DirectionalLight* directionalLight) { directionalLight_ = directionalLight; }

	void SetPointLight(PointLight* pointLight) { pointLight_ = pointLight; }

	void SetSpotLight(SpotLight* spotLight) { spotLight_ = spotLight; }

	void SetCamera(Camera* camera) { camera_ = camera; }

private:

	struct LineWVP
	{
		Matrix4x4 WVP1;
		Matrix4x4 WVP2;
	};

	ModelPlatform() = default;
	~ModelPlatform() = default;
	ModelPlatform(ModelPlatform&) = default;
	ModelPlatform& operator=(ModelPlatform&) = default;

	DirectXCommon* dxCommon_;

	SrvHeapManager* srvHeapManager_;

	PrimitiveDrawer* primitiveDrawer_;

	DirectionalLight* directionalLight_ = nullptr;

	PointLight* pointLight_ = nullptr;

	SpotLight* spotLight_ = nullptr;

	Camera* camera_;

	//VertexResourceを生成
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

	Vector4* vertexData_ = nullptr;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//static const uint32_t resourceNum_ = 2048;
	static const uint32_t resourceNum_ = 1;

	//TransformationMatrix用のリソースを作る 線分用
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, resourceNum_> LineWVPResources_;
	//Microsoft::WRL::ComPtr<ID3D12Resource> LineWVPResource_;
	//データを書き込む
	std::array<LineWVP*, resourceNum_> LineWVPDatas_;
	//LineWVP* LineWVPData_ = nullptr;

	uint32_t lineIndex_ = 0;

	//TransformationMatrix用のリソースを作る。球用
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, resourceNum_> SphereWVPResources_;
	//Microsoft::WRL::ComPtr<ID3D12Resource> WVPResource_;
	//データを書き込む
	std::array<Matrix4x4*, resourceNum_> SphereWVPDatas_;
	//Matrix4x4* WVPData_ = nullptr;

	uint32_t sphereIndex_ = 0;
	/*
	//TransformationMatrix用のリソースを作る。モデル用
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, resourceNum_> ModelWVPResources_;
	//Microsoft::WRL::ComPtr<ID3D12Resource> WVPResource_;
	//データを書き込む
	std::array<TransformationMatrix*, resourceNum_> ModelWVPDatas_;
	//Matrix4x4* WVPData_ = nullptr;
	
	uint32_t modelIndex_ = 0;
	*/
};

