#include "Level/DynamicTiles/TrampolineTile.h"
#include "Spells/Spell.h"
#include "Spells/WindGustSpell.h"
#include "Level/LevelMainCharacter.h"
#include "Registrar.h"

REGISTER_LEVEL_DYNAMIC_TILE(LevelDynamicTileID::Trampoline, TrampolineTile)

TrampolineTile::TrampolineTile(LevelScreen* levelScreen) :
	LevelDynamicTile(levelScreen),
	LevelMovableTile(levelScreen) {
}

bool TrampolineTile::init(const LevelTileProperties& properties) {
	m_jumpingRegion.width = 40.f;
	m_jumpingRegion.height = 2.f;
	setPositionOffset(sf::Vector2f(5.f, 15.f));
	setSpriteOffset(sf::Vector2f(-5.f, -15.f));
	setBoundingBox(sf::FloatRect(0.f, 0.f, m_jumpingRegion.width, 35.f));
	return true;
}

void TrampolineTile::loadAnimation(int skinNr) {
	m_isShiftable = skinNr == 0;

	m_isCollidable = true;
	std::string destructibleTileTex = "res/texture/level_dynamic_tiles/spritesheet_tiles_destructible.png";
	g_resourceManager->loadTexture(destructibleTileTex, ResourceType::Level);
	const sf::Texture* tex = g_resourceManager->getTexture(getSpritePath());

	Animation* idleAnimation = new Animation(sf::seconds(10.f));
	idleAnimation->setSpriteSheet(tex);
	idleAnimation->addFrame(sf::IntRect(0, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	addAnimation(GameObjectState::Idle, idleAnimation);

	Animation* jumpingAnimation = new Animation();
	jumpingAnimation->setSpriteSheet(tex);
	jumpingAnimation->addFrame(sf::IntRect(TILE_SIZE * 1, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	jumpingAnimation->addFrame(sf::IntRect(TILE_SIZE * 2, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	jumpingAnimation->addFrame(sf::IntRect(TILE_SIZE * 3, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	jumpingAnimation->addFrame(sf::IntRect(TILE_SIZE * 2, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	jumpingAnimation->addFrame(sf::IntRect(TILE_SIZE * 1, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	jumpingAnimation->addFrame(sf::IntRect(TILE_SIZE * 0, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	jumpingAnimation->addFrame(sf::IntRect(TILE_SIZE * 4, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	m_jumpingTime = jumpingAnimation->getAnimationTime();
	addAnimation(GameObjectState::Jumping, jumpingAnimation);

	Animation* crumblingAnimation = new Animation();
	crumblingAnimation->setSpriteSheet(g_resourceManager->getTexture(destructibleTileTex));
	for (int i = 1; i < 5; i++) {
		crumblingAnimation->addFrame(sf::IntRect(
			BORDER + i * (2 * BORDER + TILE_SIZE),
			BORDER,
			TILE_SIZE,
			TILE_SIZE));
	}
	crumblingAnimation->setLooped(false);
	m_crumblingTime = crumblingAnimation->getAnimationTime();

	addAnimation(GameObjectState::Crumbling, crumblingAnimation);

	// initial values
	m_state = GameObjectState::Idle;
	setCurrentAnimation(getAnimation(GameObjectState::Idle), false);
	playCurrentAnimation(true);
}

void TrampolineTile::setPosition(const sf::Vector2f& pos) {
	LevelMovableTile::setPosition(pos);
	m_jumpingRegion.left = pos.x;
	m_jumpingRegion.top = pos.y - m_jumpingRegion.height;
}

void TrampolineTile::update(const sf::Time& frameTime) {
	if (m_state == GameObjectState::Crumbling) {
		updateTime(m_crumblingTime, frameTime);
		if (m_crumblingTime == sf::Time::Zero) {
			setDisposed();
		}
		MovableGameObject::update(frameTime);
		return;
	}
	else if (m_state == GameObjectState::Jumping) {
		updateTime(m_jumpingTime, frameTime);
		if (m_jumpingTime == sf::Time::Zero) {
			setState(GameObjectState::Idle);
		}
	}

	setAcceleration(sf::Vector2f(m_pushAcceleration, GRAVITY_ACCELERATION));
	sf::Vector2f nextPosition;
	calculateNextPosition(frameTime, nextPosition);
	checkCollisions(nextPosition);
	MovableGameObject::update(frameTime);
	m_pushAcceleration = 0.f;

	// check for main character
	auto const& mbb = *m_mainChar->getBoundingBox();
	if (m_state == GameObjectState::Idle &&  !m_mainChar->isDead() && !m_mainChar->isIgnoreDynamicTiles() && fastIntersect(mbb, m_jumpingRegion)) {
		auto nextBB = mbb;
		nextBB.top = m_jumpingRegion.top - nextBB.height - 10.f;
		WorldCollisionQueryRecord rec;
		rec.boundingBox = nextBB;
		rec.ignoreDynamicTiles = m_mainChar->isIgnoreDynamicTiles();
		// only set the main char up if it wouldn't collide
		if (!getLevel()->collides(rec)) {
			g_resourceManager->playSound(getSoundPath());
			m_mainChar->setVelocityY(-800.f);
			m_mainChar->setAccelerationY(0.f);
			m_mainChar->setPositionY(nextBB.top);
			m_mainChar->setJumpLock();
			setState(GameObjectState::Jumping);
			m_jumpingTime = getAnimation(GameObjectState::Jumping)->getAnimationTime();
		}
	}

	if (m_boundingBox.top + m_boundingBox.height > (m_level->getWorldRect().top + m_level->getWorldRect().height)) {
		setDisposed();
	}
}

void TrampolineTile::onHit(Spell* spell) {
	if (!m_isShiftable) return;
	switch (spell->getSpellID()) {
	case SpellID::WindGust: {
		m_pushAcceleration = dynamic_cast<WindGustSpell*>(spell)->getPushAcceleration();
		break;
	}
	default:
		break;
	}
}

void TrampolineTile::calculateUnboundedVelocity(const sf::Time& frameTime, sf::Vector2f& nextVel) const {
	// distinguish damping in the air and at the ground
	float dampingPerSec = (getVelocity().y == 0.f) ? DAMPING_GROUND : DAMPING_AIR;
	// don't damp when there is active acceleration 
	if (getAcceleration().x != 0.f) dampingPerSec = 0.f;
	nextVel.x = (getVelocity().x + getAcceleration().x * frameTime.asSeconds()) * pow(1 - dampingPerSec, frameTime.asSeconds());
	nextVel.y = getVelocity().y + getAcceleration().y * frameTime.asSeconds();
}

std::string TrampolineTile::getSpritePath() const {
	return "res/texture/level_dynamic_tiles/spritesheet_tiles_trampoline.png";
}

std::string TrampolineTile::getSoundPath() const {
	return "res/sound/tile/trampoline.ogg";
}


