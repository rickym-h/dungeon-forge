// Fill out your copyright notice in the Description page of Project Settings.


#include "Layouts/SimpleGridDungeonLayout.h"

USimpleGridDungeonLayout::USimpleGridDungeonLayout()
{
}

TArray<FGridTile> USimpleGridDungeonLayout::GetRoomTiles() const
{
	return RoomTiles.Array();
}

TArray<FGridTile> USimpleGridDungeonLayout::GetCorridorTiles() const
{
	return CorridorTiles.Array();
}

TArray<FGridTile> USimpleGridDungeonLayout::GetAllFloorTiles() const
{
	TArray<FGridTile> AllTiles = RoomTiles.Array();
	AllTiles.Append(CorridorTiles.Array());
	return AllTiles;

	
}

TArray<FTransform> USimpleGridDungeonLayout::GetDoorPositions(const float TileSize) const
{
	TArray<FTransform> DoorPositions;
	for (const FGridEdge& Door : Doors)
	{
		const FVector PositionA = UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Door.CoordinateA, TileSize);
		const FVector PositionB = UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Door.CoordinateB, TileSize);
		const FVector MidPoint = (PositionA + PositionB) / 2.0f;
		DoorPositions.Add(FTransform(MidPoint));
	}
	return DoorPositions;
}

TArray<FGridEdge> USimpleGridDungeonLayout::GetWallPositions(const float TileSize) const
{
	TSet<FGridEdge> WallPositions;
	
	// ReSharper disable once CppTooWideScopeInitStatement
	TSet<FGridCoordinate> AllCoordinates = UGridCoordinateHelperLibrary::GetAllCoordinates(GetAllFloorTiles());
	for (const FGridCoordinate& Coord : AllCoordinates)
	{
		for (const FGridCoordinate& NeighbourCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(Coord))
		{
			if (!AllCoordinates.Contains(NeighbourCoord))
			{
				WallPositions.Add(FGridEdge(Coord, NeighbourCoord, false));
			}
		}
	}

	return WallPositions.Array();
}

