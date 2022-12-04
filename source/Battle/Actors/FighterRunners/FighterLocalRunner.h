// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

constexpr int AccumulatorBreakerMax = 15;
constexpr float OneFrame = 1.f / 60.f;

class FighterLocalRunner
{
public:
	virtual ~FighterLocalRunner() = default;
	FighterLocalRunner(class FighterGameState* InGameState); 
	virtual void Init() = 0;
	virtual void Update(float DeltaTime);

protected:
	float ElapsedTime=0;
	class FighterGameState* GameState = nullptr;
};
