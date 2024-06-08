// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/DataTable.h"
#include "CustomData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    DEFAULT			UMETA(DisplayName = "Default"),
    WEAPON   		UMETA(DisplayName = "Weapon"),
    CONSUMABLE 		UMETA(DisplayName = "Consumable"),
};

USTRUCT(Atomic, BlueprintType)
struct FGameItem : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* ItemImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 StackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float MaxDurability;

    FGameItem()
        : ItemName(NAME_None), Description(TEXT("")), ItemImage(nullptr), StackSize(0), ItemType(EItemType::DEFAULT), MaxDurability(0.f)
    {
    }
};

USTRUCT(Atomic, BlueprintType)
struct FInvenSlot : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")  
    int32 Amount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 MaxAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item") 
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool IsEquipped;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float Durability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float MaxDurability;

    FInvenSlot()
        : ItemName(NAME_None), Amount(0), MaxAmount(0), ItemType(EItemType::DEFAULT), IsEquipped(false), Durability(0.f), MaxDurability(0.f)
    {
    }
};