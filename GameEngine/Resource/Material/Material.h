#pragma once
#include "GameEngine/Resource/Material/Color.h"
#include "GameEngine/Math/Math.h"

struct Material {
	Color color;
	int32_t enableLighting;
	float paddding[3];
	Matrix4x4 uvTransform;
	float shininess;
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