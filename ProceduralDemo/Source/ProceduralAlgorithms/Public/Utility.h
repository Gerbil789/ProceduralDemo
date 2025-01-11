#pragma once

#include "CoreMinimal.h"

class PROCEDURALALGORITHMS_API Utility
{
public:
	static void ShowMessage(const FString &Message, const FString& Title, EAppMsgType::Type MessageType = EAppMsgType::Ok);
};
