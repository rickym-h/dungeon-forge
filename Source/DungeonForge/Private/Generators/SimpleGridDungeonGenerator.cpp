// Fill out your copyright notice in the Description page of Project Settings.


#include "Generators/SimpleGridDungeonGenerator.h"

#include "Layouts/SimpleGridDungeonLayout.h"

USimpleGridDungeonGenerator::USimpleGridDungeonGenerator()
{
}

USimpleGridDungeonLayout* USimpleGridDungeonGenerator::GenerateLayout()
{
	USimpleGridDungeonLayout* Layout = NewObject<USimpleGridDungeonLayout>();

	const TArray<FGridCoordinate> Room = {FGridCoordinate()};
	Layout->AddRoomTiles(Room);

	const TArray<FGridCoordinate> Corridor = {FGridCoordinate(0,1)};
	Layout->AddCorridorTiles(Corridor);
	
	return Layout;
}
