// Fill out your copyright notice in the Description page of Project Settings.


#include "Layouts/SimpleGridDungeonLayout.h"

USimpleGridDungeonLayout::USimpleGridDungeonLayout()
{
}

TArray<FGridCoordinate> USimpleGridDungeonLayout::GetRoomTiles() const
{
	return RoomTiles.Array();
}

TArray<FGridCoordinate> USimpleGridDungeonLayout::GetCorridorTiles() const
{
	return CorridorTiles.Array();
}

TArray<FGridCoordinate> USimpleGridDungeonLayout::GetAllFloorTiles() const
{
	TArray<FGridCoordinate> AllTiles = RoomTiles.Array();
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
	if (!bImputesWallPositions)
	{
		// Make sure to exclude any possible door tiles that may overlap with the wall tiles.
		return Walls.Difference(Doors).Array();
	}
	
	TSet<FGridEdge> WallPositions;

	// Find all the coordinates that are adjacent to a floor tile and add a wall between them if the adjacent tile is not a floor tile.
	TSet<FGridCoordinate> AllCoordinates = TSet<FGridCoordinate>(GetAllFloorTiles());
	for (const FGridCoordinate& Coord : AllCoordinates)
	{
		for (const FGridCoordinate& NeighbourCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(Coord))
		{
			if (!AllCoordinates.Contains(NeighbourCoord))
			{
				WallPositions.Add(FGridEdge(Coord, NeighbourCoord));
			}
		}
	}

	return WallPositions.Array();
}

TArray<FGridCorner> USimpleGridDungeonLayout::GetCornerPillarPositions(const float GridSize) const
{
	if (!bImputesCornerPillarPositions)
	{
		return CornerPillars.Array();
	}

	TArray<FGridCorner> CornerPillarPositions;

	const TArray<FGridEdge> AllWalls = GetWallPositions(GridSize);

	for (const FGridEdge EdgeA : AllWalls)
	{
		for (const FGridEdge EdgeB : AllWalls)
		{
			if (EdgeA.FormsCorner(EdgeB))
			{
				CornerPillarPositions.Add(FGridCorner::FromEdges(EdgeA, EdgeB));
			}
		}
	}

	return CornerPillarPositions;
}

void USimpleGridDungeonLayout::AddRoomTiles(const TArray<FGridCoordinate>& InRoomTiles)
{
	this->RoomTiles.Append(InRoomTiles);
}

void USimpleGridDungeonLayout::AddCorridorTiles(const TArray<FGridCoordinate>& InCorridorTiles)
{
	this->CorridorTiles.Append(InCorridorTiles);
}

void USimpleGridDungeonLayout::AddWalls(const TArray<FGridEdge>& InWallLocations)
{
	this->Walls.Append(InWallLocations);
}

void USimpleGridDungeonLayout::AddDoors(const TArray<FGridEdge>& InDoorLocations)
{
	this->Doors.Append(InDoorLocations);
}

