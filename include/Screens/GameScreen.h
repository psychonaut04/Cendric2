#pragma once

#include "global.h"
#include "Level.h"
#include "MainCharacter.h"
#include "Screen.h"
#include "ResourceManager.h"

class MainCharacter;

class GameScreen : public Screen
{
public:
	GameScreen(ResourceID levelID);
	~GameScreen();

	Screen* update(sf::Time frameTime) override;
	void render(sf::RenderTarget &renderTarget) override;

private:
	Level m_currentLevel;
	MainCharacter* m_mainChar;
};