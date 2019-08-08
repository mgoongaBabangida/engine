#ifndef AMERICAN_TREASURE_H
#define AMERICAN_TREASURE_H

#include "MainContextBase.h"

class ePipeline;
class Camera;
namespace dbb { class CameraRay; }
class GUI;
class eShip;
class TerrainModel;

//*********************************************************************
// eAmericanTreasureGame
//*********************************************************************
class eAmericanTreasureGame : public eMainContextBase
{
public:
	eAmericanTreasureGame(eInputController*,
		IWindowImGui*,
		const std::string& modelsPath,
		const std::string& assetsPath,
		const std::string& shadersPath);
	virtual ~eAmericanTreasureGame() = default;

	virtual bool OnKeyPress(uint32_t asci)	override;

	virtual	void PaintGL()					override;

protected:
	virtual bool OnMousePress(uint32_t x, uint32_t y, bool left) override;
	virtual void InitializePipline()		override;
	virtual void InitializeBuffers()		override;
	virtual void InitializeModels()			override;
	virtual void InitializeRenders()		override;

	std::vector<std::shared_ptr<eShip>>	ships;
	std::shared_ptr<eShip>				focused;
	std::vector<shObject>				objects;
	std::unique_ptr<TerrainModel>		terrainModel;//keep somewhare else?
	
	std::unique_ptr<ePipeline>			pipeline;
	std::unique_ptr<Camera>				camera;
	std::unique_ptr<dbb::CameraRay>		camRay;
	std::vector<GUI>					guis;
	Light								light;
	float								waterHeight = 2.0f;
	float								angle		= 0.0f;
};

#endif //AMERICAN_TREASURE_H
