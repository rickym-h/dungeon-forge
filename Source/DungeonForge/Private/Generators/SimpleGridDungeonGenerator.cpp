// Fill out your copyright notice in the Description page of Project Settings.


#include "Generators/SimpleGridDungeonGenerator.h"

#include "Generators/BSPDungeonGenerator.h"
#include "Layouts/SimpleGridDungeonLayout.h"

FDungeonRoom::FDungeonRoom()
{
	GlobalCentre = FGridCoordinate();
	LocalCoordOffsets = {};
	PossibleRoomsIndex = -1;
}

FDungeonRoom::FDungeonRoom(FGridCoordinate InGlobalCentre, const TArray<FGridCoordinate>& InLocalCoordOffsets)
{
	GlobalCentre = InGlobalCentre;
	LocalCoordOffsets = InLocalCoordOffsets;
	PossibleRoomsIndex = -1;
}

FDungeonRoom::FDungeonRoom(FGridCoordinate InGlobalCentre, const TArray<FGridCoordinate>& InLocalCoordOffsets, int InPossibleRoomsIndex)
{
	GlobalCentre = InGlobalCentre;
	LocalCoordOffsets = InLocalCoordOffsets;
	PossibleRoomsIndex = InPossibleRoomsIndex;
}

TArray<FGridCoordinate> FDungeonRoom::GetGlobalCoordOffsets() const
{
	TArray<FGridCoordinate> OutArray;
	for (const FGridCoordinate Coord : LocalCoordOffsets)
	{
		OutArray.Add(Coord+GlobalCentre);
	}
	return OutArray;
}

int FDungeonRoom::MaxManhattanDistanceBetweenRooms(TArray<FGridCoordinate> A, TArray<FGridCoordinate> B)
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

USimpleGridDungeonGenerator::USimpleGridDungeonGenerator()
{
}

USimpleGridDungeonLayout* USimpleGridDungeonGenerator::GenerateLayout(const int32 NumRooms)
{	
	USimpleGridDungeonLayout* Layout = NewObject<USimpleGridDungeonLayout>();
	
	// Populate PotentialRooms with some layouts (just squares and rectangles for now)
	// TODO refactor to do this when parameters are updated
	TArray<TArray<FGridCoordinate>> PossibleRooms = InitPossibleRooms();
	
	// Calculate every possible combination of two rooms.
	const FDateTime StartTime = FDateTime::UtcNow();
	const TArray<TArray<TArray<FGridCoordinate>>> RoomComboOffsets = GenerateRoomComboOffsets(PossibleRooms);
	const float TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogTemp, Display, TEXT("Generated Room Offsets in %fms"), TimeElapsedInMs)

	TArray<FDungeonRoom> RoomLayout = {};
	TSet<FGridCoordinate> RoomLayoutUsedCoords;

	// Add a single room to the layout, needed to place all the rest
	int HardCodedRoom1Index = 0;
	RoomLayout.Add({FGridCoordinate(0,0), PossibleRooms[HardCodedRoom1Index], HardCodedRoom1Index});
	for (FGridCoordinate Coord : PossibleRooms[HardCodedRoom1Index])
	{
		RoomLayoutUsedCoords.Add(Coord);
	}

	TMap<FDungeonRoom, FDungeonRoom> RoomConnections;
	// Place one less than the NumRooms, since we already added the first room
	for (int i = 1; i < NumRooms; i++)
	{
		AddSingleRoomToLayout(RoomComboOffsets, PossibleRooms, RoomLayout, RoomLayoutUsedCoords, RoomConnections);
	}
	
	// RoomLayout should be a list of all the rooms in the dungeon. Now we have to convert that to a USimpleGridDungeonLayout
	for (FDungeonRoom Room : RoomLayout)
	{
		TMap<FGridCoordinate,FGridCoordinate> WallTiles;
		for (FGridCoordinate Coord : Room.LocalCoordOffsets)
		{
			Layout->AddRoomTiles({Coord+Room.GlobalCentre});

			for (FGridCoordinate AdjacentCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(Coord+Room.GlobalCentre))
			{
				if (Room.GetGlobalCoordOffsets().Contains(AdjacentCoord)) continue;
				WallTiles.Add(AdjacentCoord, Coord+Room.GlobalCentre);
			}
		}

		for (const auto& [Key, Value] : WallTiles)
		{
			Layout->AddWalls({FGridEdge(Key, Value)});
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

TArray<TArray<FGridCoordinate>> USimpleGridDungeonGenerator::InitPossibleRooms()
{
	TArray<TArray<FGridCoordinate>> AlLRooms;
	
	//Iterate over every rectangle between MinSize and MaxSize
	constexpr int MinSize = 2;
	constexpr int MaxSize = 3;
	for (int Width = MinSize; Width <= MaxSize; Width++)
	{
		for (int Height = MinSize; Height <= MaxSize; Height++)
		{
			// Instead of just creating a box from 0->Width and 0->Height, we offset
			// the box by half the width and height so the centre of the room is roughly in the middle
			const int W_Neg = -(Width+1) / 2;
			const int W_Pos = W_Neg + Width;
			const int H_Neg = -(Height+1) / 2;
			const int H_Pos = H_Neg + Height;

			// Create the actual room representation
			TArray<FGridCoordinate> RoomOffsetLayout;
			for (int W = W_Neg; W <= W_Pos; W++)
			{
				for (int H = H_Neg; H <= H_Pos; H++)
				{
					RoomOffsetLayout.Add(FGridCoordinate(W,H));
				}
			}
			
			AlLRooms.Add(RoomOffsetLayout);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Total generated possible rooms: %d"), AlLRooms.Num());

	// Since there can be a huge number of possible rooms, we reduce the number of sampled rooms to improve performance
	// TODO set equal to number of rooms
	int MaxRoomsInGen = 12;
	MaxRoomsInGen = FMath::Min(MaxRoomsInGen, AlLRooms.Num());
	
	AlLRooms.Sort([this](const TArray<FGridCoordinate>& Item1, const TArray<FGridCoordinate>& Item2) {
		return FMath::FRand() < 0.5f;
	});
	TArray<TArray<FGridCoordinate>> PossibleRooms = {};
	
	// Adds a random selection of rooms into generation
	for (int i = 0; i < MaxRoomsInGen; i++)
	{
		PossibleRooms.Add(AlLRooms[i]);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Total sampled possible rooms for actual generation: %d"), PossibleRooms.Num());
	return PossibleRooms;
}

TArray<TArray<TArray<FGridCoordinate>>> USimpleGridDungeonGenerator::GenerateRoomComboOffsets(const TArray<TArray<FGridCoordinate>>& Rooms)
{
	TArray<TArray<TArray<FGridCoordinate>>> RoomComboOffsets = {};

	for (int i = 0; i < Rooms.Num(); i++)
	{
		TArray<TArray<FGridCoordinate>> SecondRoomConnections;
		SecondRoomConnections.Init(TArray<FGridCoordinate>(), Rooms.Num());
		RoomComboOffsets.Add(SecondRoomConnections);
	}
	
	// Iterates over every room, and adds coord offsets from one room to the other
	for (int i = 0; i < Rooms.Num(); i++)
	{
		for (int j = i; j < Rooms.Num(); j++)
		{
			const TArray<FGridCoordinate> ItoJOffsets = GenerateOffsetsForRooms(Rooms[i], Rooms[j]);
			RoomComboOffsets[i][j] = ItoJOffsets;

		}
	}
	
	return RoomComboOffsets;
}

TArray<FGridCoordinate> USimpleGridDungeonGenerator::GenerateOffsetsForRooms(const TArray<FGridCoordinate>& RoomA, const TArray<FGridCoordinate>& RoomB)
{
	TArray<FGridCoordinate> OutArray;

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

void USimpleGridDungeonGenerator::AddSingleRoomToLayout(TArray<TArray<TArray<FGridCoordinate>>> RoomComboOffsets,
                                                        TArray<TArray<FGridCoordinate>> PossibleRooms, TArray<FDungeonRoom>& RoomLayout, TSet<FGridCoordinate>& RoomLayoutUsedCoords, TMap<FDungeonRoom,
                                                        FDungeonRoom>& RoomConnections) const
{
	// Take a new random room layout
	const int NewRoomIndex = FMath::RandRange(0,PossibleRooms.Num()-1);
	TArray<FGridCoordinate> NewRoomLocalCoords = PossibleRooms[NewRoomIndex];

	// Find every existing room and their PossibleRooms index
	TMap<FGridCoordinate, FDungeonRoom> PlaceableLocations;
	for (FDungeonRoom ExistingRoom : RoomLayout)
	{
		// Find all placeable points -> filter out direct centre overlaps
		for (FGridCoordinate RoomOffset : RoomComboOffsets[ExistingRoom.PossibleRoomsIndex][NewRoomIndex])
		{
			FGridCoordinate NewRoomGlobalOrigin = ExistingRoom.GlobalCentre + RoomOffset;

			// Get Global Coords for this hypothetical dungeon
			bool CanUse = true;
			for (FGridCoordinate LocalOffsetFromPotentialCentre : NewRoomLocalCoords)
			{
				if (RoomLayoutUsedCoords.Contains(LocalOffsetFromPotentialCentre + NewRoomGlobalOrigin))
				{
					CanUse = false;
					break;
				}
			}
			if (CanUse)
			{
				if (!PlaceableLocations.Contains(NewRoomGlobalOrigin))
				{
					PlaceableLocations.Add(NewRoomGlobalOrigin, ExistingRoom);
				}
			}
		}
	}

	TArray<FGridCoordinate> ListOfSpawnLocations;
	PlaceableLocations.GetKeys(ListOfSpawnLocations);

	// Select a random location to place the new room
	const FGridCoordinate RoomCentre = ListOfSpawnLocations[FMath::RandRange(0, ListOfSpawnLocations.Num() - 1)];
	const FDungeonRoom NewRoom = FDungeonRoom(RoomCentre, NewRoomLocalCoords, NewRoomIndex);
	RoomLayout.Add(NewRoom);
	RoomConnections.Add(NewRoom, PlaceableLocations[RoomCentre]);
	
	// Update global set of coord tiles
	for (FGridCoordinate LocalCoord : NewRoom.LocalCoordOffsets)
	{
		RoomLayoutUsedCoords.Add(LocalCoord+NewRoom.GlobalCentre);
	}
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
