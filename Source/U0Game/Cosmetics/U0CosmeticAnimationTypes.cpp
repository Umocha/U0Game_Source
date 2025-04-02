// Fill out your copyright notice in the Description page of Project Settings.


#include "U0CosmeticAnimationTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0CosmeticAnimationTypes)

/// ////////////////////////////
// FU0AnimLayerSelectionSet
////////////////////////////////
TSubclassOf<UAnimInstance> FU0AnimLayerSelectionSet::SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const
{
    for (const FU0AnimLayerSelectionEntry& Rule : LayerRules)
    {
        if ((Rule.Layer) && CosmeticTags.HasAll(Rule.RequiredTags))
        {
            return Rule.Layer;
        }
    }

    return DefaultLayer;
}


/// ////////////////////////////
// FU0AnimBodyStyleSelectionSet
////////////////////////////////
USkeletalMesh* FU0AnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
    // MeshRule을 순회하며, CosmeticTags 요구 조건에 맞는 MeshRule을 찾아 SkeletalMesh를 반환한다
    for (const FU0AnimBodyStyleSelectionEntry& Rule : MeshRules)
    {
        if ((Rule.Mesh) && CosmeticTags.HasAll(Rule.RequiredTags))
        {
            return Rule.Mesh;
        }
    }

    return DefaultMesh;
}
