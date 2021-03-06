#pragma once

#include "global.h"
#include "Level/Enemy.h"
#include "Level/Level.h"
#include "Spells/SpellManager.h"
#include "Screens/Screen.h"
#include "GameObjectComponents/LightComponent.h"

class SkeletonElementalEnemy final : public Enemy {
public:
	SkeletonElementalEnemy(const Level* level, Screen* screen);

	int getMentalStrength() const override;

	void insertDefaultLoot(std::map<std::string, int>& loot, int& gold) const override;
	void insertRespawnLoot(std::map<std::string, int>& loot, int& gold) const override;
	void setDead() override;
	void update(const sf::Time& frameTime) override;

	EnemyID getEnemyID() const override { return EnemyID::Skeleton_Elemental; }
	float getConfiguredDistanceToHPBar() const override { return 40.f; }

protected:
	std::string getSpritePath() const override;
	MovingBehavior* createMovingBehavior(bool asAlly) override;
	AttackingBehavior* createAttackingBehavior(bool asAlly) override;
	void handleAttackInput();
	// loads attributes and adds immune spells + enemies. all attributes are set to zero before that call. default does nothing.
	void loadAttributes() override;
	// loads spells and adds them to the spell manager. default does nothing.
	void loadSpells() override;
	void loadAnimation(int skinNr) override;

	LightComponent* m_lightComponent = nullptr;
};