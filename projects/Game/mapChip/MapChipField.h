#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "Struct.h"

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
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

private:
	
	// 1ブロックのサイズ
	const float kBlockWidth = 1.0f;
	const float kBlockHeight = 1.0f;

	// ブロックの個数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 10;

	MapChipData mapChipData_;

};
