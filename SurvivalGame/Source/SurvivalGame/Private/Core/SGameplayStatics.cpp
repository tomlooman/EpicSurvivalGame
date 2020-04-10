// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "Core/SGameplayStatics.h"
#include "SceneView.h"
#include "Engine/LocalPlayer.h"





bool USGameplayStatics::ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, 
	bool& bTargetBehindCamera, bool bPlayerViewportRelative /*= false*/)
{
	bTargetBehindCamera = false;

	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		{
			const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
			const FIntRect ViewRectangle = ProjectionData.GetConstrainedViewRect();

			FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
			if (Result.W < 0.f)
			{
				bTargetBehindCamera = true;
			}
			else if (Result.W == 0.f)
			{
				Result.W = 1.f; // Prevent Divide By Zero
			}

			const float RHW = 1.f / FMath::Abs(Result.W);
			FVector Projected = FVector(Result.X, Result.Y, Result.Z) * RHW;

			// Normalize to 0..1 UI Space
			const float NormX = (Projected.X / 2.f) + 0.5f;
			const float NormY = 1.f - (Projected.Y / 2.f) - 0.5f;

			Projected.X = (float)ViewRectangle.Min.X + (NormX * (float)ViewRectangle.Width());
			Projected.Y = (float)ViewRectangle.Min.Y + (NormY * (float)ViewRectangle.Height());

			ScreenPosition = FVector2D(Projected.X, Projected.Y);

			if (bPlayerViewportRelative)
			{
				ScreenPosition -= FVector2D(ProjectionData.GetConstrainedViewRect().Min);
			}

			return true;
		}
	}

	ScreenPosition = FVector2D::ZeroVector;
	return false;
}
