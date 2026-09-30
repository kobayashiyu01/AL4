#include "KamataEngine.h"
#pragma once
enum class MapChipType {

	kBlank, // 空白
	kBlock, // ブロック
};

class MapChipField {

	struct MapChipData {

		std::vector<std::vector<MapChipType>> data;
	};
	MapChipData mapChipData_;

	

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

public:
	
	struct IndexSet {
		int32_t xIndex;
		int32_t yIndex;
	};
	
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapchipTypeByIndex(int32_t xIndex, int32_t yIndex);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical()
	{ return kNumBlockVirtical;
	}
	uint32_t GetNumBlockHrizontal()
	{ return kNumBlockHorizontal;
	}

};
