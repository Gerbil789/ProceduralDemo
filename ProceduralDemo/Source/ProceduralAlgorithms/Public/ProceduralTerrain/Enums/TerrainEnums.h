#pragma once

UENUM(BlueprintType)
enum class EMeshStrategy : uint8
{
	Default			UMETA(DisplayName = "Default"),
	QuadTree UMETA(DisplayName = "QuadTrees"),
	VertexClustering UMETA(DisplayName = "VertexClustering"),
	QuadraticErrorMetrics UMETA(DisplayName = "QEM")
};

UENUM(BlueprintType)
enum class ETerrainChunkSize : uint8
{
	Size_8 = 0		UMETA(DisplayName = "8"),
	Size_16 = 1   UMETA(DisplayName = "16"),
	Size_32 = 2   UMETA(DisplayName = "32"),
	Size_64 = 3   UMETA(DisplayName = "64"),
	Size_128 = 4	UMETA(DisplayName = "128"),
	Size_256 = 5	UMETA(DisplayName = "256")
};

static int32 GetSize(ETerrainChunkSize Size)
{
	static const int32 Sizes[] = { 8, 16, 32, 64, 128, 256 };
	return Sizes[(uint8)Size];
}

UENUM(BlueprintType)
enum class ETerrainQuadSize : uint8
{
	Size_16 = 0     UMETA(DisplayName = "16"),
	Size_32 = 1     UMETA(DisplayName = "32"),
	Size_64 = 2     UMETA(DisplayName = "64"),
	Size_128 = 3    UMETA(DisplayName = "128"),
	Size_256 = 4    UMETA(DisplayName = "256"),
	Size_512 = 5    UMETA(DisplayName = "512"),
	Size_1024 = 6   UMETA(DisplayName = "1024")
};

static int32 GetSize(ETerrainQuadSize Size)
{
	static const int32 Sizes[] = { 16, 32, 64, 128, 256, 512, 1024 };
	return Sizes[(uint8)Size];
}
