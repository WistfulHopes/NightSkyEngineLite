// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FighterMultiplayerRunner.h"

class FighterSynctestRunner : public FighterMultiplayerRunner
{
public:
	FighterSynctestRunner(class FighterGameState* InGameState);
	void Init() override;
	void Update(float DeltaTime) override;

};
