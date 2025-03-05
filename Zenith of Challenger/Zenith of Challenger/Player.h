#pragma once

#include "object.h"
#include "camera.h"

class Player : public InstanceObject
{
public:
	Player();
	~Player() override = default;

	void MouseEvent(FLOAT timeElapsed);
	void KeyboardEvent(FLOAT timeElapsed);
	virtual void Update(FLOAT timeElapsed) override;

	void Move(XMFLOAT3 direction, FLOAT speed);  // 이동 로직 추가

	void SetCamera(const shared_ptr<Camera>& camera);
	shared_ptr<Camera> GetCamera() const { return m_camera; }  // 추가

	void SetScale(XMFLOAT3 scale);
	XMFLOAT3 GetScale() const;

private:
	shared_ptr<Camera> m_camera;

	FLOAT m_speed;
};