#include "Level/Enemies/VeliusBoss.h"
#include "Level/LevelMainCharacter.h"
#include "Level/MOBBehavior/MovingBehaviors/VeliusBossMovingBehavior.h"
#include "Level/MOBBehavior/AttackingBehaviors/AggressiveBehavior.h"
#include "Level/MOBBehavior/ScriptedBehavior/ScriptedBehavior.h"
#include "Registrar.h"
#include "GlobalResource.h"

REGISTER_ENEMY(EnemyID::Boss_Velius, VeliusBoss)

VeliusBoss::VeliusBoss(const Level* level, Screen* screen) :
	LevelMovableGameObject(level),
	Enemy(level, screen),
	Boss(level, screen) {
}

void VeliusBoss::loadAttributes() {
	m_attributes.setHealth(500);
	m_attributes.resistanceIce = -20;
	m_attributes.resistanceLight = -20;
	m_attributes.resistanceFire = 50;
	m_attributes.resistanceShadow = 10000;
	m_attributes.critical = 100;
	m_attributes.calculateAttributes();
}

void VeliusBoss::loadSpells() {
	// this is the stunning projectile
	SpellData projectile = SpellData::getSpellData(SpellID::Projectile);
	projectile.skinNr = 2;
	projectile.damage = 25;
	projectile.damagePerSecond = 6;
	projectile.isStunning = true;
	projectile.duration = sf::seconds(1.f);
	projectile.cooldown = sf::seconds(5.f);
	projectile.isBlocking = true;
	projectile.fightingTime = sf::seconds(1.f);
	projectile.fightAnimation = GameObjectState::Fighting;
	projectile.castingTime = sf::seconds(1.f);
	projectile.castingAnimation = GameObjectState::Casting;
	projectile.speed = 300;

	m_spellManager->addSpell(projectile);

	// these are the shadow projectiles
	projectile = SpellData::getSpellData(SpellID::Projectile);
	projectile.damageType = DamageType::Shadow;
	projectile.skinNr = 1;
	projectile.damage = 15;
	projectile.damagePerSecond = 4;
	projectile.count = 3;
	projectile.cooldown = sf::seconds(5.f);
	projectile.isBlocking = true;
	projectile.fightingTime = sf::seconds(1.f);
	projectile.fightAnimation = GameObjectState::Fighting;
	projectile.castingTime = sf::seconds(1.f);
	projectile.castingAnimation = GameObjectState::Casting2;
	projectile.speed = 400;

	m_spellManager->addSpell(projectile);

	m_spellManager->setCurrentSpell(0); // stun
	m_spellManager->setGlobalCooldown(sf::seconds(3.f));
}

void VeliusBoss::handleAttackInput() {
	m_spellManager->setCurrentSpell(rand() % 2);

	if (getCurrentTarget() != nullptr)
		m_spellManager->executeCurrentSpell(getCurrentTarget());
}

void VeliusBoss::loadAnimation(int skinNr) {
	int size = 120;

	setBoundingBox(sf::FloatRect(0.f, 0.f, 30.f, 90.f));
	setSpriteOffset(sf::Vector2f(-45.f, -30.f));
	const sf::Texture* tex = g_resourceManager->getTexture(getSpritePath());

	Animation* walkingAnimation = new Animation(sf::seconds(0.1f));
	walkingAnimation->setSpriteSheet(tex);
	for (int i = 0; i < 8; i++) {
		walkingAnimation->addFrame(sf::IntRect(i * size, 0, size, size));
	}

	addAnimation(GameObjectState::Walking, walkingAnimation);

	Animation* idleAnimation = new Animation();
	idleAnimation->setSpriteSheet(tex);
	idleAnimation->addFrame(sf::IntRect(8 * size, 0, size, size));

	addAnimation(GameObjectState::Idle, idleAnimation);

	Animation* jumpingAnimation = new Animation();
	jumpingAnimation->setSpriteSheet(tex);
	jumpingAnimation->addFrame(sf::IntRect(9 * size, 0, size, size));

	addAnimation(GameObjectState::Jumping, jumpingAnimation);

	Animation* deadAnimation = new Animation();
	deadAnimation->setSpriteSheet(tex);
	deadAnimation->addFrame(sf::IntRect(17 * size, 0, size, size));
	deadAnimation->setLooped(false);

	addAnimation(GameObjectState::Dead, deadAnimation);

	Animation* fightingAnimation = new Animation(sf::seconds(0.1f));
	fightingAnimation->setSpriteSheet(tex);
	fightingAnimation->addFrame(sf::IntRect(16 * size, 0, size, size));
	fightingAnimation->setLooped(false);

	addAnimation(GameObjectState::Fighting, fightingAnimation);

	Animation* castingAnimation = new Animation(sf::seconds(0.2f));
	castingAnimation->setSpriteSheet(tex);
	for (int i = 13; i < 16; ++i) {
		castingAnimation->addFrame(sf::IntRect(i * size, 0, size, size));
	}
	castingAnimation->setLooped(false);

	addAnimation(GameObjectState::Casting, castingAnimation);

	Animation* casting2Animation = new Animation(sf::seconds(0.2f));
	casting2Animation->setSpriteSheet(tex);
	for (int i = 10; i < 13; ++i) {
		casting2Animation->addFrame(sf::IntRect(i * size, 0, size, size));
	}
	casting2Animation->setLooped(false);

	addAnimation(GameObjectState::Casting2, casting2Animation);

	// initial values
	setState(GameObjectState::Idle);
	playCurrentAnimation(true);
}

MovingBehavior* VeliusBoss::createMovingBehavior(bool asAlly) {
	WalkingBehavior* behavior;
	behavior = new VeliusBossMovingBehavior(this);
	behavior->setDistanceToAbyss(100.f);
	behavior->setApproachingDistance(600.f);
	behavior->setMaxVelocityYDown(800.f);
	behavior->setMaxVelocityYUp(600.f);
	behavior->setMaxVelocityX(200.f);
	behavior->setDropAlways(true);
	behavior->calculateJumpHeight();
	return behavior;
}

AttackingBehavior* VeliusBoss::createAttackingBehavior(bool asAlly) {
	EnemyAttackingBehavior* behavior;
	behavior = new AggressiveBehavior(this);
	behavior->setAggroRange(10000.f);
	behavior->setAttackInput(std::bind(&VeliusBoss::handleAttackInput, this));
	return behavior;
}

float VeliusBoss::getConfiguredDistanceToHPBar() const {
	return 30.f;
}

std::string VeliusBoss::getSpritePath() const {
	return "res/texture/bosses/spritesheet_boss_velius.png";
}