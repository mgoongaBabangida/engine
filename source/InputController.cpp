#include "stdafx.h"
#include "InputController.h"

enum ASCII
{
	ASCII_W = 119,//87,
	ASCII_S = 115,//83
	ASCII_D = 100,//68,
	ASCII_A = 97, //65,
	ASCII_R = 114,//82,
	ASCII_F = 102,//70,
	ASCII_Q = 81,
	ASCII_J = 74,
	ASCII_L = 76,
	ASCII_K = 75,
	ASCII_I = 73,
	ASCII_Z = 90,
	ASCII_X = 88,
	ASCII_C = 67,
	ASCII_V = 86,
	ASCII_B = 66,
	ASCII_N = 78,
	ASCII_U = 85,
	ASCII_H = 72,
	ASCII_G = 71,
};

void eInputController::OnMouseMove(uint32_t x, uint32_t y)
{
	mainContext->GetCamera().mouseUpdate(glm::vec2(x, y));
}

void eInputController::OnKeyPress(uint32_t asci)
{
	//std::cout << "press" << std::endl;
	//std::cout <<"Camera= "<< m_camera.getPosition().x << " " << m_camera.getPosition().y << " " << m_camera.getPosition().z << " " << std::endl;
	switch (asci)
	{
	case ASCII_W:
	{
		mainContext->GetCamera().moveForward();
	}
	break;
	case ASCII_S:
	{
		mainContext->GetCamera().moveBackword();
	}
	break;
	case ASCII_D:
	{
		mainContext->GetCamera().strafeLeft();
	}
		break;
	case ASCII_A:
	{
		mainContext->GetCamera().strafeRight();
	}
		break;
	case ASCII_R:
	{
		mainContext->GetCamera().moveUp();
	}
		break;
	case ASCII_F:
	{
		mainContext->GetCamera().moveDown();
	}
		break;
	case ASCII_Q:
	{

	}
		break;
	case ASCII_J:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->MoveLeft(mainContext->GetObjects());
	}
	break;
	case ASCII_L:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->MoveRight(mainContext->GetObjects());
	}
	break;
	case ASCII_K:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->MoveBack(mainContext->GetObjects());
	}
	break;
	case ASCII_I:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->MoveForward(mainContext->GetObjects());
	}
	break;
	case ASCII_Z:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->MoveUp(mainContext->GetObjects());
	}
	break;
	case ASCII_X:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->MoveDown(mainContext->GetObjects());
	}
	break;
	case ASCII_C:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->TurnRight(mainContext->GetObjects());
	}
	break;
	case ASCII_V:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->TurnLeft(mainContext->GetObjects());
	}
	break;
	case ASCII_B:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->LeanRight(mainContext->GetObjects());
	}
	break;
	case ASCII_N:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->LeanLeft(mainContext->GetObjects());
	}
	break;
	case ASCII_U:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->LeanForward(mainContext->GetObjects());
	}
		break;
	case ASCII_H:
	{
		if (mainContext->GetFocusedObject() != nullptr)
			mainContext->GetFocusedObject()->LeanBack(mainContext->GetObjects());
	}
		break;
	case ASCII_G:
	{
		auto script = mainContext->GetFocusedObject()->getScript();
		if (script != nullptr)
			script->shoot();
	}
		break;
	}
}

void eInputController::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	//std::cout <<"e->x()"<< e->x() << " " << e->y() << std::endl;
	if(left)
	{
		for (auto& gui : mainContext->GetGuis())
		{
			if (gui.isPressed(x, y))
				gui.Perssed();
		}

		mainContext->GetCameraRay().press(x, y);
		mainContext->GetCameraRay().Update(mainContext->GetCamera(), x, y, mainContext->Width(), mainContext->Height());
		mainContext->GetFocusedObject() = mainContext->GetCameraRay().calculateIntersaction(mainContext->GetObjects());
	}
	else
	{
		dbb::plane pl(glm::vec3(1.0f, mainContext->WaterHeight(), 1.0f), 
					  glm::vec3(0.0f, mainContext->WaterHeight(), 0.0f),
					  glm::vec3(0.0f, mainContext->WaterHeight(), 1.0f)); // arbitrary triangle on waterHeight plane
		mainContext->GetCameraRay().Update(mainContext->GetCamera(), x, y, mainContext->Width(), mainContext->Height());
		glm::vec3 target = dbb::intersection(pl, mainContext->GetCameraRay().getLine());

		if(mainContext->GetFocusedObject() != nullptr)
		{
			mainContext->GetFocusedObject()->getScript()->setDestination(target);
		}
#ifdef DEBUG_HANDLERS
		std::cout << "TARGET=" << "x= " << target.x << "y= " << target.y << "z= " << target.z << std::endl;

		glm::vec3 position = glm::vec3(0.0f, 2.0f, 0.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 target_dir = glm::normalize(target - position);
		float angle = glm::dot(glm::normalize(target_dir), glm::normalize(direction));
		std::cout << "in press" << std::endl;
		std::cout << "dot= " << angle << "radians= " << glm::acos(angle) << " degrees= " << glm::degrees(glm::acos(angle)) << std::endl;
		glm::quat rot = glm::toQuat(glm::rotate(glm::mat4(), glm::acos(angle), glm::vec3(0, 1, 0)));

		glm::vec3 ASIX = glm::normalize(glm::cross(target_dir, direction));
		std::cout << "Asix=" << ASIX.x << ASIX.y << ASIX.z << std::endl;
#endif
	}
}

void eInputController::OnMouseRelease()
{
	mainContext->GetCameraRay().release();
}
