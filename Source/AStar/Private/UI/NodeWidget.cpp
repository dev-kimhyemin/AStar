
#include "UI/NodeWidget.h"
#include "UI/NodeWidgetButton.h"
#include "UI/NodeWidgetTextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"


void UNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(!BackgroundImage)
	{
		BackgroundImage = Cast<UImage>(WidgetTree->FindWidget(TEXT("BackgroundImage")));
	}

	if (!FromStart)
	{
		FromStart = Cast<UNodeWidgetTextBlock>(WidgetTree->FindWidget(TEXT("FromStart")));
	}
	
	if (!ToDestination)
	{
		ToDestination = Cast<UNodeWidgetTextBlock>(WidgetTree->FindWidget(TEXT("ToDestination")));
	}
	
	if (!TotalDistance)
	{
		TotalDistance = Cast<UNodeWidgetTextBlock>(WidgetTree->FindWidget(TEXT("TotalDistance")));
	}
	
	if(!Button)
	{
		Button = Cast<UNodeWidgetButton>(WidgetTree->FindWidget(TEXT("Button")));
	}
}

void UNodeWidget::OnMouseButtonClicked()
{
	if(Delegate.IsBound())
	{
		Delegate.Execute(Row, Col);
	}
}
