#pragma once

UENUM(BlueprintType)
enum class EMeshStrategy : uint8
{
	Default			UMETA(DisplayName = "Default"),
	QuadTree UMETA(DisplayName = "QuadTrees"),
	VertexClustering UMETA(DisplayName = "VertexClustering"),
	QuadraticErrorMetrics UMETA(DisplayName = "QEM")
};