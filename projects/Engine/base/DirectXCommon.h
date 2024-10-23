#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include <array>
#include <chrono>

#include "WinApp.h"
#include "DirectXTex/DirectXTex.h"

class DirectXCommon
{
public:

	//シングルトンインスタンスの取得
	static DirectXCommon* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(WinApp* winApp);

	//描画前処理
	void PreDraw();

	//描画後処理
	void PostDraw();
	
	Microsoft::WRL::ComPtr<IDxcBlob> CompilerShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile);

	//Resource作成の関数化
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	//TextureResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	//DiscriptorHeap作成の関数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisiblr);


	// デバイスの取得
	ID3D12Device* GetDevice() const { return device_.Get(); }

	//描画コマンドリストの取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }


	IDxcUtils* GetDxcUtils() const { return dxcUtils_.Get(); }

	IDxcCompiler3* GetDxcCompiler() const { return dxcCompiler_.Get(); }

	IDxcIncludeHandler* GetIncludeHandler() const { return includeHandler_.Get(); }

	uint32_t GetBackBufferCount() const { return backBuffers; }

	/// <summary>
	/// RTVの指定番号のCPUデスクリプタハンドルを取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// RTVの指定番号のGPUデスクリプタハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);

	

	/// <summary>
	/// DSVの指定番号のCPUデスクリプタハンドルを取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// DSVの指定番号のGPUデスクリプタハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);

	
private:

	//static DirectXCommon* instance_;

	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(DirectXCommon&) = delete;
	const DirectXCommon& operator=(DirectXCommon&) = delete;

	//DXGIデバイス初期化
	void InitializeDXGIDevice();

	//コマンド関連初期化
	void InitializeCommand();

	//スワップチェーンの生成
	void CreateSwapChain();

	//レンダーターゲットの生成
	void CreateFinalRenderTargets();

	//深度バッファ生成
	void CreateDepthBuffer();

	//各種デスクリプタヒープの生成
	void CreateDescriptorHeaps();

	//フェンス作成
	void CreateFence();
	
	//ビューポート矩形の初期化
	void CreateViewport();

	//シザー矩形の初期化
	void CreateScissorRect();

	//DXCコンパイラの生成
	void CreateDXCCompiler();

	//ImGuiの初期化
	//void ImGuiInitialize();

	//FPS固定初期化
	void InitializeFixFPS();

	//FPS固定更新
	void UpdateFixFPS();

	//DepthStencilTextureを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

	
	//DescriptorHandleを取得する関数。CPU
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	//DescriptorHandleを取得する関数。GPU
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	// ウィンドウズアプリケーション管理
	WinApp* winApp_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

	uint32_t backBuffers = 2;

	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	D3D12_VIEWPORT viewport_{};

	D3D12_RECT scissorRect_{};

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point referance_;

};

