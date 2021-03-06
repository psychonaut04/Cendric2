#include "SpellCreators/HolyFireSpellCreator.h"
#include "Screens/LevelScreen.h"

HolyFireSpellCreator::HolyFireSpellCreator(const SpellData& spellData, LevelMovableGameObject *owner) : SpellCreator(spellData, owner) {
}

void HolyFireSpellCreator::execExecuteSpell(const sf::Vector2f& target) {
	SpellData spellData = m_spellData;
	updateDamageAndHeal(spellData);
	HolyFireSpell* newSpell = new HolyFireSpell();
	newSpell->load(spellData, m_owner, target);
	m_screen->addObject(newSpell);
}

void HolyFireSpellCreator::addDurationModifier(int level) {
	m_spellData.duration += static_cast<float>(level) * m_spellData.durationModifierAddition;
}

void HolyFireSpellCreator::addRangeModifier(int level) {
	m_spellData.range += m_spellData.rangeModifierAddition * level;
	m_spellData.boundingBox.height = 2 * m_spellData.range;
	m_spellData.boundingBox.width = 2 * m_spellData.range;
}

