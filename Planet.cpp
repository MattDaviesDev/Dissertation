// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"
#include "ProceduralMeshComponent.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Misc/DefaultValueHelper.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComponent;

	PlanetMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Planet Mesh"));
	PlanetMesh->SetupAttachment(SceneComponent);

	if (subdivideIterations > 7)
	{
		subdivideIterations = 7;
	}

}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
}

void APlanet::CreatePlanet(int _octaves, int _subdivideIterations, int _planetRadius, float _strength, float _baseRoughness, int _roughness, float _persistence, float _minValue, float _orbitSpeed, float _rotateSpeed, FVector _rotationOffset)
{
	octaves = _octaves;
	subdivideIterations = _subdivideIterations;
	if (subdivideIterations > 7)
	{
		subdivideIterations = 7;
	}
	planetRadius = _planetRadius;
	strength = _strength;
	baseRoughness = _roughness;
	roughness = _roughness;
	persistence = _persistence;
	minValue = _minValue;
	rotationSpeed = _rotateSpeed;
	rotationOffset = _rotationOffset;
	orbitSpeed = _orbitSpeed;
	loading = true;
	FString timeStart = FDateTime::Now().ToString();
	if (loading)
	{
		LoadVertexData();
		LoadTriangleData();
	}
	else
	{
		Vertices = StartVertices;
		Triangles = StartTriangles;
		SubdivideMesh(subdivideIterations);
		FixDuplicatedVertices();
		SaveVertexData();
		SaveTriangleData();
	}
	SetUpNormals();
	SetUpUVs();
	FString timeEnd = FDateTime::Now().ToString();
	//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Green, FString(TEXT("Started at: " + timeStart + ", Finished at: " + timeEnd)));
	//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Green, FString(FString::FromInt(Vertices.Num()) + " verts saved. " + FString::FromInt(Triangles.Num() / 3) + " Triangles saved"));

	//RandomiseAllProperties();

	BuildTerrain();

	SpawnMesh();
	
	FRotator planetRotationAxis = FRotator(_rotationOffset.X, _rotationOffset.Y, _rotationOffset.Z);

	FQuat quatRot = FQuat(planetRotationAxis);

	AddActorLocalRotation(quatRot, false, 0, ETeleportType::TeleportPhysics);

	radius = FVector(FVector::Dist(GetActorLocation(), FVector::ZeroVector), 0, 0);

}

void APlanet::SubdivideMesh(int iterations)
{
	for (int i = 0; i < iterations; i++)
	{
		int TriCount = Triangles.Num();
		int VertCount = Vertices.Num();
		for (int j = 0; j < TriCount; j += 3)
		{
			int32 V1 = Triangles[j];
			int32 V2 = Triangles[j + 1];
			int32 V3 = Triangles[j + 2];
			SubdivideTriangle(Vertices[V1], Vertices[V2], Vertices[V3]);
		} 
		Triangles = TempTriangles;
		Vertices = TempVertices;
		TempTriangles.Empty(0);
		TempVertices.Empty(0);
	}
}

void APlanet::SubdivideTriangle(FVector V1, FVector V2, FVector V3)
{
	FVector V4 = GetMiddlePoint(V1, V2).GetSafeNormal();
	FVector V5 = GetMiddlePoint(V2, V3).GetSafeNormal();
	FVector V6 = GetMiddlePoint(V3, V1).GetSafeNormal();

	int32 VertexCount = TempVertices.Num() - 1;
	// add new Verticies and old vertices
	TempVertices.Push(V1);
	TempVertices.Push(V2);
	TempVertices.Push(V3);
	TempVertices.Push(V4);
	TempVertices.Push(V5);
	TempVertices.Push(V6);

	// Add triangles
	// tri 1
	TempTriangles.Push(VertexCount + 1);
	TempTriangles.Push(VertexCount + 4);
	TempTriangles.Push(VertexCount + 6);

	// tri 2
	TempTriangles.Push(VertexCount + 4);
	TempTriangles.Push(VertexCount + 2);
	TempTriangles.Push(VertexCount + 5);

	// tri 3
	TempTriangles.Push(VertexCount + 4);
	TempTriangles.Push(VertexCount + 5);
	TempTriangles.Push(VertexCount + 6);

	// tri 4
	TempTriangles.Push(VertexCount + 6);
	TempTriangles.Push(VertexCount + 5);
	TempTriangles.Push(VertexCount + 3);
}

FVector APlanet::GetMiddlePoint(FVector V1, FVector V2)
{
	return V1 + (V2 - V1) * 0.5f;
}

void APlanet::SpawnMesh()
{
	//PlanetMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	PlanetMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, Colors, TArray<FProcMeshTangent>(), true);
}

void APlanet::FixDuplicatedVertices()
{
	// First fix all triangle indexes, so triangles that are using different vertices on the same world co-ordinate
	// should now be using the same vertex.
	for (int i = 0; i < Vertices.Num(); i++)
	{
		FVector CurrentVert = Vertices[i];
		for (int j = 0; j < Vertices.Num(); j++)
		{
			if (i != j) 
			{
				if (Vertices[j] == CurrentVert) 
				{
					for (int k = 0; k < Triangles.Num(); k++)
					{
						if (Triangles[k] == j) 
						{
							Triangles[k] = i;
						}
					}
				}
			}
		}
	}
	// Fill the TempVertices array with the new list of vertices, ensuring there are no duplicates
	for (int i = 0; i < Triangles.Num(); i++)
	{
		FVector Currentvert = Vertices[Triangles[i]];
		bool duplicate = false;
		for (int j = 0; j < TempVertices.Num(); j++)
		{
			if (TempVertices[j] == Currentvert) 
			{
				duplicate = true;
				break;
			}
		}
		if (!duplicate)
		{
			TempVertices.Push(Currentvert);
		}
	}
	// Create the new Triangle indexes using the new Vertex list
	for (int i = 0; i < Triangles.Num(); i++)
	{
		for (int j = 0; j < TempVertices.Num(); j++)
		{
			if (TempVertices[j] == Vertices[Triangles[i]]) 
			{
				TempTriangles.Push(j);
			}
		}
	}
	// Reset the Triangles and Vertices arrays and free up the memory for the temporary arrays
	Triangles = TempTriangles;
	Vertices = TempVertices;
	TempTriangles.Empty(0);
	TempVertices.Empty(0);
	// Print the Triangle and Vertex counts on screen for testing purposes.
	FString vertices = "";
	vertices.AppendInt(Vertices.Num());
	FString triangles = "";
	triangles.AppendInt(Triangles.Num() / 3);
	GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Green, FString(TEXT("Vertex count: " + vertices + ", Tri count: " + triangles)));
}

void APlanet::SaveVertexData()
{
	TArray<FString> array;
	FString fileName = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "/Content/Vertices/VertexData";
	fileName.AppendInt(subdivideIterations);
	fileName.Append(".txt");
	for (size_t i = 0; i < Vertices.Num(); i++)
	{
		array.Push(FString::SanitizeFloat(Vertices[i].X));
		array.Push(FString::SanitizeFloat(Vertices[i].Y));
		array.Push(FString::SanitizeFloat(Vertices[i].Z));
	}
	FFileHelper::SaveStringArrayToFile(array, *fileName);
}

void APlanet::LoadVertexData()
{
	TArray<FString> array;
	
	// Create the file path
	FString fileName = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) 
		+ "/Content/Vertices/VertexData";
	fileName.AppendInt(subdivideIterations);
	fileName.Append(".txt");

	//Store each line of the text file to an array
	FFileHelper::LoadFileToStringArray(array, *fileName);

	// Loop over the array and convert the strings to floats
	for (size_t i = 0; i < array.Num(); i += 3)
	{
		FVector temp = FVector(FCString::Atof(*array[i]), 
			FCString::Atof(*array[i + 1]), FCString::Atof(*array[i + 2]));
		Vertices.Push(temp);
	}
}

void APlanet::SaveTriangleData()
{
	TArray<FString> array;
	FString fileName = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "/Content/Triangles/TriangleData";
	fileName.AppendInt(subdivideIterations);
	fileName.Append(".txt");
	for (size_t i = 0; i < Triangles.Num(); i++)
	{
		array.Push(FString::FromInt(Triangles[i]));
	}
	FFileHelper::SaveStringArrayToFile(array, *fileName);
}

void APlanet::LoadTriangleData()
{
	TArray<FString> array;
	FString fileName = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "/Content/Triangles/TriangleData";
	fileName.AppendInt(subdivideIterations);
	fileName.Append(".txt");
	FFileHelper::LoadFileToStringArray(array, *fileName);
	for (size_t i = 0; i < array.Num(); i++)
	{
		int triIndex;
		FDefaultValueHelper::ParseInt(*array[i], triIndex);
		Triangles.Push(triIndex);
	}
}

FVector APlanet::CalculatePointOnPlanet(FVector pointOnUnitSphere)
{
	float elevation = Evaluate(pointOnUnitSphere);
	//return pointOnUnitSphere * planetRadius; // icosphere
	return pointOnUnitSphere * planetRadius * (1 + elevation); //icoplanet
}

float APlanet::Evaluate(FVector point)
{
	float noiseValue = 0.0f;
	float frequency = baseRoughness;
	float amplitude = 1;
	
	for (int i = 0; i < octaves; i++)
	{
		float v = FMath::PerlinNoise3D(point * frequency + centre);
		noiseValue += (v + 1) * 0.5f * amplitude;
		frequency *= roughness;
		amplitude *= persistence;
	}

	return noiseValue * strength;
}

void APlanet::BuildTerrain()
{
	for (int i = 0; i < Vertices.Num(); i++)
	{
		Vertices[i] = CalculatePointOnPlanet(Vertices[i]);
	}
}

void APlanet::SetUpNormals()
{
	for (int i = 0; i < Triangles.Num(); i+=3)
	{
		FVector thisNorm = FVector::CrossProduct(Vertices[Triangles[i]], Vertices[Triangles[i + 2]]).GetSafeNormal();
		for (int j = 0; j < 3; j++)
		{
			Normals.Add(thisNorm);
			Colors.Add(FLinearColor::White);
		}
	}
}

void APlanet::SetUpUVs()
{
	for (int i = 0; i < Triangles.Num(); i += 3)
	{
		UVs.Add(FVector2D(1.f, 0.f));
		UVs.Add(FVector2D(0.f, 1.f));
		UVs.Add(FVector2D(0.f, 0.f));
	}
}

void APlanet::SpinPlanet(float Delta)
{
	SceneComponent->AddLocalRotation(FRotator(rotationOffset.X, rotationOffset.Y, rotationOffset.Z) * rotationSpeed * Delta);
}

void APlanet::OrbitPlanet(float Delta)
{
	FVector newLocation = FVector::ZeroVector;

	angleAxis += orbitSpeed * Delta;

	if (angleAxis >= 360.f) 
	{
		angleAxis = 0.f;
	}

	FVector rotateValue = radius.RotateAngleAxis(angleAxis, FVector(0, 0, 1));

	newLocation.X += rotateValue.X;
	newLocation.Y += rotateValue.Y;
	newLocation.Z += rotateValue.Z;

	SetActorLocation(newLocation);
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpinPlanet(DeltaTime);

	OrbitPlanet(DeltaTime);

	UpdateWaterSpherePosition();

	UpdateWaterSphereRotation();
}

void APlanet::UpdateWaterSpherePosition() 
{
	if (waterSphere != nullptr) 
	{
		waterSphere->SetActorLocation(GetActorLocation());
	}
}

void APlanet::UpdateWaterSphereRotation()
{
	if (waterSphere != nullptr)
	{
		waterSphere->SetActorRotation(GetActorRotation());
	}
}