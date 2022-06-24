#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TraceHelpers.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = nxCategory)
	class USoundCue* AR_ShotCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = nxCategory)
	class USkeletalMeshComponent* AR_MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "nxCategory | Particles")
	class UParticleSystemComponent* AR_ParticleComponent;

	UPROPERTY(EditDefaultsOnly, Category = nxCategory)
	class UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "nxCategory | Particles")
	class UParticleSystem* ImpactParticle;

	UPROPERTY(EditDefaultsOnly, Category = "nxCategory | Particles")
	UParticleSystem* BeamTrail;

	AShooterCharacter();

public:

public:

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAiming();
	UPROPERTY(BlueprintReadOnly)
	bool IsMovingBackwards;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess = "true"))
	void AimingSwitch(bool aim);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void MoveForward(float scale);
	UFUNCTION()
	void MoveRight(float scale);
	UFUNCTION()
	void LookUp(float scale);
	UFUNCTION()
	void LookSide(float scale);
	UFUNCTION()
	void FireWeapon();
	UFUNCTION()
	void AimWeaponPrivate();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = nxCamera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpring;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = nxCamera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private:
	bool m_isAiming;
};
