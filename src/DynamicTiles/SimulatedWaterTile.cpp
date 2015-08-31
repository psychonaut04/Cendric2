#include "DynamicTiles/SimulatedWaterTile.h"
#include "Spell.h"

#include "DynamicTiles/FrozenWaterTile.h"
#include "CustomParticleUpdaters.h"

using namespace std;

const float SimulatedWaterTile::TENSION = 0.4f;
const float SimulatedWaterTile::DAMPING = 0.05f;
const float SimulatedWaterTile::SPREAD = 0.7f;

const float SimulatedWaterTile::WATER_LEVEL = 40.f;
const float SimulatedWaterTile::WATER_SURFACE_THICKNESS = 4.f;
const int SimulatedWaterTile:: NUMBER_COLUMNS_PER_SUBTILE = 10;

const sf::Color SimulatedWaterTile::WATER_COLOR = sf::Color(20, 50, 100, 128);

void SimulatedWaterTile::init()
{
	setSpriteOffset(sf::Vector2f(0.f, 0.f));
}

void SimulatedWaterTile::load(int skinNr)
{
	m_isCollidable = false;

	const sf::FloatRect *bb = getBoundingBox();
	m_x = getPosition().x;
	m_y = getPosition().y;
	m_width = bb->width;
	m_height = bb->height;

	setBoundingBox(sf::FloatRect(m_x, m_y + 11.f, m_width, m_height));

	m_nTiles = static_cast<int>(bb->width / m_tileSize.x);

	m_nColumns = NUMBER_COLUMNS_PER_SUBTILE * m_nTiles;
	m_columns = vector<WaterColumn>();
	for (int i = 0; i < m_nColumns; ++i)
	{
		WaterColumn c;
		c.targetHeight = WATER_LEVEL;
		c.height = WATER_LEVEL;
		c.velocity = 0.f;
		c.fixed = false;

		m_columns.push_back(c);
	}

	m_leftDeltas = new float[m_nColumns];
	m_rightDeltas = new float[m_nColumns];

	m_frozenTiles = std::vector<FrozenWaterTile *>(m_nTiles, nullptr);

	m_vertexArray = sf::VertexArray(sf::Quads, 2 * 4 * (m_nColumns - 1));

	// Particle System
	m_ps = std::unique_ptr<particles::MetaballParticleSystem>(new particles::MetaballParticleSystem(100, g_resourceManager->getTexture(ResourceID::Texture_Particle_blob), WINDOW_WIDTH, WINDOW_HEIGHT));
	g_resourceManager->getTexture(ResourceID::Texture_Particle_blob)->setSmooth(true);
	m_ps->color = WATER_COLOR;
	m_ps->threshold = 0.7f;

	// Generators
	auto posGen = m_ps->addGenerator<particles::DiskPositionGenerator>();
	m_particlePosition = &posGen->center;
	posGen->radius = 40.f;

	auto sizeGen = m_ps->addGenerator<particles::SizeGenerator>();
	sizeGen->minStartSize = 8.f;
	sizeGen->maxStartSize = 16.0f;
	sizeGen->minEndSize = 4.0f;
	sizeGen->maxEndSize = 8.0f;

	auto velGen = m_ps->addGenerator<particles::AngledVelocityGenerator>();
	velGen->minAngle = -60.f;
	velGen->maxAngle = 60.f;
	m_particleMinSpeed = &velGen->minStartVel;
	m_particleMaxSpeed = &velGen->maxStartVel;

	auto timeGen = m_ps->addGenerator<particles::TimeGenerator>();
	timeGen->minTime = 10.0f;
	timeGen->maxTime = 10.0f;

	// Updaters
	auto timeUpdater = m_ps->addUpdater<particles::TimeUpdater>();

	auto waterUpdater = m_ps->addUpdater<particles::SimulatedWaterUpdater>();
	waterUpdater->water = this;

	auto eulerUpdater = m_ps->addUpdater<particles::EulerUpdater>();
    eulerUpdater->globalAcceleration = sf::Vector2f(0.0f, 500.0f);
}

void SimulatedWaterTile::update(const sf::Time& frameTime)
{
	float dt = frameTime.asSeconds();
	dt *= 20.f;

	for (int i = 0; i < m_nColumns; ++i)
	{
		m_columns[i].update(DAMPING, TENSION, dt);
	}

	for (int iterations = 0; iterations < 8; ++iterations)
	{
		for (int i = 0; i < m_nColumns; ++i)
		{
			if (i > 0)
			{
				m_leftDeltas[i] = SPREAD * (m_columns[i].height - m_columns[i - 1].height);
				m_columns[i - 1].velocity += m_leftDeltas[i] * dt;
			}
			if (i < m_nColumns - 1)
			{
				m_rightDeltas[i] = SPREAD * (m_columns[i].height - m_columns[i + 1].height);
				m_columns[i + 1].velocity += m_rightDeltas[i] * dt;
			}
		}

		for (int i = 0; i < m_nColumns; ++i)
		{
			if (i > 0)
			{
				m_columns[i - 1].height += m_leftDeltas[i] * dt;
			}
			if (i < m_nColumns - 1)
			{
				m_columns[i + 1].height += m_rightDeltas[i] * dt;
			}
		}
	}

	m_columns[0].height = m_columns[0].targetHeight;
	m_columns[m_nColumns - 1].height = m_columns[m_nColumns - 1].targetHeight;

	for (int i = 0; i < m_nColumns; ++i)
	{
		if (m_columns[i].fixed)
		{
			m_columns[i].height = m_columns[i].targetHeight;
		}
	}

	sf::Color transparent = sf::Color(255, 255, 255, 0);

	float scale = m_width / (float)(m_nColumns - 1);
	float thickness = 4.f;

	for (int i = 0; i < m_nColumns - 1; ++i)
	{
		sf::Vector2f p1 = sf::Vector2f(m_x + i * scale, m_y + m_height - m_columns[i].height);
		sf::Vector2f p2 = sf::Vector2f(m_x + (i+1) * scale, m_y + m_height - m_columns[i+1].height);
		sf::Vector2f p3 = sf::Vector2f(p2.x, m_y + m_height);
		sf::Vector2f p4 = sf::Vector2f(p1.x, m_y + m_height);
		sf::Vector2f p5 = sf::Vector2f(p2.x, p2.y - thickness);
		sf::Vector2f p6 = sf::Vector2f(p1.x, p1.y - thickness);

		m_vertexArray[8 * i + 0].position = p1;
		m_vertexArray[8 * i + 0].color = WATER_COLOR;
		m_vertexArray[8 * i + 1].position = p2;
		m_vertexArray[8 * i + 1].color = WATER_COLOR;
		m_vertexArray[8 * i + 2].position = p3;
		m_vertexArray[8 * i + 2].color = WATER_COLOR;
		m_vertexArray[8 * i + 3].position = p4;
		m_vertexArray[8 * i + 3].color = WATER_COLOR;

		m_vertexArray[8 * i + 4].position = p1;
		m_vertexArray[8 * i + 4].color = WATER_COLOR;
		m_vertexArray[8 * i + 5].position = p2;
		m_vertexArray[8 * i + 5].color = WATER_COLOR;
		m_vertexArray[8 * i + 6].position = p5;
		m_vertexArray[8 * i + 6].color = transparent;
		m_vertexArray[8 * i + 7].position = p6;
		m_vertexArray[8 * i + 7].color = transparent;
	}

	m_ps->update(frameTime);
}

float SimulatedWaterTile::getHeight(float xPosition)
{
	int index = static_cast<int>((xPosition - m_x) / (m_width / (m_nColumns - 1)));
	if (index < 0 || index > m_nColumns - 1)
		return WATER_LEVEL;

	return m_columns[index].height;
}

void SimulatedWaterTile::splash(float xPosition, float velocity)
{
	int index = static_cast<int>((xPosition - m_x) / (m_width / (m_nColumns - 1)));
	if (index > 0 && index < m_nColumns)
	{
		m_columns[index].velocity = velocity;
	}

	velocity = std::abs(velocity);
	float y = getHeight(xPosition);
	const sf::FloatRect *bb = getBoundingBox();

	*m_particlePosition = sf::Vector2f(xPosition, bb->top + bb->height - y - WATER_SURFACE_THICKNESS);
	*m_particleMinSpeed = 0.2f * velocity;
	*m_particleMaxSpeed = 1.0f * velocity;
	int nParticles = static_cast<int>(velocity / 8);
	m_ps->emit(nParticles);
}

void SimulatedWaterTile::render(sf::RenderTarget& target)
{
	target.draw(m_vertexArray);
	m_ps->render(target);
}

void SimulatedWaterTile::onHit(Spell* spell)
{
	auto id = spell->getSpellID();

	int index = static_cast<int>(std::floor((spell->getPosition().x - m_x) / m_tileSize.x));
	bool frozen = isFrozen(index);
	bool doSplash = true;

	if (id == SpellID::IceBall)
	{
		if (!frozen)
		{
			freeze(index);
			spell->setDisposed();
		}
		doSplash = false;
	}
	else if (id == SpellID::FireBall)
	{
		if (frozen) {
			doSplash = false;
		}
	}

	if (doSplash)
	{
		float vx = spell->getVelocity().x;
		float vy = spell->getVelocity().y;
		float vel = std::sqrt(vx*vx + vy*vy);
		splash(spell->getPosition().x, -vel * 0.5f);
		spell->setDisposed();
	}
}

void SimulatedWaterTile::onHit(LevelMovableGameObject* mob)
{
	float vx = mob->getVelocity().x;
	float vy = mob->getVelocity().y;
	float vel = std::sqrt(vx*vx + vy*vy);
	// TODO: find maximum value for velocity, s.t. the waves stay inside the tile
	float x = mob->getBoundingBox()->left + mob->getBoundingBox()->width / 2;
	splash(x, -vel * 0.5f);
}

void SimulatedWaterTile::freeze(int index)
{
	if (index >= 0 && index < m_nTiles)
	{
		for (int i = 0; i < NUMBER_COLUMNS_PER_SUBTILE; ++i)
		{
			WaterColumn &col = m_columns[index * NUMBER_COLUMNS_PER_SUBTILE + i];
			col.height = col.targetHeight;
			col.fixed = true;
		}

		FrozenWaterTile *frozenTile = new FrozenWaterTile(this, index);
		frozenTile->setTileSize(m_tileSize);
		frozenTile->init();
		frozenTile->setPosition(sf::Vector2f(m_x + index * m_tileSize.x, m_y));
		const sf::FloatRect *bb = frozenTile->getBoundingBox();
		frozenTile->setBoundingBox(sf::FloatRect(bb->left, bb->top, bb->width, bb->height - 35.f));	// ice tile is ca. 15 pixels thick
		frozenTile->setDebugBoundingBox(sf::Color::Yellow);
		frozenTile->load(0);
		m_frozenTiles[index] = frozenTile;
		m_screen->addObject(frozenTile);
	}
}

void SimulatedWaterTile::melt(int index)
{
	if (index >= 0 && index < m_nTiles)
	{
		for (int i = 0; i < NUMBER_COLUMNS_PER_SUBTILE; ++i)
		{
			WaterColumn &col = m_columns[index * NUMBER_COLUMNS_PER_SUBTILE + i];
			col.fixed = false;
		}

		m_frozenTiles[index] = nullptr;	// LET IT GOOOO, LET IT GO. Can't keep this pointer anymoooreee.
	}
}

bool SimulatedWaterTile::isFrozen(int index)
{
	if (m_frozenTiles[index])
		return true;
	return false;
}