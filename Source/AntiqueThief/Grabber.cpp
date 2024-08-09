// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay() 
{
	Super::BeginPlay();
	// ...
	
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandler = GetPhysicsHandle();
	if(PhysicsHandler && GetPhysicsHandle()->GetGrabbedComponent() != nullptr)
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		FRotator CurrentRotation = GetComponentRotation();
		PhysicsHandler->SetTargetLocationAndRotation(TargetLocation, CurrentRotation);
	}

	// ...
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandler = GetPhysicsHandle();
	if(PhysicsHandler == nullptr)
	{
		return;
	}

	FHitResult HitResult;
	bool HasHit = GetGrabbableInReach(HitResult);
	if(HasHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();
		
		AActor* HitActor = HitResult.GetActor();
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HitActor->Tags.Add("Grabbed");

		PhysicsHandler->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
			);
	}
}

void UGrabber::Release()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if(PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		AActor* Actor = PhysicsHandle->GetGrabbedComponent()->GetOwner();
		Actor->Tags.Remove("Grabbed");
		PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const 
{
	//This is a template function that can take arguments to be evaluated in compile time.
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(Result == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("The Physics Handle is NULL!!!"));
	}
	return Result;
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDist;
	DrawDebugLine(GetWorld(), Start, End, FColor::Purple);

	// To create trace channels go to project settings
	// To find trace channels, go to project folder -> Config -> DefaultEngine.ini
	// Search for Grabber
	// And you can see the channel name
	// Then copy and paste it here as a parameter
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	bool HasHit = GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel2, 
		Sphere
		);
	return HasHit;
}