// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/SimpleGridDungeonInstance.h"

#include "Generators/SimpleGridDungeonGenerator.h"
#include "Layouts/SimpleGridDungeonLayout.h"


// Sets default values
ASimpleGridDungeonInstance::ASimpleGridDungeonInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Generator = CreateDefaultSubobject<USimpleGridDungeonGenerator>("Dungeon Generator");
	Layout = CreateDefaultSubobject<USimpleGridDungeonLayout>("Dungeon Layout");
}

void ASimpleGridDungeonInstance::GenerateLayout()
{
	Super::GenerateLayout();
	UE_LOG(LogTemp, Log, TEXT("ASimpleGridDungeonInstance::GenerateLayout()"));

	Layout = Generator->GenerateLayout();
}

void ASimpleGridDungeonInstance::SpawnDungeon()
{
	Super::SpawnDungeon();
	UE_LOG(LogTemp, Log, TEXT("ASimpleGridDungeonInstance::SpawnDungeon()"));
}

void ASimpleGridDungeonInstance::GenerateDungeon()
{
	Super::GenerateDungeon();
	UE_LOG(LogTemp, Log, TEXT("ASimpleGridDungeonInstance::GenerateDungeon()"));
}

// Called when the game starts or when spawned
void ASimpleGridDungeonInstance::BeginPlay()
{
	Super::BeginPlay();
	
}

