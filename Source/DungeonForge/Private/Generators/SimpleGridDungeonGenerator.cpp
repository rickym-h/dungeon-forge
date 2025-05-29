// Fill out your copyright notice in the Description page of Project Settings.


#include "Generators/SimpleGridDungeonGenerator.h"

#include "Layouts/SimpleGridDungeonLayout.h"

USimpleGridDungeonGenerator::USimpleGridDungeonGenerator()
{
}

USimpleGridDungeonLayout* USimpleGridDungeonGenerator::GenerateLayout()
{
	USimpleGridDungeonLayout* Layout = NewObject<USimpleGridDungeonLayout>();

	const TArray<FGridTile> Room = {FGridTile(FGridCoordinate(), 500)};
	Layout->AddRoomTiles(Room);

	const TArray<FGridTile> Corridor = {FGridTile(FGridCoordinate(0, 1), 500)};
	Layout->AddCorridorTiles(Corridor);
	
	return Layout;
}
