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
	
	int PossibleRoomsIndex;

	FDungeonRoom();
	FDungeonRoom(FGridCoordinate InGlobalCentre, const TArray<FGridCoordinate>& InLocalCoordOffsets);
	FDungeonRoom(FGridCoordinate InGlobalCentre, const TArray<FGridCoordinate>& InLocalCoordOffsets, int InPossibleRoomsIndex);
	TArray<FGridCoordinate> GetGlobalCoordOffsets() const;
	static int MaxManhattanDistanceBetweenRooms(TArray<FGridCoordinate> A, TArray<FGridCoordinate> B);
	static bool DoRoomsOverlap(FDungeonRoom A, FDungeonRoom B);
	static bool AreRoomsTouching(const FDungeonRoom& A, const FDungeonRoom& B);

	bool operator==(const FDungeonRoom& Other) const
	{
		return GlobalCentre == Other.GlobalCentre;
	}
	bool operator!=(const FDungeonRoom& Other) const
	{
		return !(*this == Other);
	}
};
FORCEINLINE uint32 GetTypeHash(const FDungeonRoom& DungeonRoom)
{
	return FCrc::MemCrc32(&DungeonRoom.GlobalCentre, sizeof(DungeonRoom.GlobalCentre));
}
/**
 * 
 */
UCLASS()
class DUNGEONFORGE_API USimpleGridDungeonGenerator : public UObject
{
	GENERATED_BODY()
	
	USimpleGridDungeonGenerator();

public:
	/**
	 * A very simple dungeon generator which keeps adding rooms in an arbitrary position to the layout until it reaches a certain room count.
	 * @return The generated layout.
	 */
	UFUNCTION(BlueprintCallable)
	USimpleGridDungeonLayout* GenerateLayout();

protected:
	TArray<TArray<FGridCoordinate>> InitPossibleRooms();

	TArray<TArray<TArray<FGridCoordinate>>> GenerateRoomComboOffsets(const TArray<TArray<FGridCoordinate>>& Rooms);
	TArray<FGridCoordinate> GenerateOffsetsForRooms(const TArray<FGridCoordinate>& RoomA, const TArray<FGridCoordinate>& RoomB);
	void AddSingleRoomToLayout(TArray<TArray<TArray<FGridCoordinate>>> RoomComboOffsets, TArray<TArray<FGridCoordinate>> PossibleRooms, TArray<FDungeonRoom> &RoomLayout, TSet<FGridCoordinate> &RoomLayoutUsedCoords, TMap<FDungeonRoom, FDungeonRoom>& RoomConnections) const;
	
	UFUNCTION(BlueprintCallable)
	static USimpleGridDungeonLayout* SimpleStaticLayout1();
	
};
