#include "MapChipField.h"
#include <map>
#include <fstream>
#include <sstream>
#include <assert.h>

namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
	{"2", MapChipType::kFloor},
	{"3", MapChipType::kThorn},
	{"4", MapChipType::kSpring},
	{"5", MapChipType::kWall},
	{"6", MapChipType::kWallThorn},
	{"7", MapChipType::kWallThornSide},
	{"8", MapChipType::kGoal},
	{"9", MapChipType::kSpawnSpace},
};

}

//マップチップデータをリセット
void MapChipField::ResetMapChipDate() {

    mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}

}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	ResetMapChipDate();

	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCsv;
	//ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();	
	//ファイルを閉じる
	file.close();

	std::string line;

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		
		getline(mapChipCsv, line);

		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}

		}

	}

}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	//指定ブロックの中心座標を取得する
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;

}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {

	IndexSet indexSet = {};
	indexSet.xIndex = uint32_t((position.x + kBlockWidth / 2) / kBlockWidth);
	//indexSet.yIndex = uint32_t(kNumBlockVirtical - 1 - (position.y + kBlockHeight / 2) / kBlockHeight);
	//indexSet.yIndex = uint32_t(kNumBlockVirtical - (position.y + kBlockHeight / 2) / kBlockHeight);
	indexSet.yIndex = uint32_t((position.y + kBlockHeight / 2) / kBlockHeight);
	indexSet.yIndex = kNumBlockVirtical - 1 - indexSet.yIndex;
	return indexSet;
}

uint32_t MapChipField::GetNumBlockVirtical() { return kNumBlockVirtical; }

uint32_t MapChipField::GetNumBlockHorizontal() { return kNumBlockHorizontal; }

void MapChipField::CreateRandomMapChip(const std::string& baseFilePath)
{
	ResetMapChipDate();

	unsigned int currenttime = unsigned int(time(nullptr));

	srand(currenttime);

	RandomLoadMapChipCsv(baseFilePath);
}

void MapChipField::RandomLoadMapChipCsv(const std::string& baseFilePath)
{

	for (int index = 0; index < 5; ++index) {

		int stageNum = rand() % 12 + 1;

		std::string filePath = "Resources/csv/" + baseFilePath + std::to_string(stageNum) + ".csv";

		if (index < 2) {

			//ファイルを開く
			std::ifstream file;
			file.open(filePath);
			assert(file.is_open());

			// マップチップCSV
			std::stringstream mapChipCsv;
			//ファイルの内容を文字列ストリームにコピー
			mapChipCsv << file.rdbuf();
			//ファイルを閉じる
			file.close();

			std::string line;

			for (uint32_t i = 0; i < 6; ++i) {

				getline(mapChipCsv, line);

				//1行分の文字列をストリームに変換して解析しやすくする
				std::istringstream line_stream(line);

				for (uint32_t j = 0; j < 18; ++j) {

					std::string word;
					getline(line_stream, word, ',');

					if (mapChipTable.contains(word)) {
						mapChipData_.data[index * 6 + i][j] = mapChipTable[word];
					}
				}
			}
		}
		else if (index == 2) {

			filePath = "Resources/csv/playerSpawn.csv";

			std::ifstream file;
			file.open(filePath);
			assert(file.is_open());

			// マップチップCSV
			std::stringstream mapChipCsv;
			//ファイルの内容を文字列ストリームにコピー
			mapChipCsv << file.rdbuf();
			//ファイルを閉じる
			file.close();

			std::string line;

			for (uint32_t i = 0; i < 3; ++i) {

				getline(mapChipCsv, line);

				//1行分の文字列をストリームに変換して解析しやすくする
				std::istringstream line_stream(line);

				for (uint32_t j = 0; j < 18; ++j) {

					std::string word;
					getline(line_stream, word, ',');

					if (mapChipTable.contains(word)) {
						mapChipData_.data[index * 6 + i][j] = mapChipTable[word];
					}
				}
			}
		}
		else if(index > 2) {
			
			//ファイルを開く
			std::ifstream file;
			file.open(filePath);
			assert(file.is_open());

			// マップチップCSV
			std::stringstream mapChipCsv;
			//ファイルの内容を文字列ストリームにコピー
			mapChipCsv << file.rdbuf();
			//ファイルを閉じる
			file.close();

			std::string line;

			for (uint32_t i = 0; i < 6; ++i) {

				getline(mapChipCsv, line);

				//1行分の文字列をストリームに変換して解析しやすくする
				std::istringstream line_stream(line);

				for (uint32_t j = 0; j < 18; ++j) {

					std::string word;
					getline(line_stream, word, ',');

					if (mapChipTable.contains(word)) {
						mapChipData_.data[(index * 6) - 3 + i][j] = mapChipTable[word];
					}
				}
			}
		}
	}
}
