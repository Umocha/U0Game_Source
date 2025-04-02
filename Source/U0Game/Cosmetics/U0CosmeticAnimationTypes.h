// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "U0CosmeticAnimationTypes.generated.h"

USTRUCT(BlueprintType)
struct FU0AnimLayerSelectionEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Layer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;
};


USTRUCT(BlueprintType)
struct FU0AnimLayerSelectionSet
{
	GENERATED_BODY()

	/* CosmeticTags 기반하여, 적절한 AnimLayer를 반환한다 */
	TSubclassOf<UAnimInstance> SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const;

	/* 앞서 보았던 U0AnimBodyStyleSelection의 MeshRules과 같이 AnimInstance의 Rule을 가진 LayerRules로 생각하면 됨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FU0AnimLayerSelectionEntry> LayerRules;

	/* Default Layer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> DefaultLayer;

};

USTRUCT(BlueprintType)
struct FU0AnimBodyStyleSelectionEntry
{
	GENERATED_BODY()

	/* AnimLayer를 적용할 대상 SkeletalMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	/* Cosmetic Tag라고 생각하면 됨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FU0AnimBodyStyleSelectionSet
{
	GENERATED_BODY()

	/* GameplayTag를 통해 (CosmeticTags), Mesh Rules에 따라 알맞은 BodyStyle를 변환한다 */
	USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;

	/* AnimLayer 적용할 SkeletalMesh를 들고 있음 -> Animation-Mesh간 Rules을 MeshRules라고 생각하면 됨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FU0AnimBodyStyleSelectionEntry> MeshRules;

	/* 그냥 디폴트로 적용할 SkeletalMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	/* Physics Asset은 하나로 동일함 -> 즉 모든 Animation의 Physics 속성은 공유함 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;

};