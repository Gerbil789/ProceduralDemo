#include "ProceduralTerrain/Modifiers/TextureModifier.h"
#include "Engine/Texture.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "Rendering/Texture2DResource.h"

void UTextureModifier::ApplyModifier(float& height, const FVector2D& Location) const
{
  UE_LOG(LogTemp, Warning, TEXT("TextureModifier - NOT IMPLEMENTED."));

  //if (!Texture)
  //{
  //  UE_LOG(LogTemp, Error, TEXT("TextureModifier - Texture is empty."));
  //  return;
  //}

  //Texture->

  //// Access the texture's resource
  //FTexture2DResource* TextureResource = (FTexture2DResource*)Texture->Resource;
  //if (!TextureResource)
  //{
  //  UE_LOG(LogTemp, Warning, TEXT("Invalid texture resource!"));
  //  return;
  //}

  //// Extract the raw data from the texture's mip maps
  //const FTexture2DMipMap& MipMap = Texture->GetResourceData()->Mips[0];  // First MipMap (0 index)
  //const TArray<uint8>& Data = MipMap.BulkData;

  //// Process the raw texture data
  //for (int32 i = 0; i < Data.Num(); i += 4) // Assuming RGBA format (4 bytes per pixel)
  //{
  //  uint8 R = Data[i];
  //  uint8 G = Data[i + 1];
  //  uint8 B = Data[i + 2];
  //  uint8 A = Data[i + 3];

  //  // Output the pixel data
  //  UE_LOG(LogTemp, Log, TEXT("Pixel %d: R=%d, G=%d, B=%d, A=%d"), i / 4, R, G, B, A);
  //}


  //for (int Y = 0; Y < ChunkSize; ++Y)
  //{
  //  for (int X = 0; X < ChunkSize; ++X)
  //  {
  //    float U = (float)X / (float)(ChunkSize - 1);
  //    float V = (float)Y / (float)(ChunkSize - 1);

  //    int32 TextureX = FMath::Clamp(FMath::RoundToInt(U * (TextureWidth - 1)), 0, TextureWidth - 1);
  //    int32 TextureY = FMath::Clamp(FMath::RoundToInt(V * (TextureHeight - 1)), 0, TextureHeight - 1);

  //    int32 PixelIndex = TextureY * TextureWidth + TextureX;
  //    FColor PixelColor = FormattedImageData[PixelIndex];

  //    float GrayValue = (PixelColor.R + PixelColor.G + PixelColor.B) / (3.0f * 255.0f);

  //    int32 HeightMapIndex = Y * ChunkSize + X;

  //    switch (ModifierType)
  //    {
  //    case ETerrainModifierType::Additive:
  //      HeightMap[HeightMapIndex] += GrayValue;
  //      break;

  //    case ETerrainModifierType::Multiplicative:
  //      HeightMap[HeightMapIndex] *= GrayValue;
  //      break;

  //    default:
  //      UE_LOG(LogTemp, Warning, TEXT("TextureModifier - Unknown ModifierType."));
  //      break;
  //    }
  //  }
  //}

  //Mip.BulkData.Unlock();
}
