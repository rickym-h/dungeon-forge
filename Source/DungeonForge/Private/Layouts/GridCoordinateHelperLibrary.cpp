// Fill out your copyright notice in the Description page of Project Settings.


#include "Layouts/GridCoordinateHelperLibrary.h"

FGridCoordinate::FGridCoordinate(): X(0), Y(0)
{
}

FGridCoordinate::FGridCoordinate(const int32 InX, const int32 InY)
{
	X = InX;
	Y = InY;
}

float FGridCoordinate::DistanceFromCentre() const
{
	if (X == 0 && Y == 0)
	{
		return 0.0f;
	} else if (X == 0)
	{
		return Y;
	} else if (Y == 0)
	{
		return X;
	}
	return FMath::Sqrt(static_cast<float>(FMath::Square(X) + FMath::Square(Y)));
}

FGridCoordinate FGridCoordinate::Inverse() const
{
	return FGridCoordinate(-this->X, -this->Y);
}

FGridCoordinate FGridCoordinate::Rotate(const int32 RotationCount) const
{
	if (RotationCount == 1)
	{
		return FGridCoordinate(this->Y, -this->X);
	}
	return FGridCoordinate(this->Y, -this->X).Rotate(RotationCount - 1);
}

FGridEdge::FGridEdge()
{
}

FGridEdge::FGridEdge(const FGridCoordinate& InCoordinateA, const FGridCoordinate& InCoordinateB)
{
	CoordinateA = InCoordinateA;
	CoordinateB = InCoordinateB;
	Sort();
}

void FGridEdge::Sort()
{
	if (GetTypeHash(CoordinateA) < GetTypeHash(CoordinateB)) { return; }

	// swap coordinates
	const FGridCoordinate Temp = CoordinateA;
	CoordinateA = CoordinateB;
	CoordinateB = Temp;
}

bool FGridEdge::SharesSingleCoordinate(const FGridEdge EdgeB, FGridCoordinate& OutSharedCoordinate) const
{
	const TSet<FGridCoordinate> EdgeACoords = { CoordinateA, CoordinateB };
	const TSet<FGridCoordinate> EdgeBCoords = { EdgeB.CoordinateA, EdgeB.CoordinateB };
	TSet<FGridCoordinate> SharedCoords = EdgeACoords.Intersect(EdgeBCoords);
	if (SharedCoords.Num() == 1)
	{
		OutSharedCoordinate = SharedCoords.Array()[0];
		return true;
	}
	return false;
}

bool FGridEdge::FormsCorner(FGridEdge EdgeB) const
{
	FGridCoordinate OutSharedCoordinate;
	if (!SharesSingleCoordinate(EdgeB, OutSharedCoordinate)) return false;

	TSet<FGridCoordinate> AllCoords = { CoordinateA, CoordinateB, EdgeB.CoordinateA, EdgeB.CoordinateB }; // This should only be 3 coordinates as one is duplicated
	AllCoords.Remove(OutSharedCoordinate); // Remove the duplicated coord

	// The edges form a corner if these remaining coordinates are diagonal from eachother.
	const TArray<FGridCoordinate> PossibleCorners = AllCoords.Array();
	const FGridCoordinate A = PossibleCorners[0];
	const TSet<FGridCoordinate> ADiagonals = TSet(UGridCoordinateHelperLibrary::GetAdjacentCoordinates(A, true, 0)).Difference(TSet(UGridCoordinateHelperLibrary::GetAdjacentCoordinates(A, false, 0)));

	return ADiagonals.Contains(PossibleCorners[1]);
}

FGridCorner::FGridCorner()
{
}

FGridCorner::FGridCorner(const FGridCoordinate& InCoordinateA, const FGridCoordinate& InCoordinateB, const FGridCoordinate& InCoordinateC,
	const FGridCoordinate& InCoordinateD)
{
	CoordinateA = InCoordinateA;
	CoordinateB = InCoordinateB;
	CoordinateC = InCoordinateC;
	CoordinateD = InCoordinateD;
	Sort();
}

void FGridCorner::Sort()
{
	TArray<FGridCoordinate> Corners = { CoordinateA, CoordinateB, CoordinateC, CoordinateD };

	Corners.Sort([](const FGridCoordinate& A, const FGridCoordinate& B)
	{
		return GetTypeHash(A) < GetTypeHash(B);
	});

	CoordinateA = Corners[0];
	CoordinateB = Corners[1];
	CoordinateC = Corners[2];
	CoordinateD = Corners[3];
}

FGridCorner FGridCorner::FromEdges(const FGridEdge& EdgeA, const FGridEdge& EdgeB)
{	
	if (!EdgeA.FormsCorner(EdgeB))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot create corner from edges that do not share a single coordinate"));
		return FGridCorner();
	}
	
	TSet<FGridCoordinate> OtherCoords = { EdgeA.CoordinateA, EdgeA.CoordinateB, EdgeB.CoordinateA, EdgeB.CoordinateB };
	// At this point OtherCoords should be a list of 3 coordinates since one of the edge coordinates is shared.

	TSet<FGridCoordinate> AdjacentCoords;

	for (const FGridCoordinate OtherCoord : OtherCoords)
	{
		for (const auto AdjacentCoord : UGridCoordinateHelperLibrary::GetAdjacentCoordinates(OtherCoord))
		{
			if (AdjacentCoords.Contains(AdjacentCoord) && !OtherCoords.Contains(AdjacentCoord))
			{
				OtherCoords.Add(AdjacentCoord);
				return FGridCorner(OtherCoords.Array()[0], OtherCoords.Array()[1], OtherCoords.Array()[2], OtherCoords.Array()[3]);
			};
			AdjacentCoords.Add(AdjacentCoord);
		}
	}

	// Shouldn't get to this point
	UE_LOG(LogTemp, Error, TEXT("FGridCorner::FromEdges"));
	return FGridCorner();
}

TArray<FGridCoordinate> UGridCoordinateHelperLibrary::GetAdjacentCoordinates(const FGridCoordinate& Coordinate, const bool bIncludeDiagonal, const int32 Direction)
{
	TArray<FGridCoordinate> AdjacentCoordinates;

	// Add orthogonal coordinates
	if (Direction == 0 || Direction == 2) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X + 1, Coordinate.Y));
	if (Direction == 0 || Direction == 4) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X - 1, Coordinate.Y));
	if (Direction == 0 || Direction == 1) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X, Coordinate.Y + 1));
	if (Direction == 0 || Direction == 3) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X, Coordinate.Y - 1));

	if (bIncludeDiagonal && Direction == 0)
	{
		// Add diagonal coordinates
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X + 1, Coordinate.Y + 1));
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X + 1, Coordinate.Y - 1));
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X - 1, Coordinate.Y + 1));
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X - 1, Coordinate.Y - 1));
	}

	return AdjacentCoordinates;
}

FVector UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(const FGridCoordinate& Coordinate, const float TileSize)
{
	return FVector(Coordinate.X * TileSize, Coordinate.Y * TileSize, 0.0f);
}

TArray<FGridCoordinate> UGridCoordinateHelperLibrary::Expand(TArray<FGridCoordinate> RoomRepresentation, const int32 ExpansionDistance,
	const bool bIncludeDiagonal, const int32 Direction)
{
	TSet<FGridCoordinate> RoomRepresentationSet = TSet(RoomRepresentation);

	for (int i = 0; i < ExpansionDistance; i++)
	{
		for (FGridCoordinate Coordinate : RoomRepresentation)
		{
			RoomRepresentationSet.Append(GetAdjacentCoordinates(Coordinate, bIncludeDiagonal, Direction));
		}
		RoomRepresentation = RoomRepresentationSet.Array();
	}
	
	return RoomRepresentationSet.Array();
}

TSet<FGridCoordinate> UGridCoordinateHelperLibrary::RotateClockwise(const TSet<FGridCoordinate>& Coordinates, const int32 RotationCount)
{
	TSet<FGridCoordinate> RotatedCoordinates;
	
	for (const FGridCoordinate Coordinate : Coordinates)
	{
		RotatedCoordinates.Add(Coordinate.Rotate(RotationCount));
	}
	
	return RotatedCoordinates;
}
