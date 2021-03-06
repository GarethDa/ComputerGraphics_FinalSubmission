#include "BoomerangBehavior.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "HealthManager.h"

BoomerangBehavior::BoomerangBehavior()
{
}

BoomerangBehavior::~BoomerangBehavior()
{
}

void BoomerangBehavior::Awake()
{
	//Set up references
	_boomerangEntity = GetGameObject()->SelfRef();
	_rigidBody = GetGameObject()->Get<Gameplay::Physics::RigidBody>();
	_scene = GetGameObject()->GetScene();

	if (GetGameObject()->Name == "Boomerang 1") {
		_player = _scene->FindObjectByName("Player 1");
		_camera = GetGameObject()->GetScene()->PlayerCamera;
	}
	else
	{
		_player = _scene->FindObjectByName("Player 2");
		_camera = GetGameObject()->GetScene()->PlayerCamera2;
	}

	_rigidBody->SetMass(1);
}

void BoomerangBehavior::Update(float deltaTime)
{
	if (_state != boomerangState::INACTIVE) {
		updateTrackingPoint(deltaTime);
		defyGravity();
		Seek(deltaTime);
	}
}

void BoomerangBehavior::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat, "Launch Force", &_boomerangLaunchForce, 1.0f);
	LABEL_LEFT(ImGui::DragFloat, "Boomerang Acceleration", &_boomerangAcceleration, 1.0f);
}

void BoomerangBehavior::Seek(float deltaTime)
{
	glm::vec3 desiredVector = _targetPoint - _boomerangEntity->GetPosition();
	glm::vec3 currentVector = _rigidBody->GetLinearVelocity();

	desiredVector = glm::normalize(desiredVector);
	currentVector = glm::normalize(currentVector);

	glm::vec3 appliedVector = glm::normalize(desiredVector - currentVector);
	appliedVector = appliedVector * _boomerangAcceleration * deltaTime;
	_rigidBody->ApplyForce(appliedVector);
}

void BoomerangBehavior::throwWang(glm::vec3 playerPosition, float chargeLevel)
{
	_state = boomerangState::FORWARD;
	_distance = 1;
	glm::vec3 cameraLocalForward = glm::vec3(_camera->GetView()[0][2], _camera->GetView()[1][2], _camera->GetView()[2][2]) * -1.0f;
	_boomerangEntity->SetPosition(playerPosition + glm::vec3(0.0f, 0.0f, 1.5f) + cameraLocalForward * _projectileSpacing);
	_boomerangEntity->SetRotation(_player->GetRotationEuler());
	_rigidBody->SetLinearVelocity(glm::vec3(0));
	_rigidBody->SetAngularVelocity({ 0.f, 0.f, 900.f });
	_rigidBody->SetLinearVelocity(cameraLocalForward * _boomerangLaunchForce * chargeLevel);
}

void BoomerangBehavior::defyGravity()
{
	_rigidBody->ApplyForce(glm::vec3(0, 0, 12.81f));
}

void BoomerangBehavior::updateTrackingPoint(float deltaTime)
{
	//std::cout << "Updating Tracking Point" << std::endl;

	if (_state == boomerangState::FORWARD) {
		_distance += _distanceDelta;// *deltaTime;
	}
	else {
		_distance -= 1.33f * _distanceDelta;// *deltaTime;
	}

	glm::vec3 cameraLocalRot = glm::vec3(_camera->GetView()[0][2], _camera->GetView()[1][2], _camera->GetView()[2][2]);
	_targetPoint = _player->GetPosition() + glm::vec3(0.0f, 0.0f, 1.5f) + glm::normalize(cameraLocalRot) * -_distance;
	//std::cout << "X: " << _targetPoint.x << " Y: " << _targetPoint.y << " Z: " << _targetPoint.z << std::endl;

	if (glm::abs(_distance) < -0.01f) {
		makeBoomerangInactive();
	}
}

void BoomerangBehavior::returnBoomerang()
{
	_state = boomerangState::RETURNING;
}

void BoomerangBehavior::SetAcceleration(float newAccel)
{
	_boomerangAcceleration = newAccel;
}

void BoomerangBehavior::SetInactivePosition(glm::vec3 newPosition)
{
	_inactivePosition = newPosition;
}

glm::vec3 BoomerangBehavior::getPosition()
{
	return GetGameObject()->GetPosition();
}

bool BoomerangBehavior::getReadyToThrow()
{
	if (_state == boomerangState::INACTIVE) {
		return true;
	}
	return false;
}

bool BoomerangBehavior::isInactive()
{
	if (_state == boomerangState::INACTIVE)
	{
		return true;
	}
	else 
	return false;
}

void BoomerangBehavior::OnCollisionEnter()
{
	returnBoomerang();
}

void BoomerangBehavior::makeBoomerangInactive()
{
	_boomerangEntity->SetPosition(_inactivePosition);
	_state = boomerangState::INACTIVE;
	_rigidBody->SetLinearVelocity(glm::vec3(0, 0, 0));
	_rigidBody->SetAngularVelocity({ 0,0,0 });
}

nlohmann::json BoomerangBehavior::ToJson() const {
	return {
		{ "Launch Force", _boomerangLaunchForce }
	};
}

BoomerangBehavior::Sptr BoomerangBehavior::FromJson(const nlohmann::json& blob)
{
	BoomerangBehavior::Sptr result = std::make_shared<BoomerangBehavior>();
	result->_boomerangLaunchForce = blob["Launch Force"];
	return result;
}

bool BoomerangBehavior::getReturning()
{
	if (_state == boomerangState::RETURNING) {
		return true;
	}
	return false;
}