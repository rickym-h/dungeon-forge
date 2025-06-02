// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Instances/SimpleGridDungeonInstance.h"
#include "UObject/Object.h"
#include "SimpleGridDungeonGenerator.generated.h"


USTRUCT()
struct FDungeonRoom
{
	GENERATED_BODY()

	UPROPERTY()
	FGridCoordinate GlobalCentre;

	UPROPERTY()
	TSet<FGridCoordinate> LocalCoordOffsets;

	FDungeonRoom();
	FDungeonRoom(FGridCoordinate InGlobalCentre, const TSet<FGridCoordinate>& InLocalCoordOffsets);
	TSet<FGridCoordinate> GetGlobalCoordOffsets() const;
	static int MaxManhattanDistanceBetweenRooms(TSet<FGridCoordinate> A, TSet<FGridCoordinate> B);
	static bool DoRoomsOverlap(FDungeonRoom A, FDungeonRoom B);
	static bool AreRoomsTouching(const FDungeonRoom& A, const FDungeonRoom& B);

	bool operator==(const FDungeonRoom& Other) const
	{
		if (GlobalCentre != Other.GlobalCentre)
			return false;
        
		// Compare sets by checking if they have the same elements
		if (LocalCoordOffsets.Num() != Other.LocalCoordOffsets.Num())
			return false;
        
		for (const FGridCoordinate& Coord : LocalCoordOffsets)
		{
			if (!Other.LocalCoordOffsets.Contains(Coord))
				return false;
		}
    
		return true;
	}

	bool operator!=(const FDungeonRoom& Other) const
	{
		return !(*this == Other);
	}
};
FORCEINLINE uint32 GetTypeHash(const FDungeonRoom& DungeonRoom)
{
	uint32 Hash = GetTypeHash(DungeonRoom.GlobalCentre);
	for (FGridCoordinate Coord : DungeonRoom.LocalCoordOffsets)
	{
		Hash = HashCombine(Hash, ::GetTypeHash(Coord));
	}
	return Hash;
}
/**
 * 
 */
UCLASS()
class DUNGEONFORGE_API USimpleGridDungeonGenerator : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * A very simple dungeon generator which keeps adding rooms in an arbitrary position to the layout until it reaches a certain room count.
	 * @return The generated layout.
	 */
	UFUNCTION(BlueprintCallable)
	USimpleGridDungeonLayout* GenerateLayout();
	
	/**
	 * An initialisation function to set the parameters of the generator.
	 * It creates a datalist of possible rooms and calculates the offsets between them so that the generator runs faster at runtime/generation time.
	 * @param InRoomCount The number of rooms to generate.
	 */
	void SetNumRooms(const int32 InRoomCount);

protected:
	int32 RoomCount;
	TArray<FDungeonRoom> PossibleRooms;

	/**
	 * A map of pairs of rooms (A and B) to a list of relative coordinates where an instance of B can be offset from A.
	 */
	TMap<TTuple<FDungeonRoom, FDungeonRoom>, TSet<FGridCoordinate>> RoomComboOffsetsMap;
	
	TArray<FDungeonRoom> InitPossibleRooms();

	TMap<TTuple<FDungeonRoom, FDungeonRoom>, TSet<FGridCoordinate>> GenerateRoomComboOffsets(const TArray<FDungeonRoom>& Rooms);
	TSet<FGridCoordinate> GenerateOffsetsForRooms(const TSet<FGridCoordinate>& RoomA, const TSet<FGridCoordinate>& RoomB);

	void AddSingleRoomToLayout(TArray<FDungeonRoom> &RoomLayout, TSet<FGridCoordinate> &RoomLayoutUsedCoords, TMap<FDungeonRoom, FDungeonRoom>& RoomConnections) const;
	
	UFUNCTION(BlueprintCallable)
	static USimpleGridDungeonLayout* SimpleStaticLayout1();
	
};
