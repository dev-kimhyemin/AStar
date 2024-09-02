#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_TwoParams(NodeClickedDelegate, const int, const int)

UENUM()
enum class ENodeType : uint8
{
	Default,
	Closed,
	Route,
	Start,
	Goal,
	Obstacle,
};

class UNodeWidget;

class ASTAR_API NodeAttributes
{
public:
	NodeClickedDelegate OnNodeClicked;

	void TriggerDelegate(const int Row, const int Col) const
	{
		if(OnNodeClicked.IsBound())
		{
			OnNodeClicked.Execute(Row, Col);
		}
	}
};
