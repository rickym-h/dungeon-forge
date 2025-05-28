// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Instances/SimpleGridDungeonInstance.h"
#include "Layouts/SimpleGridDungeonLayout.h"
#include "UObject/Object.h"
#include "SimpleGridDungeonGenerator.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONFORGE_API USimpleGridDungeonGenerator : public UObject
{
	GENERATED_BODY()
	
	USimpleGridDungeonGenerator();

public:
	
	/**
	 * A very simple dungeon generator which keeps adding rooms in an arbitrary position to the layout until it reaches a certain room count.
	 * @return The generated layout.
	 */
	UFUNCTION(BlueprintCallable)
	USimpleGridDungeonLayout* GenerateLayout();
};
