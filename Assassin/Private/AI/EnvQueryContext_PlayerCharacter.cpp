// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryContext_PlayerCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/AIControllerBase.h"
#include "Character/Enemy/Enemy.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"


void UEnvQueryContext_PlayerCharacter::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                      FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	AEnemy* QuerierActor = Cast<AEnemy>(QueryInstance.Owner.Get());
	AAIControllerBase* OwnerController = Cast<AAIControllerBase>(QuerierActor->GetController());
	AAssassinCharacter* TargetActor = Cast<AAssassinCharacter>(OwnerController->GetBlackboardComponent()->GetValueAsObject(AAIControllerBase::TargetKey));

	if(TargetActor)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
	}
}
