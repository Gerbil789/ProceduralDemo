#include "ProceduralTerrain/ProceduralTerrain.h"

AProceduralTerrain::AProceduralTerrain()
{
	//PrimaryActorTick.bCanEverTick = true;
	//Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	//RootComponent = Mesh;
	//NoiseOctaves.Add(FNoiseSettings());
}

//void AProceduralTerrain::GenerateChunkAsync(FIntPoint ChunkCoordinates, int32 ChunkSize)
//{
//  //if (!Mesh)
//  //{
//  //  UE_LOG(LogTemp, Error, TEXT("ProceduralMeshComponent is null!"));
//  //  return;
//  //}
//
//  //AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, ChunkCoordinates]()
//  //  {
//  //    TArray<FVector> Vertices;
//  //    TArray<int32> Triangles;
//  //    TArray<FVector2D> UVs;
//  //    TArray<FVector> Normals;
//  //    TArray<FProcMeshTangent> Tangents;
//
//  //    float StartX = ChunkCoordinates.X * (ChunkSize - 1) * QuadSize;
//  //    float StartY = ChunkCoordinates.Y * (ChunkSize - 1) * QuadSize;
//
//  //    Vertices.Reserve(ChunkSize * ChunkSize);
//  //    Triangles.Reserve((ChunkSize - 1) * (ChunkSize - 1) * 6);
//  //    UVs.Reserve(ChunkSize * ChunkSize);
//  //    Normals.Reserve(ChunkSize * ChunkSize);
//  //    Tangents.Reserve(ChunkSize * ChunkSize);
//
//		//	FProcMeshSection* MeshSection = new FProcMeshSection();
//
//  //    for (int32 y = 0; y < ChunkSize; y++)
//  //    {
//  //      for (int32 x = 0; x < ChunkSize; x++)
//  //      {
//  //        float WorldX = StartX + x * QuadSize;
//  //        float WorldY = StartY + y * QuadSize;
//
//  //        float Height = 0.0f;
//  //        float CurrentFrequency = 1.0f;
//  //        float CurrentAmplitude = 1.0f;
//  //        for (const FNoiseSettings& Octave : NoiseOctaves)
//  //        {
//  //          FVector2D NoiseInput = FVector2D(WorldX, WorldY) * ((Octave.Frequency / 1000) * CurrentFrequency);
//  //          NoiseInput += Octave.Offset; // Add per-octave offset
//
//  //          Height += FMath::PerlinNoise2D(NoiseInput) * (Octave.Amplitude * CurrentAmplitude);
//
//  //          // Adjust frequency and amplitude for the next octave
//  //          CurrentFrequency *= Octave.Lacunarity;
//  //          CurrentAmplitude *= Octave.Persistence;
//  //        }
//
//  //        Vertices.Add(FVector(WorldX, WorldY, Height));
//
//  //        if (x < ChunkSize - 1 && y < ChunkSize - 1)
//  //        {
//  //          int32 i = y * ChunkSize + x;
//  //          Triangles.Add(i);
//  //          Triangles.Add(i + ChunkSize);
//  //          Triangles.Add(i + 1);
//  //          Triangles.Add(i + ChunkSize);
//  //          Triangles.Add(i + ChunkSize + 1);
//  //          Triangles.Add(i + 1);
//  //        }
//
//  //        UVs.Add(FVector2D(x / (float)(ChunkSize - 1), y / (float)(ChunkSize - 1)));
//  //        Normals.Add(FVector(0, 0, 1));
//  //        Tangents.Add(FProcMeshTangent(1, 0, 0));
//  //      }
//  //    }
//
//  //    int32 SectionIndex = GetChunkId(ChunkCoordinates);
//
//  //    AsyncTask(ENamedThreads::GameThread, [this, ChunkCoordinates, SectionIndex, Vertices = MoveTemp(Vertices), Triangles = MoveTemp(Triangles), UVs = MoveTemp(UVs), Normals = MoveTemp(Normals), Tangents = MoveTemp(Tangents)]()
//  //      {
//  //        Mesh->CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
//
//		//			if (TerrainMaterial)
//		//			{
//		//				Mesh->SetMaterial(SectionIndex, TerrainMaterial);
//		//			}
//
//
//  //      });
//
//  //    UE_LOG(LogTemp, Warning, TEXT("Chunk Generated Async: %d, %d"), ChunkCoordinates.X, ChunkCoordinates.Y);
//  //  });
// 
//}


int32 AProceduralTerrain::GetChunkId(FIntPoint Chunk)
{
	//FScopeLock Lock(&CriticalSectionLock); // Lock the critical section, auto unlocks when out of scope
 // int32 Id = ChunkIds.Num();

	//while (ChunkIds.Contains(Id))
	//{
	//	Id++;
	//}
	//
	//ChunkIds.Add(Id);
 // LoadedChunks.Add(Chunk, Id);

 // return Id;

	return 0;
}

