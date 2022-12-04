// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterLocalRunner.h"
#include "Battle/Actors/FighterGameState.h"

void FighterLocalRunner::Update(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	int accumulatorBreaker = 0;
	while (ElapsedTime >= OneFrame && accumulatorBreaker < AccumulatorBreakerMax)
	{
		//while elapsed time is greater than one frame...
		GameState->TickGameState();
		ElapsedTime -= OneFrame ;
		accumulatorBreaker++;
	}
}

FighterLocalRunner::FighterLocalRunner(FighterGameState* InGameState)
{
	GameState = InGameState;
}
