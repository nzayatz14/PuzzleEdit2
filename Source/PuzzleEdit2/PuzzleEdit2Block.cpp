// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzleEdit2.h"
#include "PuzzleEdit2Block.h"
#include "PuzzleEdit2BlockGrid.h"

APuzzleEdit2Block::APuzzleEdit2Block()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/GreenMaterial.GreenMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/VioletMaterial.VioletMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.5f,0.5f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->AttachTo(DummyRoot);
	BlockMesh->OnClicked.AddDynamic(this, &APuzzleEdit2Block::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &APuzzleEdit2Block::OnFingerPressedBlock);

	// Save a pointer to the orange material
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
    BlueMaterial = ConstructorStatics.BlueMaterial.Get();
}

void APuzzleEdit2Block::BlockClicked(UPrimitiveComponent* ClickedComp)
{
    changeColor();
    
    // Tell the Grid to change the score
    if(OwningGrid != NULL)
    {
        OwningGrid->AddScore();
    }
    
    for (int i = 0; i<numAdjacentBlocks; i++) {
        adjacentBlocks[i]->changeColor();
        UE_LOG(LogTemp, Warning, TEXT("Block Pointer %d: %p\n"), i,adjacentBlocks[i]);
    }
}


void APuzzleEdit2Block::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	BlockClicked(TouchedComponent);
}


void APuzzleEdit2Block::changeColor(){
    // Check we are not already active
    if(!bIsActive)
    {
        bIsActive = true;
        
        // Change material
        BlockMesh->SetMaterial(0, OrangeMaterial);
        
    }else{
        bIsActive = false;
        
        // Change material
        BlockMesh->SetMaterial(0, BlueMaterial);
    }
}

