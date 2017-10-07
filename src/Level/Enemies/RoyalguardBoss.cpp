#include "Level/Enemies/RoyalguardBoss.h"
#include "Level/Level.h"
#include "Level/LevelMainCharacter.h"

const sf::Vector2f RoyalguardBoss::WEAPON_ORIGIN = sf::Vector2f(56.f, 15.f);
const sf::Time RoyalguardBoss::HEALING_TIME = sf::seconds(5.f);

RoyalguardBoss::RoyalguardBoss(const Level* level, Screen* screen) :
	LevelMovableGameObject(level),
	Enemy(level, screen) {

	m_isAlwaysUpdate = true;
	m_isBoss = true;
	m_stateTime = sf::seconds(1.f);
}

void RoyalguardBoss::notifyOtherDeath(const sf::Vector2f& newPos) {
	if (m_isDead) return;
	setPosition(newPos);
	setStunned(HEALING_TIME);

	m_weaponOffset = sf::Vector2f(79.f, 65.f);
	m_isWeaponVisible = true;
	m_stateTime = HEALING_TIME;
	m_weaponRotateType = WeaponRotateType::Fixed;
	m_weapon.setRotation(270.f); // up
}

void RoyalguardBoss::revive() {
	if (!m_isDead) return;

	m_isDead = false;
	m_attributes.currentHealthPoints = m_attributes.maxHealthPoints;
}

void RoyalguardBoss::setDead() {
	if (m_isDead) return;
	Enemy::setDead();
	clearReward();

	WorldCollisionQueryRecord rec;
	rec.boundingBox = *getBoundingBox();

	for (auto go : *m_screen->getObjects(GameObjectType::_Enemy)) {
		if (auto other = dynamic_cast<RoyalguardBoss*>(go)) {
			if (other == this) continue;
			m_other = other;
			rec.boundingBox.left += 20.f;
			other->notifyOtherDeath(getPosition() + (m_level->collides(rec) ? sf::Vector2f() : sf::Vector2f(20.f, 0.f)));
			rec.boundingBox.left -= 20.f;
			break;
		}
	}
}

void RoyalguardBoss::update(const sf::Time& frameTime) {
	Enemy::update(frameTime);
	if (m_isWeaponVisible) {
		updateWeapon(frameTime);
	}
	updateBossState(frameTime);
}

void RoyalguardBoss::render(sf::RenderTarget& target) {
	Enemy::render(target);
	if (m_isWeaponVisible) {
		target.draw(m_weapon);
	}
}

void RoyalguardBoss::loadWeapon() {
	g_resourceManager->loadTexture(getWeaponTexturePath(), ResourceType::Level);
	auto tex = g_resourceManager->getTexture(getWeaponTexturePath());
	if (!tex) return;
	
	m_weapon.setTexture(*tex);
	m_weapon.setOrigin(WEAPON_ORIGIN);
}

void RoyalguardBoss::updateWeapon(const sf::Time& frameTime) {
	sf::Vector2f offset = isFacingRight() ? m_weaponOffset : sf::Vector2f(m_boundingBox.width - m_weaponOffset.x, m_weaponOffset.y);
	m_weapon.setPosition(getPosition() + offset);
	switch (m_weaponRotateType)
	{
	case WeaponRotateType::ToMainChar: 
	{
		sf::Vector2f toMainChar = m_mainChar->getCenter() - m_weapon.getPosition();
		m_weapon.setRotation(radToDeg(std::atan2(toMainChar.y, toMainChar.x)));
		break;
	}
	case WeaponRotateType::Turn:
		m_weapon.setRotation(m_weapon.getRotation() + frameTime.asSeconds() * 360.f);
		break;
	case WeaponRotateType::Fixed:
	default:
		break;
	}
}

void RoyalguardBoss::clearBossState() {
	m_stateTime = sf::Time::Zero;
}

sf::Time RoyalguardBoss::getConfiguredWaitingTime() const {
	return sf::Time::Zero;
}

float RoyalguardBoss::getConfiguredDistanceToHPBar() const {
	return 30.f;
}
