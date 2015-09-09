// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzleEdit2.h"
#include "PuzzleEdit2BlockGrid.h"
#include "PuzzleEdit2Block.h"
#include "Components/TextRenderComponent.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

APuzzleEdit2BlockGrid::APuzzleEdit2BlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->AttachTo(DummyRoot);

	// Set defaults
	Size = 5;
	BlockSpacing = 150.f;
}


void APuzzleEdit2BlockGrid::BeginPlay()
{
	Super::BeginPlay();

	// Number of blocks
	const int32 NumBlocks = Size * Size;
    APuzzleEdit2Block** blocks = new APuzzleEdit2Block*[NumBlocks];

	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex/Size) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%Size) * BlockSpacing; // Modulo gives remainder

		// Make postion vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		APuzzleEdit2Block* NewBlock = GetWorld()->SpawnActor<APuzzleEdit2Block>(BlockLocation, FRotator(0,0,0));

        blocks[BlockIndex] = NewBlock;
        //UE_LOG(LogTemp, Warning, TEXT("Block %d: %p\n"), BlockIndex, blocks[BlockIndex]);
		// Tell the block about its owner
		if(NewBlock != NULL)
		{
			NewBlock->OwningGrid = this;
		}
	}
    
    
    //TODO: Add blocks to adjacent lists
    for (int i = 0; i<NumBlocks; i++) {
        APuzzleEdit2Block** adjacentBlocks = new APuzzleEdit2Block*[4];
        int numPieces = 0;
        
        if ((i+Size) < NumBlocks) {
            adjacentBlocks[numPieces] = blocks[i+Size];
            UE_LOG(LogTemp, Warning, TEXT("Block %d: %p = %p\n"), i, adjacentBlocks[numPieces] ,blocks[i+Size]);
            numPieces++;
        }
        
        if ((i-Size) >= 0) {
            adjacentBlocks[numPieces] = blocks[i-Size];
            numPieces++;
        }
        
        if ((i+1) % Size != 0 && (i+1) < NumBlocks) {
            adjacentBlocks[numPieces] = blocks[i+1];
            numPieces++;
        }
        
        if (i % Size != 0 && (i-1) >= 0) {
            adjacentBlocks[numPieces] = blocks[i-1];
            numPieces++;
        }
        
        //UE_LOG(LogTemp, Warning, TEXT("Block %d: %d\n"), i,numPieces);
        
        APuzzleEdit2Block **adjacentArray = new APuzzleEdit2Block*[numPieces];
        
        for (int j = 0; j<numPieces; j++){
            //UE_LOG(LogTemp, Warning, TEXT("Block Pointer %d: %p\n"), j,adjacentBlocks[j]);
            adjacentArray[j] = adjacentBlocks[j];
            
            if (i == 24) {
                //adjacentArray[j]->changeColor();
                //UE_LOG(LogTemp, Warning, TEXT("Block %d: %p\n"), j,adjacentArray[j]);
            }
        }
        
        if (i == 24) {
            blocks[i]->changeColor();
        }
        
        blocks[i]->numAdjacentBlocks = numPieces;
        blocks[i]->adjacentBlocks = new APuzzleEdit2Block*[numPieces];
        blocks[i]->adjacentBlocks = adjacentArray;
    }
}


void APuzzleEdit2BlockGrid::AddScore()
{
	// Increment score
	Score++;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

#undef LOCTEXT_NAMESPACE
