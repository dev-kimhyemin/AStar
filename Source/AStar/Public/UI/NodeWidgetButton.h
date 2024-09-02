
#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "NodeWidgetButton.generated.h"


UCLASS()
class ASTAR_API UNodeWidgetButton : public UButton
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void OnButtonClicked(bool bIsLeftMouse);
};
