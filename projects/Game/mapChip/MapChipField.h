#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "Struct.h"

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ステージの壁
	kWall, //動かない壁
	kSpring, //ばね
	kThorn, //棘
	kWallThorn, //棘壁( 元々、壁についてるやつ )
	kFloor, //床
	kWallThornSide,//棘壁( 片方だけver)
	kGoal, //ゴール
	kSpawnSpace, // 敵の生成空間
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
///マップチップフィールド
/// <summary>
class MapChipField {	

public:
	
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	//範囲矩形
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	void ResetMapChipDate();

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	uint32_t GetNumBlockVirtical();

	uint32_t GetNumBlockHorizontal();

	void CreateRandomMapChip(const std::string& baseFilePath);

	void RandomLoadMapChipCsv(const std::string& filePath);

private:
	
	// 1ブロックのサイズ
	const float kBlockWidth = 1.0f;
	const float kBlockHeight = 1.0f;

	// ブロックの個数
	const uint32_t kNumBlockVirtical = 27;
	const uint32_t kNumBlockHorizontal = 18;

	MapChipData mapChipData_;

};

