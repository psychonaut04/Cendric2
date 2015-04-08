#pragma once

#include "global.h"
#include "ResourceManager.h"
#include "Screen.h"
#include "LoadingScreen.h"

class MenuScreen : public Screen
{
public:
	MenuScreen();

	Screen* update(sf::Time frameTime) override;
	void render(sf::RenderTarget &renderTarget) override;

	virtual void onEnter(Screen *previousScreen);
	virtual void onExit(Screen *nextScreen);

private:
	sf::Sprite m_screenSprite;
};