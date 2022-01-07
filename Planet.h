// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.generated.h"

UCLASS()
class INNOVATIONPROJECT_API APlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlanet();

	const float a = -0.525731112119133606;
	const float b = -0.850650808352039932;

	const TArray<FVector> StartVertices = {
		{-a, 0.0f, b}, {a, 0.0f, b}, {-a, 0.0f, -b}, {a, 0.0f, -b},
		{0.0f, b, a}, {0.0f, b, -a}, {0.0f, -b, a}, {0.0f, -b, -a},
		{b, a, 0.0f}, {-b, a, 0.0f}, {b, -a, 0.0f}, {-b, -a, 0.0f}
	};

	const TArray<int32> StartTriangles = {
		{0}, {1}, {4}, {0}, {4}, {9}, {9}, {4}, {5}, {4}, {8}, {5}, {4}, {1}, {8},
		{8}, {1}, {10}, {8}, {10}, {3}, {5}, {8}, {3}, {5}, {3}, {2}, {2}, {3}, {7},
		{7}, {3}, {10}, {7}, {10}, {6}, {7}, {6}, {11}, {11}, {6}, {0}, {0}, {6}, {1},
		{6}, {10}, {1}, {9}, {11}, {0}, {9}, {2}, {11}, {9}, {5}, {2}, {7}, {11}, {2}
	};

	TArray<FVector> TempVertices;
	TArray<int32> TempTriangles;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;

	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	//TArray<class FProcMeshTangent> Tangents;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
		class UProceduralMeshComponent* PlanetMesh;

	UPROPERTY(EditAnywhere, Category = "Planet")
		int subdivideIterations;
	UPROPERTY(EditAnywhere, Category = "Planet")
		bool loading;

	UPROPERTY(EditAnywhere, Category = "Shape Settings")
		int planetRadius;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
		float strength;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
		float baseRoughness;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
		float roughness;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
		int octaves;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
		float persistence;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
		float minValue;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
		FVector centre;

	float t = 0.0f;


	UPROPERTY(EditAnywhere)
		float rotationSpeed;

	UPROPERTY(EditAnywhere)
		float orbitSpeed;

	UPROPERTY(EditAnywhere)
		FVector rotationOffset;

	UPROPERTY(EditAnywhere)
		float angleAxis;

	UPROPERTY(EditAnywhere)
		FVector radius;

	UPROPERTY(EditAnywhere)
		FVector axisVector;

	AActor* waterSphere;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SubdivideMesh(int iterations);

	void SubdivideTriangle(FVector V1, FVector V2, FVector V3);

	FVector GetMiddlePoint(FVector V1, FVector V2);

	void SpawnMesh();

	void FixDuplicatedVertices();

	void SaveVertexData();

	void LoadVertexData();

	void SaveTriangleData();

	void LoadTriangleData();


	float Evaluate(FVector point);

	void BuildTerrain();

	void SetUpNormals();
	
	void SetUpUVs();

	void SpinPlanet(float Delta);

	void OrbitPlanet(float Delta);

	void UpdateWaterSpherePosition();

	void UpdateWaterSphereRotation();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector CalculatePointOnPlanet(FVector pointOnUnitSphere);
	void CreatePlanet(int _octaves, int _subdivideIterations, int _planetRadius, float _strength, float _baseRoughness, int _roughness, float _persistence, float _minValue, float _orbitSpeed, float _rotateSpeed, FVector _rotationOffset);
};

