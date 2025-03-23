#include "ProceduralTerrain/Modifiers/TextureModifier.h"
#include "Engine/Texture.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "Rendering/Texture2DResource.h"


void UTextureModifier::ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const
{
  if (HeightMap.IsEmpty())
  {
    UE_LOG(LogTemp, Error, TEXT("TextureModifier - Heightmap is empty."));
    return;
  }

  if (!Texture)
  {
    UE_LOG(LogTemp, Error, TEXT("TextureModifier - Texture is empty."));
    return;
  }

  FTexturePlatformData* PlatformData = Texture->GetPlatformData();
  if (!PlatformData || PlatformData->Mips.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("TextureModifier - Invalid texture platform data."));
    return;
  }

  FTexture2DMipMap& Mip = PlatformData->Mips[0];
  int32 TextureWidth = Mip.SizeX;
  int32 TextureHeight = Mip.SizeY;

  void* Data = Mip.BulkData.Lock(LOCK_READ_ONLY);
  if (!Data)
  {
    UE_LOG(LogTemp, Error, TEXT("TextureModifier - Failed to lock texture data."));
    return;
  }

  FColor* FormattedImageData = static_cast<FColor*>(Data);

  for (int Y = 0; Y < ChunkSize; ++Y)
  {
    for (int X = 0; X < ChunkSize; ++X)
    {
      float U = (float)X / (float)(ChunkSize - 1);
      float V = (float)Y / (float)(ChunkSize - 1);

      int32 TextureX = FMath::Clamp(FMath::RoundToInt(U * (TextureWidth - 1)), 0, TextureWidth - 1);
      int32 TextureY = FMath::Clamp(FMath::RoundToInt(V * (TextureHeight - 1)), 0, TextureHeight - 1);

      int32 PixelIndex = TextureY * TextureWidth + TextureX;
      FColor PixelColor = FormattedImageData[PixelIndex];

      float GrayValue = (PixelColor.R + PixelColor.G + PixelColor.B) / (3.0f * 255.0f);

      int32 HeightMapIndex = Y * ChunkSize + X;

      switch (ModifierType)
      {
      case ETerrainModifierType::Additive:
        HeightMap[HeightMapIndex] += GrayValue;
        break;

      case ETerrainModifierType::Multiplicative:
        HeightMap[HeightMapIndex] *= GrayValue;
        break;

      default:
        UE_LOG(LogTemp, Warning, TEXT("TextureModifier - Unknown ModifierType."));
        break;
      }
    }
  }

  Mip.BulkData.Unlock();
}
