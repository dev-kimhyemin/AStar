
#include "UI/MainWidget.h"
#include "UI/NodeWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/KismetStringLibrary.h"
#include "Misc/DefaultValueHelper.h"
#include "UI/NodeWidgetTextBlock.h"


void UMainWidget::GenerateMap()
{
	const int Row = GetInputLineNumber(InputRow->GetText().ToString());
	const int Col = GetInputLineNumber(InputCol->GetText().ToString());

	if(Row < 0 || Col < 0 || !NodePanel)
	{
		return;
	}

	InitializeAssignableVariables();
	
	NodeClickedDelegate Delegate;
	Delegate.BindUObject(this, &UMainWidget::OnNodeClicked);
	
	Grid.SetNum(Row);
	for(int i = 0; i < Row; i++)
	{
		Grid[i].SetNum(Col);
		for(int j = 0; j < Col; j++)
		{
			if(UNodeWidget* NewNodeWidget = CreateWidget<UNodeWidget>(this, NodeWidget->GetClass()))
			{
				if(UUniformGridSlot* NodeSlot = NodePanel->AddChildToUniformGrid(NewNodeWidget))
				{
					NodeSlot->SetRow(i);
					NodeSlot->SetColumn(j);
					
					FNode* NewNode = new FNode(NodeWidget, i, j);
					NewNode->WidgetBackgroundImage = NewNodeWidget->GetBackgroundImage();
					NewNode->WidgetFromStartTextBlock = NewNodeWidget->GetFromStartTextBlock();
					NewNode->WidgetToDestinationTextBlock = NewNodeWidget->GetToDestinationTextBlock();
					NewNode->WidgetTotalDistanceTextBlock = NewNodeWidget->GetTotalDistanceTextBlock();
					
					Grid[i][j] = NewNode;
				}

				NewNodeWidget->InitializeNode(Delegate, i, j);
			}
		}
	}
}

void UMainWidget::InitializeAssignableVariables()
{
	NodePanel->ClearChildren();
	Grid.Empty();
	StartNode = nullptr;
	GoalNode = nullptr;
}

int UMainWidget::GetInputLineNumber(const FString& Str)
{
	if(int ParsedValue; UKismetStringLibrary::IsNumeric(Str) && FDefaultValueHelper::ParseInt(Str, ParsedValue))
	{
		if(ParsedValue <= MAX_LINE)
		{
			return ParsedValue;
		}
	}

	return -1;
}

TArray<FNode*> UMainWidget::AStar()
{
	if(StartNode == GoalNode)
	{
		return TArray<FNode*>();
	}
	
	TArray<FNode*> Route;
	TArray<FNode*> OpenList;
	TArray<FNode*> ClosedList;
		
	StartNode->FromStart = 0;
	StartNode->ToDestination = Heuristic(StartNode, GoalNode);
	ClosedList.Emplace(StartNode);

	FNode* CurrentNode = StartNode;

	while(CurrentNode != GoalNode)
	{
		for(const auto& Neighbor : GetNeighbors(CurrentNode))
		{
			if(Neighbor->bIsObstacle || ClosedList.Contains(Neighbor))
			{
				continue;
			}

			if(OpenList.Contains(Neighbor) && CurrentNode->FromStart + 1 > Neighbor->FromStart)
			{
				continue;
			}
			
			Neighbor->FromStart = CurrentNode->FromStart + 1;
			Neighbor->ToDestination = Heuristic(Neighbor, GoalNode);
			Neighbor->ParentIndices = {CurrentNode->Row, CurrentNode->Col};
			
			UpdateNodeWidgetText(Neighbor, Neighbor->FromStart, Neighbor->ToDestination);
			ChangeNodeWidgetColor(Neighbor, ENodeType::Closed);
			
			OpenList.Emplace(Neighbor);
		}

		if(OpenList.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("Goal is unreachable"));
			break;
		}

		FNode* SelectedNode = OpenList[0];
		
		for(int i = 0; i < OpenList.Num(); i++)
		{
			if(OpenList[i]->TotalDistance() < SelectedNode->TotalDistance())
			{
				SelectedNode = OpenList[i];
			}
		}

		OpenList.Remove(SelectedNode);
		ClosedList.Emplace(SelectedNode);
		
		CurrentNode = SelectedNode;
	}

	while(CurrentNode != nullptr)
	{
		Route.Emplace(CurrentNode);

		const int ParentRow = CurrentNode->ParentIndices.X;
		const int ParentCol = CurrentNode->ParentIndices.Y;

		if(ParentRow > 0 && ParentCol > 0)
		{
			CurrentNode = Grid[ParentRow][ParentCol];
		}
		else
		{
			CurrentNode = nullptr;
		}
	}

	return Route;
}

float UMainWidget::Heuristic(const FNode* A, const FNode* B) const
{
	const int Weight = HeuristicSlider->GetValue();
	
	return Weight * (abs(A->Row - B->Row) + abs(A->Col - B->Col));
}

TArray<FNode*> UMainWidget::GetNeighbors(const FNode* Node)
{
	TArray<FNode*> Neighbors;
	
	const TArray<pair<int, int>> Directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
	const int GridRow = Grid.Num();
	const int GridCol = Grid[0].Num();

	for(const auto& [first, second] : Directions)
	{
		const int NextRow = Node->Row + first;
		const int NextCol = Node->Col + second;
		
		if(NextRow >= 0 && NextRow < GridRow && NextCol >= 0 && NextCol < GridCol)
		{
			Neighbors.Emplace(Grid[NextRow][NextCol]);
		}
	}

	return Neighbors;
}

void UMainWidget::ClearNodes()
{
	for(const auto& Row : Grid)
	{
		for(const auto& Cell : Row)
		{
			Cell->FromStart = 0;
			Cell->ToDestination = 0;
			Cell->ParentIndices = {-1, -1};
			
			if(Cell->bIsObstacle)
			{
				continue;
			}
			
			ChangeNodeWidgetColor(Cell,ENodeType::Default);
			UpdateNodeWidgetText(Cell, 0, 0);
		}
	}
}

void UMainWidget::UpdateNodeWidgetText(const FNode* Node, const int InFromStart, const int InToDestination)
{
	Node->WidgetFromStartTextBlock->SetText(FText::FromString(FString::FromInt(InFromStart)));
	Node->WidgetToDestinationTextBlock->SetText(FText::FromString(FString::FromInt(InToDestination)));
	Node->WidgetTotalDistanceTextBlock->SetText(FText::FromString(FString::FromInt(InFromStart + InToDestination)));
}

void UMainWidget::ChangeNodeWidgetColor(const FNode* Node, const ENodeType NodeType)
{
	FLinearColor Color;

	switch (NodeType)
	{
		case ENodeType::Default:
			Color = FLinearColor(0.1f, 0.1f, 0.1f);
			break;
		case ENodeType::Closed:
			Color = FLinearColor(0.5f, 0.25f, 0.15f);
			break;
		case ENodeType::Route:
			Color = FLinearColor(0.1f, 0.5f, 0.25f);
			break;
		case ENodeType::Start:
			Color = FLinearColor(0.1f, 0.1f, 0.5f);
			break;
		case ENodeType::Goal:
			Color = FLinearColor(0.1f, 0.4f, 0.1f);
			break;
		case ENodeType::Obstacle:
			Color = FLinearColor(0.01f, 0.01f, 0.01f);
			break;
	}
	
	Node->WidgetBackgroundImage->SetColorAndOpacity(Color);
}

void UMainWidget::OnNodeClicked(const int Row, const int Col)
{
	FNode* ClickedNode = Grid[Row][Col];

	if(bIsObstacleMode)
	{
		ToggleClickedNodeObstacle(ClickedNode);
		return;
	}

	if(ClickedNode->bIsObstacle)
	{
		return;
	}
	
	if(!StartNode)
	{
		ClearNodes();
		StartNode = ClickedNode;
	}
	else
	{
		GoalNode = ClickedNode;
		SimulateAStar();
	}
}

void UMainWidget::SimulateAStar()
{
	for(TArray<FNode*> SimulatedRoute = AStar(); const auto& Path : SimulatedRoute)
	{
		ENodeType NodeType = ENodeType::Route;

		if(Path == StartNode)
		{
			NodeType = ENodeType::Start;
		}
		else if(Path == GoalNode)
		{
			NodeType = ENodeType::Goal;
		}

		ChangeNodeWidgetColor(Path, NodeType);
	}

	StartNode = nullptr;
	GoalNode = nullptr;
}

void UMainWidget::ToggleClickedNodeObstacle(FNode* Node)
{
	Node->bIsObstacle = !Node->bIsObstacle;

	ChangeNodeWidgetColor(Node, Node->bIsObstacle ? ENodeType::Obstacle : ENodeType::Default);
}

void UMainWidget::LogNode(const FNode* Node, const FString& NodeName)
{
	UE_LOG(LogTemp, Log, TEXT("%s[%d][%d] / FromStart: %d, ToDestination: %d, TotalDistance: %d"), *NodeName, Node->Row, Node->Col, Node->FromStart, Node->ToDestination, Node->TotalDistance());
}
