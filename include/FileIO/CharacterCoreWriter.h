#pragma once

#include "global.h"

#include "Structs/CharacterCoreData.h"
#include "FileIO/CharacterCoreIO.h"

// a writer to write a savefile.
class CharacterCoreWriter final : public CharacterCoreIO {
public:
	bool saveToFile(const std::string& filename, const CharacterCoreData& data) const;

	// tries to create file with that filename and returns true if successful.
	// if file already exists, it returns false.
	bool createFile(const std::string& filename) const;

private:
	std::string writeAndHash(const std::string& in, const CharacterCoreData& data) const;

private:

	std::string writeTimePlayed(const CharacterCoreData& data) const;
	std::string writeSaveGameName(const CharacterCoreData& data) const;
	std::string writeDateSaved(const CharacterCoreData& data) const;

	std::string writeIsInLevel(const CharacterCoreData& data) const;
	std::string writeMapID(const CharacterCoreData& data) const;
	std::string writeMapPosition(const CharacterCoreData& data) const;
	std::string writeLevelID(const CharacterCoreData& data) const;
	std::string writeLevelPosition(const CharacterCoreData& data) const;
	std::string writeForcedMapID(const CharacterCoreData& data) const;
	std::string writeForcedMapPosition(const CharacterCoreData& data) const;
	std::string writeEnemiesKilled(const CharacterCoreData& data) const;
	std::string writeEnemiesLooted(const CharacterCoreData& data) const;
	std::string writeItemsLooted(const CharacterCoreData& data) const;
	std::string writeChestsLooted(const CharacterCoreData& data) const;
	std::string writeWaypointsUnlocked(const CharacterCoreData& data) const;
	std::string writeModifiersUnlocked(const CharacterCoreData& data) const;
	std::string writeTriggersTriggered(const CharacterCoreData& data) const;
	std::string writeDoorsOpen(const CharacterCoreData& data) const;
	std::string writeTilesExplored(const CharacterCoreData& data) const;
	std::string writeQuestStates(const CharacterCoreData& data) const;
	std::string writeMerchantStates(const CharacterCoreData& data) const;
	std::string writeQuestProgressTargets(const CharacterCoreData& data) const;
	std::string writeQuestProgressConditions(const CharacterCoreData& data) const;
	std::string writeQuestProgressDescription(const CharacterCoreData& data) const;
	std::string writeProgressConditions(const CharacterCoreData& data) const;
	std::string writeSpellsLearned(const CharacterCoreData& data) const;
	std::string writeModifiersLearned(const CharacterCoreData& data) const;
	std::string writeHintsLearned(const CharacterCoreData& data) const;
	std::string writeGuild(const CharacterCoreData& data) const;
	std::string writeReputationProgress(const CharacterCoreData& data) const;
	std::string writeWeather(const CharacterCoreData& data) const;

	std::string writeAttributes(const CharacterCoreData& data) const;

	std::string writeGold(const CharacterCoreData& data) const;
	std::string writeDeaths(const CharacterCoreData& data) const;
	std::string writeStoredGold(const CharacterCoreData& data) const;
	std::string writeItemID(const CharacterCoreData& data) const;
	std::string writeStoredItemID(const CharacterCoreData& data) const;
	std::string writeEquippedWeaponSlots(const CharacterCoreData& data) const;
	std::string writeEquippedItems(const CharacterCoreData& data) const;
	std::string writeWeaponConfigurations(const CharacterCoreData& data) const;
	std::string writeWeaponSpell(const CharacterCoreData& data) const;
	std::string writeQuickslots(const CharacterCoreData& data) const;
};