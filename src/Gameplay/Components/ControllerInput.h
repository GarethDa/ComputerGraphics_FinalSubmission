#pragma once
#include "IComponent.h"

struct GLFWwindow;

class ControllerInput : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<ControllerInput> Sptr;

	ControllerInput();
	virtual ~ControllerInput();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(ControllerInput);
	virtual nlohmann::json ToJson() const override;
	static ControllerInput::Sptr FromJson(const nlohmann::json& blob);

	bool IsValid();

	void SetController(int ID);

	bool GetButtonDown(int ID);

	float GetAxisValue(int ID);

	void SetEnabled(bool enabled);

	bool GetEnabled();

	bool GetButtonPressed(int ID);

	void ControllerInput::SetSensitivity(float inSensitivity);

	float ControllerInput::GetSensitivity();

	float ControllerInput::GetMinSensitivity();

	float ControllerInput::GetMaxSensitivity();

protected:
	bool _controllerConnected = false;
	int _controllerID;
	int _buttonCount;
	int _axesCount;

	bool _enabled;

	float _sensitivity;
	float _minSensitivity;
	float _maxSensitivity;

	std::vector<char> buttonList;
	std::vector<char> prevButtonList;

	GLFWwindow* _window;
};

