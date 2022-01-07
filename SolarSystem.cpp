// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"
#include "ProceduralMeshComponent.h"
#include "Planet.h"

// Sets default values
ASolarSystem::ASolarSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComponent;

}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();
	CreateSolarSystem();
	AssignPlanetMaterials();
	AssignPlanetWaterSphere();
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASolarSystem::SpawnSun()
{
}

void ASolarSystem::CreateSolarSystem()
{
	for (int i = 0; i < numberOfPlanets; i++)
	{
		FVector spawnPos = FVector(distanceBetweenPlanets + (distanceBetweenPlanets * i), 0.f, 0.f);
		SpawnPlanet(spawnPos);
	}
}

void ASolarSystem::SpawnPlanet(FVector pos)
{
	FTransform spawnLocation;
	APlanet* spawnedPlanet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), spawnLocation);
	spawnedPlanet->SetActorLocation(pos);
	int _octaves = 0;
	int _subddivisions = 0;
	int _radius = 0;
	float _strength = 0;
	float _baseRoughness = 0;
	float _roughness = 0;
	float _persistence = 0;
	float _waterLevel = 0;
	float _rotateSpeed = 0;
	float _orbitSpeed = 0;
	int planetNumber = pos.X / distanceBetweenPlanets;
	float orbitSpeedMultiplier = (float)numberOfPlanets / (float)planetNumber;
	FVector _spinAxis = FVector();
	RandomiseAllValues(_octaves, _subddivisions, _radius, _strength, _baseRoughness, _roughness, _persistence, _waterLevel, _orbitSpeed, _rotateSpeed, _spinAxis);
	spawnedPlanet->CreatePlanet(_octaves, _subddivisions, _radius, _strength, _baseRoughness, _roughness, _persistence, _waterLevel, _orbitSpeed * orbitSpeedMultiplier, _rotateSpeed, _spinAxis);
	planets.Add(spawnedPlanet);
}

void ASolarSystem::RandomiseAllValues(int& octaves, int& subdivisions, int& radius, float& strength, float& baseRoughness, float& roughness, float& persistence, float& waterLevel, float &orbitSpeed, float& rotateSpeed, FVector& spinAxis)
{
	octaves = FMath::RandRange(minOctaves, maxOctaves);
	radius = FMath::RandRange(minRadius, maxRadius);
	subdivisions = FMath::RandRange(minSubdivisions, maxSubdivisions);
	strength = FMath::RandRange(minStrength, maxStrength);
	baseRoughness = FMath::RandRange(minBaseRoughness, maxBaseRoughness);
	roughness = FMath::RandRange(minRoughness, maxRoughness);
	persistence = FMath::RandRange(minPersistence, maxPersistence);
	waterLevel = FMath::RandRange(minWaterLevel, maxWaterLevel);
	orbitSpeed = FMath::RandRange(minOrbitSpeed * orbitSpeedMultipiler, maxOrbitSpeed * orbitSpeedMultipiler);
	rotateSpeed = FMath::RandRange(minRotateSpeed, maxRotateSpeed);
	spinAxis = FVector(FMath::RandRange(minSpinAxis.X, maxSpinAxis.X), FMath::RandRange(minSpinAxis.Y, maxSpinAxis.Y), FMath::RandRange(minSpinAxis.Z, maxSpinAxis.Z));
}

void ASolarSystem::AssignPlanetMaterials()
{
	for (int i = 0; i < planets.Num(); i++)
	{
		if (planets[i] != nullptr) 
		{
			planets[i]->PlanetMesh->SetMaterial(0, planetTempTexture);
		}
	}

}

void ASolarSystem::AssignPlanetWaterSphere()
{
	if (defaultWater != NULL) 
	{
		for (int i = 0; i < planets.Num(); i++)
		{
			if (planets[i] != nullptr) 
			{
				FTransform spawnLocation;
				AActor* tempWater = GetWorld()->SpawnActor<AActor>(defaultWater, spawnLocation);
				tempWater->SetActorLocation(planets[i]->GetActorLocation());
				float xyzScale = planets[i]->planetRadius * planets[i]->minValue;
				FVector scale = FVector(xyzScale).GetSafeNormal(); // pretty sure this is needed to make it a unit sphere
				scale *= planets[i]->planetRadius; // making it big
				tempWater->SetActorScale3D(scale / 25); // Finally!!!
				tempWater->SetActorRotation(planets[i]->GetActorRotation());
				planets[i]->waterSphere = tempWater;
			}
		}
	}
}

