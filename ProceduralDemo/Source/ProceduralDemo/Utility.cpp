#include "Utility.h"

FString Utility::CleanName(const FString& FullName)
{
    int32 PrefixIndex = FullName.Find(TEXT("__"));
    int32 SuffixIndex = FullName.Find(TEXT("_C"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);

    if (PrefixIndex != INDEX_NONE && SuffixIndex != INDEX_NONE)
    {
        // Extract the substring between the end of '__' and start of '_C'
        return FullName.Mid(PrefixIndex + 2, SuffixIndex - PrefixIndex - 2);
    }
    
    return FullName; // Return original if format wasn't matched
}
