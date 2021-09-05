// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem() :
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),
	// item interp variables
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		// hide pickup widget
		PickupWidget->SetVisibility(false);
	}

	// sets activestars array based on item rarity
	SetActiveStars();

	// setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// set item properties based on item state 
	SetItemProperties(ItemState);

	// set custom depth to disabled
	InitializeCustomDepth();

}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStars()
{
	// the 0 element isn't used
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		// set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// set areasphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// set collisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		// set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// set areaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// set collisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_PickedUp:
		break;
	case EItemState::EIS_Equipped:
		// set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// set areasphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// set collisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		// set mesh properties
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		// set areaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// set collisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_MAX:
		break;
	default:
		break;
	}

}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		// subtract 1 from the item count of the interp location struct
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
		Character->GetPickupItem(this);
	}
	// set scale back to normal
	SetActorScale3D(FVector(1.f));
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (Character && ItemZCurve)
	{
		// elapsed time since we started itemInterpTimer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// get curve value corresponding to ElapsedTime
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		// get the item's initial location when the curve started
		FVector ItemLocation = ItemInterpStartLocation;
		// get location in front of the camera
		const FVector CameraInterpLocation{ GetInterpLocation() };

		// vector from iotem to camera interp location, x and y are zeroed out
		const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };
		// scale factor to multiply with curvevalue
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		// interpolated x values
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.f);

		// interpolated y values
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.f);

		// set x and y of itemlocation to interped values 
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		// adding curve value to the z component of the initial location (scaled by deltaZ)
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// camera rotation this frame 
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		// camera rotation plus initial yaw offset
		FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector(0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;

	case EItemType::EIT_Weapon:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;
	}
	return FVector();
}

void AItem::PlayPickupSound()
{
	if (Character)
	{
		if (Character->ShouldPlayPickupSound())
		{
			Character->StartPickupSoundTimer();
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(true);
}

void AItem::DisableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(false);
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::PlayEquipSound()
{
	if (Character)
	{
		if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemInterp(DeltaTime);

}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
	// store a handle to the character
	Character = Char;

	// get array index in InterpLocations with the lowest item count
	InterpLocIndex = Character->GetInterpLocationIndex();
	// add 1 to the item count for this interp location struct
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	PlayPickupSound();

	if (PickupSound)
	{
		UGameplayStatics::PlaySound2D(this, PickupSound);
	}

	// store initial location of the item
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	// get initial yaw of the camera
	const float CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	// get initial yaw of the item
	const float ItemRotationYaw{ GetActorRotation().Yaw };
	// intial yaw offset between Camera And Item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;
}

