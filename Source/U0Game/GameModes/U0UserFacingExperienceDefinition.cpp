// Fill out your copyright notice in the Description page of Project Settings.


#include "U0UserFacingExperienceDefinition.h"
#include "CommonSessionSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0UserFacingExperienceDefinition)

UCommonSession_HostSessionRequest* UU0UserFacingExperienceDefinition::CreateHostingRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

	// 잠깐 한걸음 깊이 생각해보기:
	// - UCommonSession_HostSessionRequest는 UObject로 생성해놓고, 알아서 GC가 된다:
	// - 해당 객체는 현재 프레임에서 사용하기 때문에, GC에 대한 염려가 필요없다: 만약 다음 프레임이든 추가적인 프레임 상에서 해당 객체를 사용할 경우, LifeTime을 보장받지 못함
	//    - 그렇지 않으면 dangling 난다!
	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

	return Result;
}
