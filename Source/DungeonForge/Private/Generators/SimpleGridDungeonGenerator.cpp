// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable All
#include "Generators/SimpleGridDungeonGenerator.h"

#include "Generators/BSPDungeonGenerator.h"
#include "Layouts/SimpleGridDungeonLayout.h"

FDungeonRoom::FDungeonRoom()
{
	GlobalCentre = FGridCoordinate();
	LocalCoordOffsets = {};
}

FDungeonRoom::FDungeonRoom(FGridCoordinate InGlobalCentre, const TSet<FGridCoordinate>& InLocalCoordOffsets)
{
	GlobalCentre = InGlobalCentre;
	LocalCoordOffsets = InLocalCoordOffsets;
}

TSet<FGridCoordinate> FDungeonRoom::GetGlobalCoordOffsets() const
{
	TSet<FGridCoordinate> OutArray;
	for (const FGridCoordinate Coord : LocalCoordOffsets)
	{
		OutArray.Add(Coord+GlobalCentre);
	}
	return OutArray;
}

int FDungeonRoom::MaxManhattanDistanceBetweenRooms(TSet<FGridCoordinate> A, TSet<FGridCoordinate> B)
{
	int MaxA = 0;
	for (const FGridCoordinate C : A)
	{
		MaxA = FMath::Max3(MaxA,abs(C.X),abs(C.Y));
	}
	int MaxB = 0;
	for (const FGridCoordinate C : B)
	{
		MaxB = FMath::Max3(MaxB,abs(C.X),abs(C.Y));
	}
	return 2 * (MaxA + MaxB + 1);
}

bool FDungeonRoom::DoRoomsOverlap(FDungeonRoom A, FDungeonRoom B)
{
	TSet<FGridCoordinate> GlobalCoords;
	for (FGridCoordinate Coord : A.LocalCoordOffsets)
	{
		GlobalCoords.Add(Coord+A.GlobalCentre);
	}

	for (FGridCoordinate Coord : B.LocalCoordOffsets)
	{
		if (GlobalCoords.Contains(Coord+B.GlobalCentre)) return true;
	}
	return false;
}

bool FDungeonRoom::AreRoomsTouching(const FDungeonRoom& A, const FDungeonRoom& B)
{
	if (DoRoomsOverlap(A, B))
	{
		return false;
	}

	TSet<FGridCoordinate> ACoords;
	for (const FGridCoordinate Coord : A.LocalCoordOffsets)
	{
		ACoords.Add(Coord+A.GlobalCentre);
	}

	for (const FGridCoordinate Coord : B.LocalCoordOffsets)
	{
		for (FGridCoordinate AdjacentCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(Coord+B.GlobalCentre, false))
		{
			if (ACoords.Contains(AdjacentCoord))
			{
				return true;
			}
		}
	}
	return false;
}

USimpleGridDungeonLayout* USimpleGridDungeonGenerator::GenerateLayout()
{	
	USimpleGridDungeonLayout* Layout = NewObject<USimpleGridDungeonLayout>();
	
	TArray<FDungeonRoom> RoomLayout = {};
	TSet<FGridCoordinate> RoomLayoutUsedCoords;

	// Add a single room to the layout, needed to place all the rest
	int StartingRoomIndex = 0;
	FDungeonRoom StartingRoom = FDungeonRoom(FGridCoordinate(0,0), PossibleRooms[StartingRoomIndex].LocalCoordOffsets);
	RoomLayout.Add(StartingRoom);
	RoomLayoutUsedCoords.Append(StartingRoom.GetGlobalCoordOffsets());

	TMap<FDungeonRoom, FDungeonRoom> RoomConnections;
	// Place one less than the NumRooms, since we already added the first room
	check(RoomCount >= 1)
	for (int i = 1; i < RoomCount; i++)
	{
		AddSingleRoomToLayout(RoomLayout, RoomLayoutUsedCoords, RoomConnections);
	}
	
	// RoomLayout should be a list of all the rooms in the dungeon. Now we have to convert that to a USimpleGridDungeonLayout
	for (FDungeonRoom Room : RoomLayout)
	{
		TSet<FGridEdge> WallEdges;
		for (FGridCoordinate Coord : Room.LocalCoordOffsets)
		{
			Layout->AddRoomTiles({Coord+Room.GlobalCentre});

			for (FGridCoordinate AdjacentCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(Coord+Room.GlobalCentre))
			{
				if (Room.GetGlobalCoordOffsets().Contains(AdjacentCoord)) continue;
				WallEdges.Add({AdjacentCoord, Coord+Room.GlobalCentre});
			}
		}

		for (const auto& Edge : WallEdges)
		{
			Layout->AddWalls({Edge});
		}
	}

	Layout->bImputesWallPositions = false;

	FlushPersistentDebugLines(GetWorld());
	// Add doors between the rooms
	for (TTuple<FDungeonRoom, FDungeonRoom> Connection : RoomConnections)
	{
		// Find tiles with an adjacent tile in the other room
		TMap<FGridCoordinate, FGridCoordinate> PotentialDoorTiles;
		for (const FGridCoordinate Coord : Connection.Key.GetGlobalCoordOffsets())
		{
			for (const FGridCoordinate AdjacentCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(Coord))
			{
				if (Connection.Value.GetGlobalCoordOffsets().Contains(AdjacentCoord))
				{
					PotentialDoorTiles.Add(Coord, AdjacentCoord);
				}
			}
		}
		// Pick a random tile to place the door
		TArray<FGridCoordinate> Keys;
		PotentialDoorTiles.GetKeys(Keys);
		const FGridCoordinate DoorTile = Keys[FMath::RandRange(0, Keys.Num() - 1)];
		const FGridCoordinate DoorTarget = PotentialDoorTiles[DoorTile];
		Layout->AddDoors({FGridEdge(DoorTile, DoorTarget)});
	}
	
	return Layout;
}

void USimpleGridDungeonGenerator::SetNumRooms(const int32 InRoomCount)
{
	RoomCount = InRoomCount;
	
	// Populate PotentialRooms with some layouts (just squares and rectangles for now)
	PossibleRooms = InitPossibleRooms();
	// Calculate every possible combination of two rooms.
	RoomComboOffsetsMap = GenerateRoomComboOffsets(PossibleRooms);
}

TArray<FDungeonRoom> USimpleGridDungeonGenerator::InitPossibleRooms()
{
	TArray<FDungeonRoom> AllRooms;
	
	//Iterate over every rectangle between MinSize and MaxSize
	constexpr int MinSize = 2;
	constexpr int MaxSize = 4;
	for (int Width = MinSize; Width <= MaxSize; Width++)
	{
		for (int Height = MinSize; Height <= MaxSize; Height++)
		{
			// Instead of just creating a box from 0->Width and 0->Height, we offset
			// the box by half the width and height so the centre of the room is roughly in the middle
			const int W_Neg = -(Width) / 2;
			const int H_Neg = -(Height) / 2;

			// Create the actual room representation
			TSet<FGridCoordinate> RoomOffsetLayout;
			for (int X = W_Neg; X < W_Neg + Width; X++)
			{
				for (int Y = H_Neg; Y < H_Neg + Height; Y++)
				{
					RoomOffsetLayout.Add(FGridCoordinate(X,Y));
				}
			}
			FDungeonRoom RoomRepresentation(FGridCoordinate(0,0), RoomOffsetLayout);
			AllRooms.Add(RoomRepresentation);
		}
	}

	// Create some L shaped rooms
	const TArray<FGridCoordinate> LRoom1Initial = FRectBox(FGridCoordinate(0, 0), FGridCoordinate(1, 1)).GetFillCoordinates();
	const TSet<FGridCoordinate> LRoom1Right = TSet(UGridCoordinateHelperLibrary::Expand(LRoom1Initial, 2, false, 1));
	const TSet<FGridCoordinate> LRoomUp = TSet(UGridCoordinateHelperLibrary::Expand(LRoom1Initial, 2, false, 2));
	const TSet<FGridCoordinate> LRoom1 = LRoom1Right.Union(LRoomUp);
	AllRooms.Add(FDungeonRoom(FGridCoordinate(0,0), LRoom1));
	AllRooms.Add(FDungeonRoom(FGridCoordinate(0,0), UGridCoordinateHelperLibrary::RotateClockwise(LRoom1, 1)));
	AllRooms.Add(FDungeonRoom(FGridCoordinate(0,0), UGridCoordinateHelperLibrary::RotateClockwise(LRoom1, 2)));
	AllRooms.Add(FDungeonRoom(FGridCoordinate(0,0), (UGridCoordinateHelperLibrary::RotateClockwise(LRoom1, 3))));
	
	UE_LOG(LogTemp, Warning, TEXT("Total generated possible rooms: %d"), AllRooms.Num());

	// Since there can be a huge number of possible rooms, we reduce the number of sampled rooms to improve performance
	// TODO set equal to number of rooms
	int MaxRoomsInGen = 12;
	MaxRoomsInGen = FMath::Min(MaxRoomsInGen, AllRooms.Num());

	// Shuffle the rooms
	AllRooms.Sort([](const FDungeonRoom& A, const FDungeonRoom& B) { return FMath::RandBool(); });
	TArray<FDungeonRoom> OutPossibleRooms = {};
	
	// Adds a random selection of rooms into the possible rooms
	for (int i = 0; i < MaxRoomsInGen; i++)
	{
		OutPossibleRooms.Add(AllRooms[i]);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Total sampled possible rooms for actual generation: %d"), OutPossibleRooms.Num());
	return OutPossibleRooms;
}

TMap<TTuple<FDungeonRoom, FDungeonRoom>, TSet<FGridCoordinate>> USimpleGridDungeonGenerator::GenerateRoomComboOffsets(const TArray<FDungeonRoom>& Rooms)
{
	TMap<TTuple<FDungeonRoom, FDungeonRoom>, TSet<FGridCoordinate>> OutRoomComboOffsets = {};
	
	// Iterates over every room, and adds coord offsets from one room to the other
	for (const FDungeonRoom RoomA : Rooms)
	{
		for (const FDungeonRoom RoomB : Rooms)
		{
			// Since we calculate inverses to rooms below, it is possible that the offsets have already been calculated and added to the map.
			if (OutRoomComboOffsets.Contains(TTuple<FDungeonRoom, FDungeonRoom>(RoomA, RoomB))) continue;
			
			const TSet<FGridCoordinate> ItoJOffsets = GenerateOffsetsForRooms(RoomA.LocalCoordOffsets, RoomB.LocalCoordOffsets);
			OutRoomComboOffsets.Add(TTuple<FDungeonRoom, FDungeonRoom>(RoomA, RoomB), ItoJOffsets);
			
			// The offsets for the other room to itself should just be the inverse of its own generated offsets
			TSet<FGridCoordinate> JtoIOffsets;
			for (FGridCoordinate Offset : ItoJOffsets)
			{
				JtoIOffsets.Add(Offset.Inverse());
			}
			OutRoomComboOffsets.Add(TTuple<FDungeonRoom, FDungeonRoom>(RoomB, RoomA), JtoIOffsets);
		}
	}
	
	return OutRoomComboOffsets;
}

TSet<FGridCoordinate> USimpleGridDungeonGenerator::GenerateOffsetsForRooms(const TSet<FGridCoordinate>& RoomA, const TSet<FGridCoordinate>& RoomB)
{
	TSet<FGridCoordinate> OutArray;

	const int MaxBFSRange = FDungeonRoom::MaxManhattanDistanceBetweenRooms(RoomA, RoomB);

	const FDungeonRoom RoomADungeonRoom = FDungeonRoom(FGridCoordinate(), RoomA);
	FDungeonRoom RoomBDungeonRoom = FDungeonRoom(FGridCoordinate(), RoomB);

	TSet<FGridCoordinate> VisitedCoords;
	TQueue<FGridCoordinate> SearchQueue;
	SearchQueue.Enqueue(FGridCoordinate(0,0));
	VisitedCoords.Add(FGridCoordinate(0,0));

	const TSet<FGridCoordinate> RoomACoords = TSet(RoomA);
	int CurrentRange = 0;
	while (CurrentRange <= MaxBFSRange)
	{
		// Take top item from queue
		FGridCoordinate CurrentCoord;
		SearchQueue.Dequeue(CurrentCoord);

		// Only process if current coord not in RoomA
		if (!RoomACoords.Contains(CurrentCoord))
		{
			CurrentRange = CurrentCoord.X + CurrentCoord.Y;
			
			RoomBDungeonRoom.GlobalCentre = CurrentCoord;
			if (FDungeonRoom::AreRoomsTouching(RoomADungeonRoom, RoomBDungeonRoom))
			{
				OutArray.Add(CurrentCoord);
			}
		}

		// Add all neighbors to queue
		for (FGridCoordinate AdjacentCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(CurrentCoord, true))
		{
			if (!VisitedCoords.Contains(AdjacentCoord))
			{
				VisitedCoords.Add(AdjacentCoord);
				SearchQueue.Enqueue(AdjacentCoord);
			}
		}		
	}
	return OutArray;
}

void USimpleGridDungeonGenerator::AddSingleRoomToLayout(TArray<FDungeonRoom>& RoomLayout, TSet<FGridCoordinate>& RoomLayoutUsedCoords, TMap<FDungeonRoom, FDungeonRoom>& RoomConnections) const
{
	// Take a new random room layout
	const int NewRoomIndex = FMath::RandRange(0,PossibleRooms.Num()-1);
	FDungeonRoom NewRoom = PossibleRooms[NewRoomIndex];

	// Find every existing room and their PossibleRooms index
	TMap<FGridCoordinate, FDungeonRoom> PlaceableLocations;
	for (FDungeonRoom ExistingRoom : RoomLayout)
	{
		// Find all placeable points
		const TTuple<FDungeonRoom, FDungeonRoom> MapKey = TTuple<FDungeonRoom, FDungeonRoom>(FDungeonRoom(FGridCoordinate(), ExistingRoom.LocalCoordOffsets), NewRoom);
		for (FGridCoordinate RoomOffset : RoomComboOffsetsMap[MapKey])
		{
			const FGridCoordinate NewRoomGlobalOrigin = ExistingRoom.GlobalCentre + RoomOffset;
			
			// Filter out origins that are already used (quick check because we already have a set)
			if (RoomLayoutUsedCoords.Contains(NewRoomGlobalOrigin)) continue;

			// If none of the coordinates of the placement of the new room are already used, then we can place the new room at this RoomOffset
			const FDungeonRoom NewRoomInGlobalSpace = FDungeonRoom(NewRoomGlobalOrigin, NewRoom.LocalCoordOffsets);
			if (NewRoomInGlobalSpace.GetGlobalCoordOffsets().Intersect(RoomLayoutUsedCoords).Num() == 0)
			{
				PlaceableLocations.Add(NewRoomGlobalOrigin, ExistingRoom);
			}
		}
	}

	// Select a random location to place the new room
	TArray<FGridCoordinate> ListOfSpawnLocations;
	PlaceableLocations.GetKeys(ListOfSpawnLocations);
	const FGridCoordinate RoomCentre = ListOfSpawnLocations[FMath::RandRange(0, ListOfSpawnLocations.Num() - 1)];
	
	NewRoom.GlobalCentre = RoomCentre;
	RoomLayout.Add(NewRoom);
	RoomConnections.Add(NewRoom, PlaceableLocations[RoomCentre]);
	
	// Update global set of coord tiles
	RoomLayoutUsedCoords.Append(NewRoom.GetGlobalCoordOffsets());
}

USimpleGridDungeonLayout* USimpleGridDungeonGenerator::SimpleStaticLayout1()
{
	USimpleGridDungeonLayout* Layout = NewObject<USimpleGridDungeonLayout>();

	const TArray<FGridCoordinate> Room1 = FRectBox(FGridCoordinate(0, 0), FGridCoordinate(4, 2)).GetFillCoordinates();
	Layout->AddRoomTiles(Room1);
	const TArray<FGridCoordinate> Room2 = FRectBox(FGridCoordinate(-5,-1), FGridCoordinate(-2,1)).GetFillCoordinates();
	Layout->AddRoomTiles(Room2);
	const TArray<FGridCoordinate> Room3 = FRectBox(FGridCoordinate(-6,-6), FGridCoordinate(-2,-3)).GetFillCoordinates();
	Layout->AddRoomTiles(Room3);
	const TArray<FGridCoordinate> Room4 = FRectBox(FGridCoordinate(8,-4), FGridCoordinate(12,0)).GetFillCoordinates();
	Layout->AddRoomTiles(Room4);
	const TArray<FGridCoordinate> Room5 = FRectBox(FGridCoordinate(4,-7), FGridCoordinate(6,-5)).GetFillCoordinates();
	Layout->AddRoomTiles(Room5);
	const TArray<FGridCoordinate> Room6 = FRectBox(FGridCoordinate(8,-10), FGridCoordinate(10,-8)).GetFillCoordinates();
	Layout->AddRoomTiles(Room6);
	
	const TArray<FGridCoordinate> Corridor12 = FRectBox(FGridCoordinate(-1,0), FGridCoordinate(-1,0)).GetFillCoordinates();
	Layout->AddCorridorTiles(Corridor12);
	const TArray<FGridCoordinate> Corridor23 = FRectBox(FGridCoordinate(-4,-2), FGridCoordinate(-4,-2)).GetFillCoordinates();
	Layout->AddCorridorTiles(Corridor23);
	const TArray<FGridCoordinate> Corridor145 = FRectBox(FGridCoordinate(6,-4), FGridCoordinate(6,0)).GetFillCoordinates();
	Layout->AddCorridorTiles(Corridor145);
	const TArray<FGridCoordinate> Corridor456 = FRectBox(FGridCoordinate(7,-6), FGridCoordinate(8,-6)).GetFillCoordinates();
	Layout->AddCorridorTiles(Corridor456);
	const TArray<FGridCoordinate> Corridor46 = FRectBox(FGridCoordinate(8,-7), FGridCoordinate(8,-5)).GetFillCoordinates();
	Layout->AddCorridorTiles(Corridor46);
	const TArray<FGridCoordinate> Corridor56 = FRectBox(FGridCoordinate(6,-9), FGridCoordinate(6,-8)).GetFillCoordinates();
	Layout->AddCorridorTiles(Corridor56);
	const TArray<FGridCoordinate> Corridor562 = FRectBox(FGridCoordinate(7,-9), FGridCoordinate(7,-9)).GetFillCoordinates();
	Layout->AddCorridorTiles(Corridor562);

	Layout->AddCorridorTiles({FGridCoordinate(5,0), FGridCoordinate(7,-2)});

	// Add doors between corridor and room boundaries
	for (FGridCoordinate CorridorCoord : Layout->GetCorridorTiles())
	{
		for (FGridCoordinate AdjacentCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(CorridorCoord))
		{
			if (Layout->GetRoomTiles().Contains(AdjacentCoord))
			{
				Layout->AddDoors({FGridEdge(CorridorCoord, AdjacentCoord)});
			}
		}
	}
	
	return Layout;
}
