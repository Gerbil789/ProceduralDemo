#include "Utility.h"

FString Utility::CleanName(FString Name)
{
    // Remove "default__" prefix (if present)
    if (Name.StartsWith(TEXT("default__")))
    {
        Name = Name.RightChop(9); // Remove "default__" (length is 9 characters)
    }

    // Remove "_C" suffix (if present)
    if (Name.EndsWith(TEXT("_C")))
    {
        Name = Name.LeftChop(2); // Remove "_C" (length is 2 characters)
    }

    return Name;
}
