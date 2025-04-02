// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "U0TaggedActor.generated.h"

/*
* 원래 GameplayTag 관련 로직이 있으나 생략
* 나중에 따로 알아보자
*/

UCLASS()
class U0GAME_API AU0TaggedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AU0TaggedActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
