#ifndef DUNGEON_H
#define DUNGEON_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QMap>
#include "character.h"
#include "item.h"
#include "equipment.h"

enum class RoomType {
    ENTRANCE,
    BATTLE,
    ELITE,
    TREASURE,
    REST,
    BOSS
};

struct Room {
    RoomType type;
    bool cleared;
    Character* monster;
    QVector<Item*> items;
    QVector<Equipment*> equipments;
    int x, y;
    QString description;

    Room() : type(RoomType::BATTLE), cleared(false), monster(nullptr), x(0), y(0) {}
};

class Dungeon
{
public:
    Dungeon(int maxFloors = 5);
    ~Dungeon();

    void generateFloor(int floor);
    Room getRoom(int x, int y);
    void setRoomCleared(int x, int y, bool cleared = true);

    int currentFloor() const { return m_currentFloor; }
    int maxFloors() const { return m_maxFloors; }
    bool hasNextFloor() const { return m_currentFloor < m_maxFloors; }
    void nextFloor();

    Room currentRoom() { return getRoom(m_playerX, m_playerY); }

    void setPlayerPosition(int x, int y) { m_playerX = x; m_playerY = y; }
    int playerX() const { return m_playerX; }
    int playerY() const { return m_playerY; }

    QVector<QVector<Room>>& getFloor() { return m_currentFloorRooms; }
    Character* generateMonster(int floor);
    Character* generateElite(int floor);
    Character* generateBoss(int floor);

    Item* generateRandomItem(int floor);
    Equipment* generateRandomEquipment(int floor);

private:
    int m_maxFloors;
    int m_currentFloor;
    int m_playerX, m_playerY;
    QVector<QVector<Room>> m_currentFloorRooms;

    void initializeRooms();
    void placeRooms();
    void placeBoss();
    QString getRoomDescription(RoomType type);
};

#endif // DUNGEON_H
