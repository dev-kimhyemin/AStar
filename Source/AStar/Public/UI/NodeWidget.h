
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NodeAttributes.h"
#include "NodeWidget.generated.h"

class UImage;
class UNodeWidgetTextBlock;
class UNodeWidgetButton;
class UButton;
class UTextBlock;

UCLASS()
class ASTAR_API UNodeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeNode(const NodeClickedDelegate& InDelegate, const int InRow, const int InCol)
	{
		Delegate = InDelegate;
		Row = InRow;
		Col = InCol;
	}

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnMouseButtonClicked();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* BackgroundImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UNodeWidgetTextBlock* FromStart;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UNodeWidgetTextBlock* ToDestination;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UNodeWidgetTextBlock* TotalDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UNodeWidgetButton* Button;

private:
	NodeClickedDelegate Delegate;

	int Row;
	int Col;

public:
	UImage* GetBackgroundImage() const {return BackgroundImage;}
	UNodeWidgetTextBlock* GetFromStartTextBlock() const {return FromStart;}
	UNodeWidgetTextBlock* GetToDestinationTextBlock() const {return ToDestination;}
	UNodeWidgetTextBlock* GetTotalDistanceTextBlock() const {return TotalDistance;}
};
