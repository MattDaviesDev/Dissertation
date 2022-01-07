// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SolarSystem.generated.h"


UCLASS()
class INNOVATIONPROJECT_API ASolarSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASolarSystem();

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> defaultWater;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> sunPrefab;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* planetTempTexture;

	AActor* sun;

	UPROPERTY(EditAnywhere)
		int numberOfPlanets;

	UPROPERTY(EditAnywhere)
		float distanceBetweenPlanets;

	TArray<class APlanet*> planets;

	UPROPERTY(EditAnywhere)
		int minOctaves;

	UPROPERTY(EditAnywhere)
		int maxOctaves;

	UPROPERTY(EditAnywhere)
		int minSubdivisions;

	UPROPERTY(EditAnywhere)
		int maxSubdivisions;

	UPROPERTY(EditAnywhere)
		int minRadius;
		
	UPROPERTY(EditAnywhere)
		int maxRadius;

	UPROPERTY(EditAnywhere)
		float minStrength;

	UPROPERTY(EditAnywhere)
		float maxStrength;

	UPROPERTY(EditAnywhere)
		float minBaseRoughness;

	UPROPERTY(EditAnywhere)
		float maxBaseRoughness;

	UPROPERTY(EditAnywhere)
		float minRoughness;

	UPROPERTY(EditAnywhere)
		float maxRoughness;

	UPROPERTY(EditAnywhere)
		float minPersistence;

	UPROPERTY(EditAnywhere)
		float maxPersistence;

	UPROPERTY(EditAnywhere)
		float minWaterLevel;

	UPROPERTY(EditAnywhere)
		float maxWaterLevel;

	UPROPERTY(EditAnywhere)
		float orbitSpeedMultipiler = 1;

	UPROPERTY(EditAnywhere)
		float minOrbitSpeed;

	UPROPERTY(EditAnywhere)
		float maxOrbitSpeed;

	UPROPERTY(EditAnywhere)
		float minRotateSpeed;

	UPROPERTY(EditAnywhere)
		float maxRotateSpeed;

	UPROPERTY(EditAnywhere)
		FVector minSpinAxis;

	UPROPERTY(EditAnywhere)
		FVector maxSpinAxis;


	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnSun();

	void CreateSolarSystem();

	void SpawnPlanet(FVector pos);

	void RandomiseAllValues(int& octaves, int& subdivisions, int& radius, float& strength, float& baseRoughness, float& roughness, float& persistence, float& waterLevel, float& orbitSpeed, float& rotateSpeed, FVector& spinAxis);

	void AssignPlanetMaterials();

	void AssignPlanetWaterSphere();

};
