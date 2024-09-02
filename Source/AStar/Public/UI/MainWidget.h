
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NodeAttributes.h"
#include "MainWidget.generated.h"

#define MAX_LINE 13

class USlider;
class UImage;
class UNodeWidgetTextBlock;
class UOverlay;
class UNodeWidget;
class UUniformGridPanel;
class UEditableTextBox;

using namespace std;


USTRUCT()
struct FNode
{
	GENERATED_BODY()
	
	UPROPERTY()
	UNodeWidget* Widget;
	
	UPROPERTY()
	UImage* WidgetBackgroundImage = nullptr;

	UPROPERTY()
	UNodeWidgetTextBlock* WidgetFromStartTextBlock = nullptr;

	UPROPERTY()
	UNodeWidgetTextBlock* WidgetToDestinationTextBlock = nullptr;

	UPROPERTY()
	UNodeWidgetTextBlock* WidgetTotalDistanceTextBlock = nullptr;
	
	int Row;
	int Col;
	int FromStart;
	int ToDestination;
	FIntVector2 ParentIndices;
	bool bIsObstacle;

	FNode(UNodeWidget* Widget, const int Row, const int Col, const FIntVector2 ParentIndices = {-1, -1})
		: Widget(Widget), Row(Row), Col(Col), FromStart(0), ToDestination(0), ParentIndices(ParentIndices), bIsObstacle(false) {}
	
	FNode() : Widget(nullptr), Row(0), Col(0), FromStart(0), ToDestination(0), ParentIndices({-1, -1}), bIsObstacle(false) {}

	int TotalDistance() const
	{
		return FromStart + ToDestination;
	}
};


UCLASS()
class ASTAR_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUniformGridPanel* NodePanel;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableTextBox* InputRow;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableTextBox* InputCol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* HeuristicSlider;

	UPROPERTY(EditDefaultsOnly)
	UNodeWidget* NodeWidget;
	

private:
	UFUNCTION(BlueprintCallable)
	void GenerateMap();

	UFUNCTION(BlueprintCallable)
	void ChangeMode(const bool bIsClicked)
	{
		bIsObstacleMode = bIsClicked;
	}
	
	void InitializeAssignableVariables();
	static int GetInputLineNumber(const FString& Str);
	
	TArray<FNode*> AStar();
	float Heuristic(const FNode* A, const FNode* B) const;
	TArray<FNode*> GetNeighbors(const FNode* Node);
	
	void ClearNodes();
	static void UpdateNodeWidgetText(const FNode* Node, const int InFromStart, const int InToDestination);
	static void ChangeNodeWidgetColor(const FNode* Node, ENodeType NodeType);

	void OnNodeClicked(const int Row, const int Col);
	void SimulateAStar();
	static void ToggleClickedNodeObstacle(FNode* Node);

	static void LogNode(const FNode* Node, const FString& NodeName);

	TArray<TArray<FNode*>> Grid;
	FNode* StartNode;
	FNode* GoalNode;

	bool bIsObstacleMode = false;
};
