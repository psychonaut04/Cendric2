#include "Level/Enemies/SeagullEnemy.h"
#include "Level/LevelMainCharacter.h"
#include "Level/MOBBehavior/MovingBehaviors/AggressiveFlyingBehavior.h"
#include "Level/MOBBehavior/MovingBehaviors/AllyFlyingBehavior.h"
#include "Level/MOBBehavior/AttackingBehaviors/AggressiveBehavior.h"
#include "Level/MOBBehavior/AttackingBehaviors/AllyBehavior.h"
#include "Registrar.h"

REGISTER_ENEMY(EnemyID::Seagull, SeagullEnemy)

void SeagullEnemy::insertDefaultLoot(std::map<std::string, int>& loot, int& gold) const {
	if (gold != 0 || !loot.empty()) return;

	int feathers = rand() % 3;
	if (feathers > 0)
		loot.insert({ "mi_feather", feathers });
}

SeagullEnemy::SeagullEnemy(Level* level, Screen* screen) :
	LevelMovableGameObject(level),
    Enemy(level, screen) {
	load(EnemyID::Seagull);
}

void SeagullEnemy::loadAttributes() {
	m_attributes.setHealth(50);
	m_attributes.resistancePhysical = 10;
	m_attributes.calculateAttributes();
}

void SeagullEnemy::loadSpells() {
	SpellData chopSpell = SpellData::getSpellData(SpellID::Chop);
	chopSpell.activeDuration = sf::milliseconds(500);
	chopSpell.cooldown = sf::milliseconds(1000);
	chopSpell.damage = 2;
	chopSpell.damagePerSecond = 10;
	chopSpell.duration = sf::seconds(3.f);
	chopSpell.boundingBox = sf::FloatRect(10, 0, 30, 30);

	m_spellManager->addSpell(chopSpell);
	m_spellManager->setCurrentSpell(0);
}

sf::Vector2f SeagullEnemy::getConfiguredSpellOffset() const {
	return sf::Vector2f(-10.f, 0.f);
}

MovingBehavior* SeagullEnemy::createMovingBehavior(bool asAlly) {
	FlyingBehavior* behavior;
	if (asAlly) {
		behavior = new AllyFlyingBehavior(this);
	}
	else {
		behavior = new AggressiveFlyingBehavior(this);
	}
	behavior->setApproachingDistance(10.f);
	behavior->setMaxVelocityYDown(200.f);
	behavior->setMaxVelocityYUp(100.f);
	behavior->setMaxVelocityX(100.f);
	behavior->setFightAnimationTime(sf::milliseconds(3 * 100));
	return behavior;
}

AttackingBehavior* SeagullEnemy::createAttackingBehavior(bool asAlly) {
	EnemyAttackingBehavior* behavior;
	if (asAlly) {
		behavior = new AllyBehavior(this);
	}
	else {
		behavior = new AggressiveBehavior(this);
	}
	behavior->setAggroRange(400.f);
	behavior->setAttackInput(std::bind(&SeagullEnemy::handleAttackInput, this));
	return behavior;
}

void SeagullEnemy::handleAttackInput() {
	if (m_enemyState != EnemyState::Chasing) return;
	if (getCurrentTarget() == nullptr) return;
	if (m_enemyAttackingBehavior->distToTarget() < 50.f) {
		m_spellManager->executeCurrentSpell(getCurrentTarget()->getCenter());
		m_chasingTime = sf::Time::Zero;
		if (isAlly()) {
			m_waitingTime = sf::seconds(1);
		} 
		else {
			m_waitingTime = sf::seconds(static_cast<float>(rand() % 8 + 3));
		}
	}
}

sf::Time SeagullEnemy::getConfiguredWaitingTime() const {
	return sf::seconds(static_cast<float>(rand() % 3 + 1));
}

sf::Time SeagullEnemy::getConfiguredChasingTime() const {
	return sf::seconds(static_cast<float>(rand() % 6 + 2));
}

void SeagullEnemy::loadAnimation() {
	setBoundingBox(sf::FloatRect(0.f, 0.f, 60.f, 54.f));
	setSpriteOffset(sf::Vector2f(-5.f, -5.f));

	Animation* flyingAnimation = new Animation();
	flyingAnimation->setSpriteSheet(g_resourceManager->getTexture(ResourceID::Texture_enemy_seagull));
	flyingAnimation->addFrame(sf::IntRect(0, 0, 70, 64));
	flyingAnimation->addFrame(sf::IntRect(70, 0, 70, 64));
	flyingAnimation->addFrame(sf::IntRect(140, 0, 70, 64));
	flyingAnimation->addFrame(sf::IntRect(210, 0, 70, 64));

	addAnimation(GameObjectState::Flying, flyingAnimation);

	Animation* idleAnimation = new Animation();
	idleAnimation->setSpriteSheet(g_resourceManager->getTexture(ResourceID::Texture_enemy_seagull));
	idleAnimation->addFrame(sf::IntRect(0, 0, 70, 64));
	idleAnimation->addFrame(sf::IntRect(70, 0, 70, 64));
	idleAnimation->addFrame(sf::IntRect(140, 0, 70, 64));
	idleAnimation->addFrame(sf::IntRect(210, 0, 70, 64));

	addAnimation(GameObjectState::Idle, idleAnimation);

	Animation* fightingAnimation = new Animation();
	fightingAnimation->setSpriteSheet(g_resourceManager->getTexture(ResourceID::Texture_enemy_seagull));
	fightingAnimation->addFrame(sf::IntRect(0, 64, 70, 64));
	fightingAnimation->addFrame(sf::IntRect(70, 64, 70, 64));
	fightingAnimation->addFrame(sf::IntRect(140, 64, 70, 64));

	addAnimation(GameObjectState::Fighting, fightingAnimation);

	Animation* deadAnimation = new Animation();
	deadAnimation->setSpriteSheet(g_resourceManager->getTexture(ResourceID::Texture_enemy_seagull));
	deadAnimation->addFrame(sf::IntRect(0, 128, 70, 64));

	addAnimation(GameObjectState::Dead, deadAnimation);

	// initial values
	setState(GameObjectState::Idle);
	playCurrentAnimation(true);
}
