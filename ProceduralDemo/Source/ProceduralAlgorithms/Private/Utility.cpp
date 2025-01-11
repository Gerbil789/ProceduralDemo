#include "Utility.h"
#include "Misc/MessageDialog.h"

void Utility::ShowMessage(const FString& Message, const FString& Title, EAppMsgType::Type MessageType)
{
	FMessageDialog::Open(MessageType, FText::FromString(Message), FText::FromString(Title));
}