// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/BaseDungeonInstance.h"


// Sets default values
ABaseDungeonInstance::ABaseDungeonInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseDungeonInstance::GenerateLayout()
{
	UE_LOG(LogTemp, Log, TEXT("ABaseDungeonInstance::GenerateLayout()"));
}

void ABaseDungeonInstance::SpawnDungeon()
{
	UE_LOG(LogTemp, Log, TEXT("ABaseDungeonInstance::SpawnDungeon()"));
}

void ABaseDungeonInstance::GenerateDungeon()
{
	UE_LOG(LogTemp, Log, TEXT("ABaseDungeonInstance::GenerateDungeon()"));
}

// Called when the game starts or when spawned
void ABaseDungeonInstance::BeginPlay()
{
	Super::BeginPlay();
	
}
