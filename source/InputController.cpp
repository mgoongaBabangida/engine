#include "InputController.h"

void eInputController::OnMouseMove(uint x, uint y)
{
	mainContext->GetCamera().mouseUpdate(glm::vec2(x, y));
}

void eInputController::OnKeyPress(uint asci)
{
	//std::cout << "press" << std::endl;
	//std::cout <<"Camera= "<< m_camera.getPosition().x << " " << m_camera.getPosition().y << " " << m_camera.getPosition().z << " " << std::endl;
	switch(asci)
	{
	case 87://W
		mainContext->GetCamera().moveForward();
		break;
	case 83://S
		mainContext->GetCamera().moveBackword();
		break;
	case Qt::Key::Key_D:
		mainContext->GetCamera().strafeLeft();
		break;
	case Qt::Key::Key_A:
		mainContext->GetCamera().strafeRight();
		break;
	case Qt::Key::Key_R:
		mainContext->GetCamera().moveUp();
		break;
	case Qt::Key::Key_F:
		mainContext->GetCamera().moveDown();
		break;
	case Qt::Key::Key_Q:

		break;

	case Qt::Key::Key_J:
		//if(m_focused!=nullptr)
		mainContext->GetFocusedObject()->MoveLeft(mainContext->GetObjects());
		break;
	case Qt::Key::Key_L:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->MoveRight(mainContext->GetObjects());
		break;
	case Qt::Key::Key_K:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->MoveBack(mainContext->GetObjects());
		break;
	case Qt::Key::Key_I:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->MoveForward(mainContext->GetObjects());
		break;
	case Qt::Key::Key_Z:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->MoveUp(mainContext->GetObjects());
		break;
	case Qt::Key::Key_X:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->MoveDown(mainContext->GetObjects());
		break;
	case Qt::Key::Key_C:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->TurnRight(mainContext->GetObjects());
		break;
	case Qt::Key::Key_V:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->TurnLeft(mainContext->GetObjects());
		break;
	case Qt::Key::Key_B:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->LeanRight(mainContext->GetObjects());
		break;
	case Qt::Key::Key_N:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->LeanLeft(mainContext->GetObjects());
		break;
	case Qt::Key::Key_U:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->LeanForward(mainContext->GetObjects());
		break;
	case Qt::Key::Key_H:
		//if (m_focused != nullptr)
		mainContext->GetFocusedObject()->LeanBack(mainContext->GetObjects());
		break;
	case Qt::Key::Key_G:
	{
		auto script = mainContext->GetFocusedObject()->getScript();
		if (script != nullptr)
			script->shoot();
		break;
	}
	}
}

void eInputController::OnMousePress(uint x, uint y, bool left)
{
	//std::cout <<"e->x()"<< e->x() << " " << e->y() << std::endl;
	if(left)
	{
		for (auto& gui : mainContext->GetGuis())
		{
			if (gui.isPressed(x, y))
				gui.Perssed();
		}

		mainContext->GetCameraRey().press(x, y);
		mainContext->GetCameraRey().Update(mainContext->GetCamera(), x, y, mainContext->Width(), mainContext->Height());
		mainContext->GetFocusedObject() = mainContext->GetCameraRey().calculateIntersaction(mainContext->GetObjects());
	}
	else
	{
		dbb::plane pl(glm::vec3(1.0f, mainContext->WaterHeight(), 1.0f), glm::vec3(0.0f, mainContext->WaterHeight(), 0.0f), glm::vec3(0.0f, mainContext->WaterHeight(), 1.0f)); // arbitrary triangle on waterHeight plane
		mainContext->GetCameraRey().Update(mainContext->GetCamera(), x, y, mainContext->Width(), mainContext->Height());
		glm::vec3 target = dbb::intersection(pl, mainContext->GetCameraRey().getLine());

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
	mainContext->GetCameraRey().release();
}
