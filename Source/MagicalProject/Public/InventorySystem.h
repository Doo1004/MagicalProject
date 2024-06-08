// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomData.h"

class MAGICALPROJECT_API InventorySystem
{
public:
	InventorySystem();
	~InventorySystem();

public:
	void AddItem();

protected:
	FGameItem		Slots;

	bool			IsAddFailed;
	
};
