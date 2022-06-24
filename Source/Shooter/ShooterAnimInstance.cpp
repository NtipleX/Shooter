// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::UpdateAnimationProperties()
{
	if (Character)
	{
		// Speed property
		FVector VelocityVector = Character->GetVelocity();
		VelocityVector.Z = 0;
		CharacterSpeed = VelocityVector.Size();

		// Is in air property
		bIsInAir = Character->GetCharacterMovement()->IsFalling();

		// Is accelerating property
		if (Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		// Is aiming property
		bIsAiming = Character->IsAiming();

		// Walk speed property while aiming
		if (bIsAiming)
		{
			auto vec = Character->GetVelocity();
			vec.Z = 0;
			WalkSpeed = vec.Size();
		}
	}
	else
		NativeInitializeAnimation();
}
