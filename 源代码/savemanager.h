#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QVector>
#include "character.h"
#include "item.h"
#include "equipment.h"
#include "skill.h"

struct SaveInfo {
    QString name;
    int floor;
    QString playerName;
    int index;
};

class SaveManager
{
public:
    SaveManager();
    ~SaveManager();

    static const int MAX_SAVES = 5;

    bool saveGame(int slot, const QString& saveName, Character* player, int currentFloor,
                  const QVector<Item*>& inventory,
                  const QVector<Equipment*>& equipments, const QVector<Skill*>& skills,
                  int playerX = 0, int playerY = 0);
    bool loadGame(int slot, Character*& player, int& currentFloor, 
                  QString& saveName, QVector<Item*>& inventory,
                  QVector<Equipment*>& equipments, QVector<Skill*>& skills,
                  int& playerX, int& playerY);

    QVector<SaveInfo> getAllSaves();
    bool hasSave(int slot) const;
    bool deleteSave(int slot);
    bool isSaveNameExists(const QString& name);
    int getFreeSlot();
    int getTotalSaveCount();
    bool hasSaveFile() const;

    static QJsonObject characterToJson(Character* character);
    static Character* jsonToCharacter(const QJsonObject& json);

    static QJsonObject itemToJson(Item* item);
    static Item* jsonToItem(const QJsonObject& json);

    static QJsonObject equipmentToJson(Equipment* equipment);
    static Equipment* jsonToEquipment(const QJsonObject& json);

    static QJsonObject skillToJson(Skill* skill);
    static Skill* jsonToSkill(const QJsonObject& json);

private:
    QString getSaveFilePath(int slot) const;
    QString getSaveInfoFilePath() const;
    void saveSaveInfo(int slot, const QString& saveName, int currentFloor, const QString& playerName);
};

#endif // SAVEMANAGER_H
