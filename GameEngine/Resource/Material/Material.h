#pragma once
#include "GameEngine/Resource/Material/Color.h"
#include "GameEngine/Math/Math.h"

struct Material {
	Color color;
	Matrix4x4 uvTransform;
	int32_t enableLighting;
	float shininess;
	float environmentCoefficient;
	float paddding;	
};

struct MultiMaterial {
	Material materialItem;
	std::string filepath;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
};

struct Material2D {
	Color color;
	Matrix4x4 uvTransform;
};