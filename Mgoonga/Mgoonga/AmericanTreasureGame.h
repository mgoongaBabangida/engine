#ifndef AMERICAN_TREASURE_H
#define AMERICAN_TREASURE_H

#include <base/Object.h>

#include <sdl_assets/MainContextBase.h>

class ePipeline;
class Camera;
namespace dbb { class CameraRay; }
class GUI;
class eShip;
class eBase;
class eTerrain;
class TerrainModel;
class Hex;
class IWindowImGui;
class eTurnController;

//*********************************************************************
// eAmericanTreasureGame
//*********************************************************************
class eAmericanTreasureGame : public eMainContextBase
{
public:
	friend class eTurnController;

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

	void		_InitializeHexes();

	std::shared_ptr<eTurnController>	turn_context;
	std::vector<std::shared_ptr<eShip>>	ships;
	std::vector<std::shared_ptr<eBase>>	bases;
	std::shared_ptr<eShip>				focused;
	std::vector<shObject>				objects;
	std::unique_ptr<TerrainModel>		terrainModel;
	std::shared_ptr<eTerrain>			terrain;
	
	std::unique_ptr<ePipeline>			pipeline;
	std::unique_ptr<Camera>				camera;
	std::unique_ptr<dbb::CameraRay>		camRay;
	std::vector<GUI>					guis;
	std::vector<Hex>					hexes;
	Light								light;
	float								waterHeight = 2.0f;
	float								angle		= 0.0f;

	IWindowImGui*						gui;
};

#endif //AMERICAN_TREASURE_H
