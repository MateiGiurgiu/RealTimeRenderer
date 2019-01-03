#pragma once
#include "pch.h"

class IRenderable
{
public:
	virtual ~IRenderable() = default;

	IRenderable(const IRenderable&) = delete;
	IRenderable& operator=(const IRenderable&) = delete;

	IRenderable(IRenderable&&) = delete;
	IRenderable& operator=(IRenderable&&) = delete;

	virtual void Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) = 0;

protected:
	IRenderable() = default;
};

