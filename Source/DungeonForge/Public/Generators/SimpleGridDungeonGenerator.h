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
	TArray<FGridCoordinate> LocalCoordOffsets;

	FDungeonRoom();
	FDungeonRoom(FGridCoordinate InGlobalCentre, const TArray<FGridCoordinate>& InLocalCoordOffsets);
	FDungeonRoom(FGridCoordinate InGlobalCentre, const TArray<FGridCoordinate>& InLocalCoordOffsets, int InPossibleRoomsIndex);
	TArray<FGridCoordinate> GetGlobalCoordOffsets() const;
	static int MaxManhattanDistanceBetweenRooms(TArray<FGridCoordinate> A, TArray<FGridCoordinate> B);
	static bool DoRoomsOverlap(FDungeonRoom A, FDungeonRoom B);
	static bool AreRoomsTouching(const FDungeonRoom& A, const FDungeonRoom& B);

	bool operator==(const FDungeonRoom& Other) const
	{
		return (GlobalCentre == Other.GlobalCentre) && (LocalCoordOffsets == Other.LocalCoordOffsets);
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
	//TArray<TArray<TArray<FGridCoordinate>>> RoomComboOffsets; // A square matrix where each cell contains a list of possible offsets for a room A, from a room B. Room A and Room B each have indexes which are used to get the list of coordinates.

	TMap<TTuple<FDungeonRoom, FDungeonRoom>, TArray<FGridCoordinate>> RoomComboOffsetsMap;
	
	TArray<FDungeonRoom> InitPossibleRooms();

	TMap<TTuple<FDungeonRoom, FDungeonRoom>, TArray<FGridCoordinate>> GenerateRoomComboOffsets(const TArray<FDungeonRoom>& Rooms);
	TArray<FGridCoordinate> GenerateOffsetsForRooms(const TArray<FGridCoordinate>& RoomA, const TArray<FGridCoordinate>& RoomB);

	void AddSingleRoomToLayout(TArray<FDungeonRoom> &RoomLayout, TSet<FGridCoordinate> &RoomLayoutUsedCoords, TMap<FDungeonRoom, FDungeonRoom>& RoomConnections) const;
	
	UFUNCTION(BlueprintCallable)
	static USimpleGridDungeonLayout* SimpleStaticLayout1();
	
};
