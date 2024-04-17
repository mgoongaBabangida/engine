#include "stdafx.h"

#include "SettingsLoadingService.h"

#include <opengl_assets/OpenGlRenderpipeline.h>

#include "ModelManagerYAML.h"

#include <thread>
#include <sstream>
#include <fstream>
#include <future>

//---------------------------------------------------------------------
void SettingsLoadingService::LoadPipelineSettings(const std::string& _path, IGame* _game, eOpenGlRenderPipeline& _pipeline)
{
	std::ifstream infile(_path);
	if (infile.is_open())
	{
		std::stringstream sstream;
		std::copy(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(sstream));

		std::string name, value, end;
		while (!sstream.eof())
		{
			sstream >> name;
			sstream >> value;
			sstream >> end;

			if (name == "SwitchSkyBox")
				value == "true" ? _pipeline.SwitchSkyBox(true) : _pipeline.SwitchSkyBox(false);
			else if (name == "SwitchWater")
				value == "true" ? _pipeline.SwitchWater(true) : _pipeline.SwitchWater(false);
			else if (name == "SkyNoiseOn")
				value == "true" ? _pipeline.GetSkyNoiseOnRef() = true : _pipeline.GetSkyNoiseOnRef() = false;
			else if (name == "KernelOn")
				value == "true" ? _pipeline.GetKernelOnRef() = true : _pipeline.GetKernelOnRef() = false;
			else if (name == "UseGuizmo")
				value == "true" ? _game->UseGizmo() = true : _game->UseGizmo() = false;
			else if (name == "RotateSkyBox")
				value == "true" ? _pipeline.GetRotateSkyBoxRef() = true : _pipeline.GetRotateSkyBoxRef() = false;
			else if (name == "ShowFPS")
				value == "true" ? _game->ShowFPS() = true : _game->ShowFPS() = false;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void SettingsLoadingService::LoadModels(const std::string& _path, const std::string& _modelFolder, ModelManagerYAML* _modelManager, bool _multithreading)
{
	std::ifstream infile(_path);
	if (infile.is_open())
	{
		std::stringstream sstream;
		std::copy(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(sstream));

		std::vector<std::tuple<std::string, std::string, bool>> file_infos;
		std::string file_name, name, end;
		while (!sstream.eof())
		{
			sstream >> file_name;
			sstream >> name;
			sstream >> end;
			if (end == "true")
			{
				if (_multithreading)
				{
					file_infos.emplace_back(name, file_name, true);
				}
				else
				{
					_modelManager->Add(name, (GLchar*)std::string(_modelFolder + file_name).c_str(), true);
					sstream >> end;
				}
			}
			else
			{
				if (_multithreading)
					file_infos.emplace_back(name, file_name, false);
				else
					_modelManager->Add(name, (GLchar*)std::string(_modelFolder + file_name).c_str());
			}
		}

		if (_multithreading)
		{
			size_t thread_count = file_infos.size() > 4 ? 4 : file_infos.size();
			size_t step = file_infos.size() / thread_count;
			if (step * thread_count != file_infos.size())
				++step;
			std::vector<std::future<std::vector<IModel*>>> tasks;
			for (size_t i = 0; i < thread_count; ++i)
			{
				std::function<std::vector<IModel*>()> func = [_modelManager, _modelFolder, i, step, &file_infos, thread_count]()-> std::vector<IModel*>
				{
					std::vector<IModel*> models;
					size_t model_index = step * i;
					while (model_index < (i + 1) * step && model_index < file_infos.size())
					{
						models.push_back(_modelManager->Add(std::get<0>(file_infos[model_index]),
							(GLchar*)std::string(_modelFolder + std::get<1>(file_infos[model_index])).c_str(), std::get<2>(file_infos[model_index])));
						++model_index;
					}
					return models;
				};
				tasks.push_back(std::async(std::launch::async, func));
			}

			//wait for the tasks
			for (auto& fut : tasks) //textures are loaded in main thread only, need to load  them separately in main thread
			{
				std::vector<IModel*> models = fut.get();
				for (IModel* m : models)
				{
					m->SetUpMeshes();
					m->ReloadTextures();
				}
			}

		}
	}
}
