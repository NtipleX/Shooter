// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"

AShooterCharacter::AShooterCharacter(): m_isAiming(false)
{
	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	AR_MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComp"));
	AR_ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FlashParticleComp"));

	CameraSpring->SetupAttachment(RootComponent);
	FollowCamera->SetupAttachment(CameraSpring, USpringArmComponent::SocketName);
	AR_MeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("RightHandSocket"));
	AR_ParticleComponent->AttachToComponent(AR_MeshComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("FlashSocket"));

	AR_MeshComponent->SetRelativeLocation(FVector::ZeroVector);
	AR_ParticleComponent->SetRelativeLocation(FVector::ZeroVector);

	PrimaryActorTick.bCanEverTick = true;
	CameraSpring->TargetArmLength = 300.f;
	CameraSpring->bUsePawnControlRotation = true;
	FollowCamera->bUsePawnControlRotation = false;
	
	//Character Rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 1.f;

}

bool AShooterCharacter::IsAiming()
{
	return m_isAiming;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookSide", this, &AShooterCharacter::LookSide);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::FireWeapon);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShooterCharacter::AimWeaponPrivate);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void AShooterCharacter::MoveForward(float scale)
{
	if (scale != 0.f)
	{
		FRotator ControllerRotation = GetController()->GetControlRotation();
		FRotator YawRotation = FRotator{ 0, ControllerRotation.Yaw, 0 };
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, scale);
		IsMovingBackwards = false;
		if (scale < 0)
			IsMovingBackwards = true;
	}
}

void AShooterCharacter::MoveRight(float scale)
{
	if (scale != 0.f)
	{
		FRotator ControllerRotation = GetController()->GetControlRotation();
		FRotator YawRotation = FRotator{ 0, ControllerRotation.Yaw, 0 };
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, scale);
	}
}

void AShooterCharacter::LookUp(float scale)
{
	AddControllerPitchInput(scale);
}

void AShooterCharacter::LookSide(float scale)
{
	AddControllerYawInput(scale);
}

void AShooterCharacter::FireWeapon()
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	ViewportSize /= 2.f;
	FVector Position, Direction;
	FHitResult HitResult;
	bool result = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GetWorld(), 0), ViewportSize, Position, Direction);
	if (result)
	{
		FVector End{ Position + Direction * 10'000.f };
		GetWorld()->LineTraceSingleByChannel(HitResult, Position, End, ECollisionChannel::ECC_WorldStatic);
		if (AR_ShotCue)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AR_ShotCue, GetActorLocation());
		}
		AR_ParticleComponent->ActivateSystem();

		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance && FireMontage)
		{
			animInstance->Montage_Play(FireMontage);
			animInstance->Montage_JumpToSection(FName("Fire"));
		}

		auto flash = AR_MeshComponent->SkeletalMesh->FindSocket(FName("FlashSocket"));

		FHitResult OutHitResult;
		if (flash)
		{
			FVector Start = flash->GetSocketLocation(AR_MeshComponent);
			if (ImpactParticle)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitResult.ImpactPoint);
			if (BeamTrail)
			{
				UParticleSystemComponent* spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamTrail, flash->GetSocketTransform(AR_MeshComponent));
				spawnedParticle->SetVectorParameter(FName("Target"), HitResult.ImpactPoint);
			}
		}
	}

}

void AShooterCharacter::AimWeaponPrivate()
{
	if (m_isAiming)
	{
		m_isAiming = false;
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else
	{
		m_isAiming = true;
		GetCharacterMovement()->MaxWalkSpeed = 120.f;
	}

	bUseControllerRotationYaw = m_isAiming;
	AimingSwitch(m_isAiming);
}
