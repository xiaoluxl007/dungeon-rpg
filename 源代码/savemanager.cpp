#include "savemanager.h"
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>

SaveManager::SaveManager()
{
}

SaveManager::~SaveManager()
{
}

QString SaveManager::getSaveFilePath(int slot) const
{
    return QCoreApplication::applicationDirPath() + QString("/save_%1.json").arg(slot);
}

QString SaveManager::getSaveInfoFilePath() const
{
    return QCoreApplication::applicationDirPath() + "/save_info.json";
}

bool SaveManager::saveGame(int slot, const QString& saveName, Character* player, int currentFloor,
                          const QVector<Item*>& inventory,
                          const QVector<Equipment*>& equipments, const QVector<Skill*>& skills,
                          int playerX, int playerY)
{
    QJsonObject root;

    root["version"] = "1.2";
    root["saveName"] = saveName;
    root["currentFloor"] = currentFloor;
    root["playerX"] = playerX;
    root["playerY"] = playerY;
    root["player"] = characterToJson(player);

    QJsonArray inventoryArray;
    for (Item* item : inventory) {
        inventoryArray.append(itemToJson(item));
    }
    root["inventory"] = inventoryArray;

    QJsonArray equipmentArray;
    for (Equipment* eq : equipments) {
        equipmentArray.append(equipmentToJson(eq));
    }
    root["equipments"] = equipmentArray;

    QJsonArray skillArray;
    for (Skill* skill : skills) {
        skillArray.append(skillToJson(skill));
    }
    root["skills"] = skillArray;

    QJsonDocument doc(root);
    QFile file(getSaveFilePath(slot));

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << file.errorString();
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    saveSaveInfo(slot, saveName, currentFloor, player->name());

    return true;
}

void SaveManager::saveSaveInfo(int slot, const QString& saveName, int currentFloor, const QString& playerName)
{
    QVector<SaveInfo> allSaves = getAllSaves();

    bool found = false;
    for (SaveInfo& info : allSaves) {
        if (info.index == slot) {
            info.name = saveName;
            info.floor = currentFloor;
            info.playerName = playerName;
            found = true;
            break;
        }
    }

    if (!found) {
        SaveInfo info;
        info.name = saveName;
        info.floor = currentFloor;
        info.playerName = playerName;
        info.index = slot;
        allSaves.append(info);
    }

    QJsonArray array;
    for (const SaveInfo& info : allSaves) {
        QJsonObject obj;
        obj["name"] = info.name;
        obj["floor"] = info.floor;
        obj["playerName"] = info.playerName;
        obj["index"] = info.index;
        array.append(obj);
    }

    QJsonObject root;
    root["saves"] = array;

    QJsonDocument doc(root);
    QFile file(getSaveInfoFilePath());

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

bool SaveManager::loadGame(int slot, Character*& player, int& currentFloor,
                          QString& saveName, QVector<Item*>& inventory,
                          QVector<Equipment*>& equipments, QVector<Skill*>& skills,
                          int& playerX, int& playerY)
{
    QFile file(getSaveFilePath(slot));

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON document";
        return false;
    }

    QJsonObject root = doc.object();

    saveName = root["saveName"].toString();
    currentFloor = root["currentFloor"].toInt(1);
    playerX = root["playerX"].toInt(0);
    playerY = root["playerY"].toInt(0);

    if (root.contains("player") && root["player"].isObject()) {
        player = jsonToCharacter(root["player"].toObject());
    }

    if (!player) {
        qWarning() << "No valid player data in save file";
        return false;
    }

    if (root.contains("inventory")) {
        QJsonArray inventoryArray = root["inventory"].toArray();
        for (const QJsonValue& value : inventoryArray) {
            if (!value.isObject()) continue;
            Item* item = jsonToItem(value.toObject());
            if (item) {
                inventory.append(item);
            }
        }
    }

    if (root.contains("equipments")) {
        QJsonArray equipmentArray = root["equipments"].toArray();
        for (const QJsonValue& value : equipmentArray) {
            if (!value.isObject()) continue;
            Equipment* eq = jsonToEquipment(value.toObject());
            if (eq) {
                equipments.append(eq);
            }
        }
    }

    if (root.contains("skills")) {
        QJsonArray skillArray = root["skills"].toArray();
        for (const QJsonValue& value : skillArray) {
            if (!value.isObject()) continue;
            Skill* skill = jsonToSkill(value.toObject());
            if (skill) {
                skills.append(skill);
            }
        }
    }

    return true;
}

QVector<SaveInfo> SaveManager::getAllSaves()
{
    QVector<SaveInfo> saves;

    QFile file(getSaveInfoFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return saves;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return saves;
    }

    QJsonObject root = doc.object();
    if (!root.contains("saves")) {
        return saves;
    }

    QJsonArray array = root["saves"].toArray();
    for (const QJsonValue& value : array) {
        QJsonObject obj = value.toObject();
        SaveInfo info;
        info.name = obj["name"].toString();
        info.floor = obj["floor"].toInt();
        info.playerName = obj["playerName"].toString();
        info.index = obj["index"].toInt();
        saves.append(info);
    }

    return saves;
}

bool SaveManager::hasSave(int slot) const
{
    return QFile::exists(getSaveFilePath(slot));
}

bool SaveManager::deleteSave(int slot)
{
    bool success = QFile::remove(getSaveFilePath(slot));

    QVector<SaveInfo> allSaves = getAllSaves();
    for (auto it = allSaves.begin(); it != allSaves.end();) {
        if (it->index == slot) {
            it = allSaves.erase(it);
        } else {
            ++it;
        }
    }

    QJsonArray array;
    for (const SaveInfo& info : allSaves) {
        QJsonObject obj;
        obj["name"] = info.name;
        obj["floor"] = info.floor;
        obj["playerName"] = info.playerName;
        obj["index"] = info.index;
        array.append(obj);
    }

    QJsonObject root;
    root["saves"] = array;

    QJsonDocument doc(root);
    QFile file(getSaveInfoFilePath());

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }

    return success;
}

bool SaveManager::isSaveNameExists(const QString& name)
{
    QVector<SaveInfo> saves = getAllSaves();
    for (const SaveInfo& info : saves) {
        if (info.name == name) {
            return true;
        }
    }
    return false;
}

int SaveManager::getFreeSlot()
{
    for (int i = 1; i <= MAX_SAVES; ++i) {
        if (!hasSave(i)) {
            return i;
        }
    }
    return -1;
}

int SaveManager::getTotalSaveCount()
{
    int count = 0;
    for (int i = 1; i <= MAX_SAVES; ++i) {
        if (hasSave(i)) {
            count++;
        }
    }
    return count;
}

bool SaveManager::hasSaveFile() const
{
    for (int i = 1; i <= MAX_SAVES; ++i) {
        if (hasSave(i)) {
            return true;
        }
    }
    return false;
}

QJsonObject SaveManager::characterToJson(Character* character)
{
    QJsonObject json;
    json["name"] = character->name();
    json["type"] = static_cast<int>(character->type());
    json["hp"] = character->hp();
    json["maxHp"] = character->baseMaxHp();
    json["mana"] = character->mana();
    json["maxMana"] = character->maxMana();
    json["attack"] = character->baseAttack();
    json["defense"] = character->baseDefense();
    json["speed"] = character->speed();
    json["level"] = character->level();
    json["experience"] = character->experience();
    json["gold"] = character->gold();
    json["expReward"] = character->expReward();
    json["goldReward"] = character->goldReward();

    QJsonArray equippedArray;
    for (Equipment* eq : character->getEquipmentList()) {
        equippedArray.append(equipmentToJson(eq));
    }
    json["equipped"] = equippedArray;

    return json;
}

Character* SaveManager::jsonToCharacter(const QJsonObject& json)
{
    QString name = json["name"].toString();
    CharacterType type = static_cast<CharacterType>(json["type"].toInt());
    int hp = json["hp"].toInt();
    int maxHp = json["maxHp"].toInt();
    int attack = json["attack"].toInt();
    int defense = json["defense"].toInt();
    int speed = json["speed"].toInt();
    int level = json["level"].toInt();
    int exp = json["experience"].toInt();
    int gold = json["gold"].toInt();
    int expReward = json["expReward"].toInt();
    int goldReward = json["goldReward"].toInt();

    Character* character = new Character(name, type, hp, attack, defense, speed, level, expReward, goldReward);
    character->setExperience(exp);
    character->setGold(gold);

    character->setMaxHp(maxHp);
    character->setHp(hp);

    if (json.contains("mana") && json.contains("maxMana")) {
        character->setMaxMana(json["maxMana"].toInt());
        character->setMana(json["mana"].toInt());
    } else {
        int defaultMaxMana = 100 + (level - 1) * 20;
        character->setMaxMana(defaultMaxMana);
        character->setMana(defaultMaxMana);
    }

    if (json.contains("equipped")) {
        QJsonArray equippedArray = json["equipped"].toArray();
        for (const QJsonValue& value : equippedArray) {
            if (!value.isObject()) continue;
            Equipment* eq = jsonToEquipment(value.toObject());
            if (eq) {
                character->equipItem(eq);
            }
        }
    }

    return character;
}

QJsonObject SaveManager::itemToJson(Item* item)
{
    QJsonObject json;
    json["name"] = item->name();
    json["type"] = static_cast<int>(item->type());
    json["value"] = item->value();
    json["consumableType"] = static_cast<int>(item->consumableType());
    return json;
}

Item* SaveManager::jsonToItem(const QJsonObject& json)
{
    QString name = json["name"].toString();
    ItemType type = static_cast<ItemType>(json["type"].toInt());
    int value = json["value"].toInt();
    ConsumableType consumableType = static_cast<ConsumableType>(json["consumableType"].toInt());

    return new Item(name, type, value, consumableType);
}

QJsonObject SaveManager::equipmentToJson(Equipment* equipment)
{
    QJsonObject json;
    json["name"] = equipment->name();
    json["type"] = static_cast<int>(equipment->type());
    json["attackBonus"] = equipment->attackBonus();
    json["defenseBonus"] = equipment->defenseBonus();
    json["hpBonus"] = equipment->hpBonus();
    json["quality"] = static_cast<int>(equipment->quality());
    return json;
}

Equipment* SaveManager::jsonToEquipment(const QJsonObject& json)
{
    QString name = json["name"].toString();
    EquipmentType type = static_cast<EquipmentType>(json["type"].toInt());
    int attackBonus = json["attackBonus"].toInt();
    int defenseBonus = json["defenseBonus"].toInt();
    int hpBonus = json["hpBonus"].toInt();
    EquipmentQuality quality = static_cast<EquipmentQuality>(json["quality"].toInt(0));

    return new Equipment(name, type, quality, attackBonus, defenseBonus, hpBonus);
}

QJsonObject SaveManager::skillToJson(Skill* skill)
{
    QJsonObject json;
    json["name"] = skill->name();
    json["type"] = static_cast<int>(skill->type());
    json["target"] = static_cast<int>(skill->target());
    json["damage"] = skill->damage();
    json["manaCost"] = skill->manaCost();
    json["unlockLevel"] = skill->unlockLevel();
    return json;
}

Skill* SaveManager::jsonToSkill(const QJsonObject& json)
{
    QString name = json["name"].toString();
    SkillType type = static_cast<SkillType>(json["type"].toInt());
    SkillTarget target = static_cast<SkillTarget>(json["target"].toInt(0));
    int damage = json["damage"].toInt();
    int manaCost = json["manaCost"].toInt();
    int unlockLevel = json["unlockLevel"].toInt();

    return new Skill(name, type, target, damage, manaCost, unlockLevel);
}
