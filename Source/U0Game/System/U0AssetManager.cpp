// Fill out your copyright notice in the Description page of Project Settings.

#include "U0AssetManager.h"
#include "U0Game/U0LogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0AssetManager)

UU0AssetManager::UU0AssetManager()
	: Super()
{
}

UU0AssetManager& UU0AssetManager::Get()
{
	if (UU0AssetManager* Singleton = Cast<UU0AssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogU0, Fatal, TEXT("invalid AssetManagerClassname in DefaultEngine.ini(projcet settings); it must be HakAssetManager"));

	// 위 UE_LOG의 Fatal은 Crash 나기 때문에 도달하지 않지만 컴파일 오류를 막기 위해 더미로 리턴
	return *NewObject<UU0AssetManager>();
}

UE_DISABLE_OPTIMIZATION
bool UU0AssetManager::TestClone()
{
	static bool bTest = ShouldLogAssetLoads();
	return true;
}

bool UU0AssetManager::ShouldLogAssetLoads()
{
	// 알아두면 좋은 것들이 두가지 있다:
	// 1. FParse::Param과 FCommandLine::Get()을 통해, 원하는 속성 값을 CommandLine으로 가져올 수 있다.
	// 2. 함수 안에 static 변수 선언이다.
	//	 - 언리얼에서 자주 보는 패턴, 보통 글로벌 변수를 선언하고 싶은 때 두가지 옵션이 있다:
	//		1. .h 파일에 extern을 선언하여, 헤더파일에 포함시킨다.
	//		2. static 함수를 선언하여, 해당 함수 안에 static 변수를 선언하여, 반환하는 방법(대표적인 예시 Sington)
	// const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}
UE_ENABLE_OPTIMIZATION

UObject* UU0AssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	// LoadSynchronous를 랩핑해서 쓰는 이유 : 불필요하게 load 되는것이 없는지 체크 하기 위함(Log)
	if (AssetPath.IsValid())
	{
		// FScopeLogTime을 확인해보자.
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			// 단순히 초단위로 로깅 진행
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		// 두가지 분기 :
		// 1. AssetManager가 있으면, AssetManager의 StreamableManager를 통해 정적 로딩
		//		- StaticLoadObject를 사용하지 않고 이걸 사용한 이유는 관리차원 말고는 Synchronous같은 경우 큰 차이는 없다.
		// 2. 없으면, FSoftObjectPath를 통해 바로 정적 로딩
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// StaticFindObject를 통해 확인하고 실패 하면 진짜 로딩함
		// 매우 느림
		return AssetPath.TryLoad();
	}

	return nullptr;
}

void UU0AssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}

void UU0AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 후일 추가하게 될 예정 일단 로깅만하고 지나가자:
	//UE_LOG(LogU0, Display, TEXT("UU0AssetMnager::StartInitialLoading"));

	// - Lyra와의 차이점을 한번 보고 가자:
	//		- STARTUP_JOB() 매크로를 활용하고 있으나, 현재 우리는 로딩과 관계 없으므로 간단하게 구현
}
