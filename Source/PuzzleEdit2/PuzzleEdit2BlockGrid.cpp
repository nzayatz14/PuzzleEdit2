// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzleEdit2.h"
#include "PuzzleEdit2BlockGrid.h"
#include "PuzzleEdit2Block.h"
#include "Components/TextRenderComponent.h"
#include <ctime>

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

APuzzleEdit2Block** allBlocks;

APuzzleEdit2BlockGrid::APuzzleEdit2BlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component for score text
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
    ScoreText->XScale = 3.f;
    ScoreText->YScale = 3.f;
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(100)));
	ScoreText->AttachTo(DummyRoot);
    
    // Create static mesh component for time
    Time = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Time0"));
    Time->XScale = 3.f;
    Time->YScale = 3.f;
    Time->SetRelativeLocation(FVector(260.f,0.f,0.f));
    Time->SetRelativeRotation(FRotator(90.f,0.f,0.f));
    Time->SetText(FText::Format(LOCTEXT("TimeFmt", "Time Left: {0}:0{1}"), FText::AsNumber(5), FText::AsNumber(0)));
    Time->AttachTo(DummyRoot);

	// Set defaults
    Score = 100;
	Size = 5;
	BlockSpacing = 150.f;
    time = 300;
    lost = false;
    won = false;
}


void APuzzleEdit2BlockGrid::BeginPlay()
{
	Super::BeginPlay();

	// Number of blocks
	const int32 NumBlocks = Size * Size;
    APuzzleEdit2Block** blocks = new APuzzleEdit2Block*[NumBlocks];
    allBlocks = new APuzzleEdit2Block*[NumBlocks];

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
        allBlocks[BlockIndex] = NewBlock;
        //UE_LOG(LogTemp, Warning, TEXT("Block %d: %p\n"), BlockIndex, blocks[BlockIndex]);
		// Tell the block about its owner
		if(NewBlock != NULL)
		{
			NewBlock->OwningGrid = this;
		}
	}
    
    
    //Add blocks to adjacent lists
    for (int i = 0; i<NumBlocks; i++) {
        APuzzleEdit2Block** adjacentBlocks = new APuzzleEdit2Block*[4];
        int numPieces = 0;
        
        //if it has a block above it
        if ((i+Size) < NumBlocks) {
            adjacentBlocks[numPieces] = blocks[i+Size];
            UE_LOG(LogTemp, Warning, TEXT("Block %d: %p = %p\n"), i, adjacentBlocks[numPieces] ,blocks[i+Size]);
            numPieces++;
        }
        
        //if it has a block below it
        if ((i-Size) >= 0) {
            adjacentBlocks[numPieces] = blocks[i-Size];
            numPieces++;
        }
        
        //if it has a block to the right of it
        if ((i+1) % Size != 0 && (i+1) < NumBlocks) {
            adjacentBlocks[numPieces] = blocks[i+1];
            numPieces++;
        }
        
        //if it has a block to the left of it
        if (i % Size != 0 && (i-1) >= 0) {
            adjacentBlocks[numPieces] = blocks[i-1];
            numPieces++;
        }
        
        //copy blocks to new, smaller sized array to save space
        APuzzleEdit2Block **adjacentArray = new APuzzleEdit2Block*[numPieces];
        
        for (int j = 0; j<numPieces; j++){
            //UE_LOG(LogTemp, Warning, TEXT("Block Pointer %d: %p\n"), j,adjacentBlocks[j]);
            adjacentArray[j] = adjacentBlocks[j];
        }
        
        //free statement causes app to crash
        //free(adjacentBlocks);
        
        blocks[i]->numAdjacentBlocks = numPieces;
        blocks[i]->adjacentBlocks = new APuzzleEdit2Block*[numPieces];
        blocks[i]->adjacentBlocks = adjacentArray;
    }
    
    
    //set up timer
    GetWorld()->GetTimerManager().SetTimer(handleClock, this, &APuzzleEdit2BlockGrid::showTime, 1.0f, true);
    //GetWorldTimerManager().SetTimer
}


/**
Function called to add score to the user
 
 - parameter void
 - returns: void
*/
void APuzzleEdit2BlockGrid::AddScore()
{
	// Increment score
	Score--;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
    
    checkWin();
    checkLoss();
}


/**
 Function called to check if the user has won
 
 - parameter void
 - returns: void
 */
void APuzzleEdit2BlockGrid::checkWin(){
    bool win = true;
    
    //check if all blocks are on
    for (int i = 0; i<25;i++){
        if (allBlocks[i]->bIsActive == false) {
            win = false;
        }
    }
    
    //if the user has won, tell them
    if (win && !lost) {
        won = true;
        GetWorld()->GetTimerManager().ClearTimer(handleClock);
        ScoreText->SetText(TEXT("YOU WIN!!!"));
    }
}


/**
 Function called to check if the user has lost
 
 - parameter void
 - returns: void
 */
void APuzzleEdit2BlockGrid::checkLoss(){
    
    //player loses if time runs out or if the score runs out
    if ((time == 0 || Score <= 0) && !won){
        GetWorld()->GetTimerManager().ClearTimer(handleClock);
        Score = 0;
        ScoreText->SetText(TEXT("You Lose :("));
        lost = true;
    }
}


/**
 Function called to show the time of the game to the user
 
 - parameter void
 - returns: void
 */
void APuzzleEdit2BlockGrid::showTime() {
    
    //increment time
    time--;
    
    //calculate minutes and seconds
    int32 minutes = time / 60;
    int32 seconds = time % 60;
    
    //Set time label
    if (seconds < 10){
        Time->SetText(FText::Format(LOCTEXT("TimeFmt", "Time Left: {0}:0{1}"), FText::AsNumber(minutes), FText::AsNumber(seconds)));
    }else{
        Time->SetText(FText::Format(LOCTEXT("TimeFmt", "Time Left: {0}:{1}"), FText::AsNumber(minutes), FText::AsNumber(seconds)));
    }
    
    //check if the user has lost
    checkLoss();
}


#undef LOCTEXT_NAMESPACE
