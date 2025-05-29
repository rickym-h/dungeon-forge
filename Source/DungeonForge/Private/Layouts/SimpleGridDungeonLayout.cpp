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

TArray<FGridEdge> USimpleGridDungeonLayout::GetDoorPositions(const float GridSize) const
{
	TArray<FGridEdge> DoorPositions = Doors.Array();
	return DoorPositions;
}

TArray<FGridEdge> USimpleGridDungeonLayout::GetWallPositions(const float GridSize) const
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

void USimpleGridDungeonLayout::AddRoomTiles(const TArray<FGridTile>& InRoomTiles)
{
	this->RoomTiles.Append(InRoomTiles);
}

void USimpleGridDungeonLayout::AddCorridorTiles(const TArray<FGridTile>& InCorridorTiles)
{
	this->CorridorTiles.Append(InCorridorTiles);
}

void USimpleGridDungeonLayout::AddDoors(const TArray<FGridEdge>& InDoorLocations)
{
	this->Doors.Append(InDoorLocations);
}

