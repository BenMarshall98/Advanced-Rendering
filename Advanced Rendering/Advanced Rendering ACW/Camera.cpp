#include "pch.h"
#include "Camera.h"

namespace Advanced_Rendering
{

	Camera::Camera(const DirectX::XMFLOAT3 & pEyePosition, const DirectX::XMFLOAT3 & pUpDirection,
		const DirectX::XMFLOAT3 & pTargetPosition) : mViewMatrix(), mUpDirection(pUpDirection),
		mEyePosition(pEyePosition), mTargetPosition(pTargetPosition)
	{
	}

	void Camera::rotateLeftRight(const bool pLeft, DX::StepTimer const & pTimer)
	{
		auto angleChange = mAngleSpeed * pTimer.GetElapsedSeconds();

		if (!pLeft)
		{
			angleChange = -angleChange;
		}

		const auto targetTemp = XMLoadFloat3(&mTargetPosition);
		const auto eyeTemp = XMLoadFloat3(&mEyePosition);
		const auto upTemp = XMLoadFloat3(&mUpDirection);

		const auto zAxis = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetTemp, eyeTemp));
		const auto xAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(zAxis, upTemp));
		const auto yAxis = DirectX::XMVector3Cross(zAxis, xAxis);

		const auto leftRightMat = DirectX::XMMatrixRotationNormal(yAxis, angleChange);

		XMStoreFloat3(&mTargetPosition, DirectX::XMVectorAdd(eyeTemp, XMVector3Transform(zAxis, leftRightMat)));
	}

	void Camera::rotateUpDown(const bool pUp, DX::StepTimer const & pTimer)
	{
		auto angleChange = mAngleSpeed * pTimer.GetElapsedSeconds();

		if (!pUp)
		{
			angleChange = -angleChange;
		}

		const auto targetTemp = XMLoadFloat3(&mTargetPosition);
		const auto eyeTemp = XMLoadFloat3(&mEyePosition);
		const auto upTemp = XMLoadFloat3(&mUpDirection);

		const auto zAxis = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetTemp, eyeTemp));
		const auto xAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(zAxis, upTemp));

		const auto leftRightMat = DirectX::XMMatrixRotationNormal(xAxis, angleChange);

		XMStoreFloat3(&mTargetPosition, DirectX::XMVectorAdd(eyeTemp, XMVector3Transform(zAxis, leftRightMat)));
		XMStoreFloat3(&mUpDirection, XMVector3Transform(upTemp, leftRightMat));
	}

	void Camera::panLeftRight(const bool pLeft, DX::StepTimer const & pTimer)
	{
		auto movementChange = mMovementSpeed * pTimer.GetElapsedSeconds();

		if (!pLeft)
		{
			movementChange = -movementChange;
		}

		const auto targetTemp = XMLoadFloat3(&mTargetPosition);
		const auto eyeTemp = XMLoadFloat3(&mEyePosition);
		const auto upTemp = XMLoadFloat3(&mUpDirection);

		const auto zAxis = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetTemp, eyeTemp));
		const auto xAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(zAxis, upTemp));

		const auto movement = DirectX::XMVectorScale(xAxis, movementChange);

		XMStoreFloat3(&mEyePosition, DirectX::XMVectorAdd(eyeTemp, movement));
		XMStoreFloat3(&mTargetPosition, DirectX::XMVectorAdd(targetTemp, movement));
	}

	void Camera::panForwardBackward(const bool pForward, DX::StepTimer const & pTimer)
	{
		auto movementChange = mMovementSpeed * pTimer.GetElapsedSeconds();

		if (!pForward)
		{
			movementChange = -movementChange;
		}

		const auto targetTemp = XMLoadFloat3(&mTargetPosition);
		const auto eyeTemp = XMLoadFloat3(&mEyePosition);

		const auto zAxis = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetTemp, eyeTemp));

		const auto movement = DirectX::XMVectorScale(zAxis, movementChange);

		XMStoreFloat3(&mEyePosition, DirectX::XMVectorAdd(eyeTemp, movement));
		XMStoreFloat3(&mTargetPosition, DirectX::XMVectorAdd(targetTemp, movement));
	}

	void Camera::panUpDown(const bool pUp, DX::StepTimer const & pTimer)
	{
		auto movementChange = mMovementSpeed * pTimer.GetElapsedSeconds();

		if (!pUp)
		{
			movementChange = -movementChange;
		}

		const auto targetTemp = XMLoadFloat3(&mTargetPosition);
		const auto eyeTemp = XMLoadFloat3(&mEyePosition);
		const auto upTemp = XMLoadFloat3(&mUpDirection);

		const auto zAxis = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetTemp, eyeTemp));
		const auto xAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(zAxis, upTemp));
		const auto yAxis = DirectX::XMVector3Cross(zAxis, xAxis);

		const auto movement = DirectX::XMVectorScale(yAxis, movementChange);

		XMStoreFloat3(&mEyePosition, DirectX::XMVectorAdd(eyeTemp, movement));
		XMStoreFloat3(&mTargetPosition, DirectX::XMVectorAdd(targetTemp, movement));
	}

	void Camera::update(ModelViewProjectionConstantBuffer & pCameraBuffer, DX::StepTimer const & pTimer)
	{
		if (mRotateLeft && !mRotateRight)
		{
			rotateLeftRight(true, pTimer);
		}
		else if (!mRotateLeft && mRotateRight)
		{
			rotateLeftRight(false, pTimer);
		}

		if (mRotateUp && !mRotateDown)
		{
			rotateUpDown(true, pTimer);
		}
		else if (!mRotateUp && mRotateDown)
		{
			rotateUpDown(false, pTimer);
		}

		if (mPanLeft && !mPanRight)
		{
			panLeftRight(true, pTimer);
		}
		else if (!mPanLeft && mPanRight)
		{
			panLeftRight(false, pTimer);
		}

		if (mPanUp && !mPanDown)
		{
			panUpDown(true, pTimer);
		}
		else if (!mPanUp && mPanDown)
		{
			panUpDown(false, pTimer);
		}

		if (mPanForward && !mPanBackward)
		{
			panForwardBackward(true, pTimer);
		}
		else if (!mPanForward && mPanBackward)
		{
			panForwardBackward(false, pTimer);
		}

		const auto targetTemp = XMLoadFloat3(&mTargetPosition);
		const auto eyeTemp = XMLoadFloat3(&mEyePosition);
		const auto upTemp = XMLoadFloat3(&mUpDirection);
		XMStoreFloat4x4(&mViewMatrix, DirectX::XMMatrixLookAtRH(eyeTemp, targetTemp, upTemp));

		DirectX::XMStoreFloat4x4(&pCameraBuffer.view, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&mViewMatrix)));
		pCameraBuffer.eyePosition = DirectX::XMFLOAT4(mEyePosition.x, mEyePosition.y, mEyePosition.z, 1.0f);
	}
}