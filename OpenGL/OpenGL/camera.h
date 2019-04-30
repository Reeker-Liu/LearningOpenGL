#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum move_t {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
};

//dafault camera value
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 WORLDUP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);

class Camera
{
public:
	//attributes
	glm::vec3 position;
	glm::vec3 worldUp;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	//euler angles
	float yaw;
	float pitch;
	//options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;
	bool lockedY;

	Camera(glm::vec3 _position = POSITION, glm::vec3 _worldUp = WORLDUP, float _yaw = YAW, float _pitch = PITCH, bool _lockedY = false)
		: front(Front), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM),
		position(_position), worldUp(_worldUp), yaw(_yaw), pitch(_pitch), lockedY(_lockedY)
	{
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	void processKeyBoard(move_t direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		switch (direction)
		{
		case FORWARD:
			position += front * velocity;
			break;
		case BACKWARD:
			position -= front * velocity;
			break;
		case LEFT:
			position -= right * velocity;
			break;
		case RIGHT:
			position += right * velocity;
			break;
		}
		if (lockedY)
			position.y = 0;
	}

	void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float yOffset)
	{
		if (zoom >= 1.0f && zoom <= 45.0f)
			zoom -= yOffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 45.0f)
			zoom = 45.0f;
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 _front;
		_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		_front.y = sin(glm::radians(pitch));
		_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(_front);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};