// Fill out your copyright notice in the Description page of Project Settings.


#include "Generators/BSPDungeonGenerator.h"

#include "Layouts/SimpleGridDungeonLayout.h"

FRectBox::FRectBox()
{
}

FRectBox::FRectBox(const FGridCoordinate InBoxOrigin, const FGridCoordinate InBoxBound)
{
	BoxOrigin = InBoxOrigin;
	BoxBound = InBoxBound;
}

int32 FRectBox::GetVolume() const
{
	return (BoxBound.X - BoxOrigin.X + 1) * (BoxBound.Y - BoxOrigin.Y + 1);
}

TArray<FGridCoordinate> FRectBox::GetFillCoordinates() const
{
	TArray<FGridCoordinate> ReturnCoordinates;

	for (int32 X = BoxOrigin.X; X <= BoxBound.X; X++)
	{
		for (int32 Y = BoxOrigin.Y; Y <= BoxBound.Y; Y++)
		{
			ReturnCoordinates.Add(FGridCoordinate(X, Y));
		}
	}

	return ReturnCoordinates;
}

USimpleGridDungeonLayout* UBSPDungeonGenerator::GenerateLayout()
{
	USimpleGridDungeonLayout* Layout = NewObject<USimpleGridDungeonLayout>();

	// Init a single large dungeon room
	// Hardcoded to 30x30 for now, but will be configurable later
	TArray<FRectBox> Rooms = {FRectBox(FGridCoordinate(), FGridCoordinate(9, 9))};

	constexpr int32 CorridorLength = 1;
	constexpr int32 MinRoomWidth = 2;

	constexpr int32 RoomCount = 16;

	for (int i = 1; i < RoomCount; i++)
	{
		// Get the room with the largest width
		Rooms.Sort([](const FRectBox& A, const FRectBox& B)
		{
			return (FMath::Max(A.BoxBound.X - A.BoxOrigin.X, A.BoxBound.Y - A.BoxOrigin.Y)) < (FMath::Max(B.BoxBound.X - B.BoxOrigin.X, B.BoxBound.Y - B.BoxOrigin.Y));
		});
		FRectBox LargestRoom = Rooms.Last();

		// Get possible splits of this room
		TArray<TArray<FRectBox>> PossibleConfigurations = GetTwoRandomPossibleBoxSplits(LargestRoom, CorridorLength, MinRoomWidth);

		if (PossibleConfigurations.Num() == 0) break;

		// Select a random split
		PossibleConfigurations.Sort([](const TArray<FRectBox>& A, const TArray<FRectBox>& B) { return FMath::RandBool(); });
		TArray<FRectBox> ChosenConfigurations = PossibleConfigurations.Pop();
		
		Rooms.Pop();
		Rooms.Append(ChosenConfigurations);
	}

	for (FRectBox Room : Rooms)
	{
		Layout->AddRoomTiles(Room.GetFillCoordinates());
	}
	
	return Layout;
}

TArray<TArray<FRectBox>> UBSPDungeonGenerator::GetAllPossibleBoxSplits(const FRectBox& Box, const int32 CorridorLength, const int32 MinRoomWidth)
{
	check(CorridorLength > 0);
	check(MinRoomWidth > 0);
	TArray<TArray<FRectBox>> ReturnConfigurations;

	// Iterate over every possible vertical split
	for (int32 i = Box.BoxOrigin.X+MinRoomWidth; i <= Box.BoxBound.X - MinRoomWidth - (CorridorLength-1); i++)
	{
		// Split the box at this point
		FRectBox BoxA = FRectBox(FGridCoordinate(Box.BoxOrigin.X, Box.BoxOrigin.Y), FGridCoordinate(i-1, Box.BoxBound.Y));
		FRectBox BoxB = FRectBox(FGridCoordinate(i+1, Box.BoxOrigin.Y), FGridCoordinate(Box.BoxBound.X, Box.BoxBound.Y));

		ReturnConfigurations.Add({BoxA, BoxB});
	}

	// Iterate over every possible horizontal split
	for (int32 i = Box.BoxOrigin.Y+MinRoomWidth; i <= Box.BoxBound.Y - MinRoomWidth - (CorridorLength-1); i++)
	{
		// Split the box at this point
		FRectBox BoxA = FRectBox(FGridCoordinate(Box.BoxOrigin.X, Box.BoxOrigin.Y), FGridCoordinate(Box.BoxBound.X, i-1));
		FRectBox BoxB = FRectBox(FGridCoordinate(Box.BoxOrigin.X, i+1), FGridCoordinate(Box.BoxBound.X, Box.BoxBound.Y));
		
		ReturnConfigurations.Add({BoxA, BoxB});
	}

	return ReturnConfigurations;
}

TArray<TArray<FRectBox>> UBSPDungeonGenerator::GetTwoRandomPossibleBoxSplits(const FRectBox& Box, const int32 CorridorLength, const int32 MinRoomWidth)
{
	check(CorridorLength > 0);
	check(MinRoomWidth > 0);
	TArray<TArray<FRectBox>> ReturnConfigurations;

	// Iterate over every possible vertical split
	const int32 MinX = Box.BoxOrigin.X + MinRoomWidth;
	const int32 MaxX = Box.BoxBound.X - MinRoomWidth - (CorridorLength-1);
	if (MinX <= MaxX)
	{
		const int32 SelectedX = FMath::RandRange(MinX, MaxX);
		// Split the box at this point
		FRectBox BoxA = FRectBox(FGridCoordinate(Box.BoxOrigin.X, Box.BoxOrigin.Y), FGridCoordinate(SelectedX-1, Box.BoxBound.Y));
		FRectBox BoxB = FRectBox(FGridCoordinate(SelectedX+1, Box.BoxOrigin.Y), FGridCoordinate(Box.BoxBound.X, Box.BoxBound.Y));

		ReturnConfigurations.Add({BoxA, BoxB});
	}

	// Iterate over every possible horizontal split
	const int32 MinY = Box.BoxOrigin.Y + MinRoomWidth;
	const int32 MaxY = Box.BoxBound.Y - MinRoomWidth - (CorridorLength-1);
	if (MinY <= MaxY)
	{
		const int32 SelectedY = FMath::RandRange(MinY, MaxY);
		// Split the box at this point
		FRectBox BoxC = FRectBox(FGridCoordinate(Box.BoxOrigin.X, Box.BoxOrigin.Y), FGridCoordinate(Box.BoxBound.X, SelectedY-1));
		FRectBox BoxD = FRectBox(FGridCoordinate(Box.BoxOrigin.X, SelectedY+1), FGridCoordinate(Box.BoxBound.X, Box.BoxBound.Y));
	
		ReturnConfigurations.Add({BoxC, BoxD});
	}

	return ReturnConfigurations;
}