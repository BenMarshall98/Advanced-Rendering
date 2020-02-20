#pragma once

#include <DirectXMath.h>
#include "Content/ShaderStructures.h"
#include "../Common/StepTimer.h"

namespace Advanced_Rendering
{
	class Camera
	{
		DirectX::XMFLOAT4X4 mViewMatrix;
		DirectX::XMFLOAT3 mUpDirection;
		DirectX::XMFLOAT3 mEyePosition;
		DirectX::XMFLOAT3 mTargetPosition;
		float mAngleSpeed = 1.0f;
		float mMovementSpeed = 10.0f;

		bool mRotateLeft = false;
		bool mRotateRight = false;
		bool mRotateUp = false;
		bool mRotateDown = false;
		bool mPanLeft = false;
		bool mPanRight = false;
		bool mPanUp = false;
		bool mPanDown = false;
		bool mPanForward = false;
		bool mPanBackward = false;

		void rotateLeftRight(bool pLeft, DX::StepTimer const& pTimer);
		void rotateUpDown(bool pUp, DX::StepTimer const & pTimer);
		void panLeftRight(bool pLeft, DX::StepTimer const & pTimer);
		void panForwardBackward(bool pForward, DX::StepTimer const & pTimer);
		void panUpDown(bool pUp, DX::StepTimer const & pTimer);

	public:
		Camera(const DirectX::XMFLOAT3 & pEyePosition, const DirectX::XMFLOAT3 & pUpDirection,
			const DirectX::XMFLOAT3 & pTargetPosition);
		~Camera() = default;
		Camera(const Camera&) = delete;
		Camera(Camera &&) = delete;
		Camera & operator= (const Camera &) = delete;
		Camera & operator= (Camera &&) = delete;

		void rotateLeft(const bool & pRotate)
		{
			mRotateLeft = pRotate;
		}

		void rotateRight(const bool & pRotate)
		{
			mRotateRight = pRotate;
		}

		void rotateUp(const bool & pRotate)
		{
			mRotateUp = pRotate;
		}

		void rotateDown(const bool & pRotate)
		{
			mRotateDown = pRotate;
		}

		void panLeft(const bool & pPan)
		{
			mPanLeft = pPan;
		}

		void panRight(const bool & pPan)
		{
			mPanRight = pPan;
		}

		void panUp(const bool & pPan)
		{
			mPanUp = pPan;
		}

		void panDown(const bool & pPan)
		{
			mPanDown = pPan;
		}

		void panForward(const bool & pPan)
		{
			mPanForward = pPan;
		}

		void panBackward(const bool & pPan)
		{
			mPanBackward = pPan;
		}

		void update(ModelViewProjectionConstantBuffer & pCameraBuffer, DX::StepTimer const & pTimer);

		void getViewPosition(DirectX::XMFLOAT3 & pViewPosition) const
		{
			pViewPosition = mEyePosition;
		}
	};
}
