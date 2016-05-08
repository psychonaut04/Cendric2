#pragma once

#include "global.h"
#include "Map/MapDynamicTile.h"
#include "Structs/SignData.h"
#include "GUI/TooltipWindow.h"

// a readable sign
class SignMapTile : public MapDynamicTile {
public:
	SignMapTile(const SignData& data, MapScreen* mapScreen);

	void update(const sf::Time& frameTime) override;
	void renderAfterForeground(sf::RenderTarget& renderTarget) override;

	void init() override;
	void loadAnimation(int skinNr) override;
	void setPosition(const sf::Vector2f& pos) override;
	void onMouseOver() override;

	GameObjectType getConfiguredType() const override;

private:
	SignData m_data;
	TooltipWindow m_tooltipWindow;
	bool m_showTooltip;
};