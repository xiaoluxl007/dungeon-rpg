#include "dungeon.h"
#include <QRandomGenerator>

Dungeon::Dungeon(int maxFloors)
    : m_maxFloors(maxFloors), m_currentFloor(1), m_playerX(0), m_playerY(0)
{
    generateFloor(1);
}

Dungeon::~Dungeon()
{
    for (auto& row : m_currentFloorRooms) {
        for (Room& room : row) {
            if (room.monster) {
                delete room.monster;
            }
            qDeleteAll(room.items);
            qDeleteAll(room.equipments);
        }
    }
}

void Dungeon::generateFloor(int floor)
{
    m_currentFloor = floor;
    m_playerX = 0;
    m_playerY = 0;

    for (auto& row : m_currentFloorRooms) {
        for (Room& room : row) {
            if (room.monster) {
                delete room.monster;
            }
            qDeleteAll(room.items);
            qDeleteAll(room.equipments);
        }
    }

    m_currentFloorRooms.clear();
    initializeRooms();
    placeRooms();
    placeBoss();
}

Room Dungeon::getRoom(int x, int y)
{
    if (x >= 0 && x < 5 && y >= 0 && y < 5) {
        return m_currentFloorRooms[y][x];
    }
    return Room();
}

void Dungeon::setRoomCleared(int x, int y, bool cleared)
{
    if (x >= 0 && x < 5 && y >= 0 && y < 5) {
        m_currentFloorRooms[y][x].cleared = cleared;
        if (m_currentFloorRooms[y][x].monster && cleared) {
            delete m_currentFloorRooms[y][x].monster;
            m_currentFloorRooms[y][x].monster = nullptr;
        }
        if (m_currentFloorRooms[y][x].type == RoomType::TREASURE && cleared) {
            m_currentFloorRooms[y][x].items.clear();
            m_currentFloorRooms[y][x].equipments.clear();
        }
    }
}

void Dungeon::nextFloor()
{
    if (hasNextFloor()) {
        generateFloor(m_currentFloor + 1);
    }
}

void Dungeon::initializeRooms()
{
    m_currentFloorRooms.resize(5);
    for (int y = 0; y < 5; ++y) {
        m_currentFloorRooms[y].resize(5);
        for (int x = 0; x < 5; ++x) {
            m_currentFloorRooms[y][x].x = x;
            m_currentFloorRooms[y][x].y = y;
            m_currentFloorRooms[y][x].cleared = false;
            m_currentFloorRooms[y][x].type = RoomType::BATTLE;
            m_currentFloorRooms[y][x].monster = nullptr;
            m_currentFloorRooms[y][x].items.clear();
            m_currentFloorRooms[y][x].equipments.clear();

            if (x == 0 && y == 0) {
                m_currentFloorRooms[y][x].type = RoomType::ENTRANCE;
                m_currentFloorRooms[y][x].description = "地牢入口，安全区域";
            }
        }
    }
}

void Dungeon::placeRooms()
{
    int numBattles = 8 + QRandomGenerator::global()->bounded(4);
    int numElites = 2 + QRandomGenerator::global()->bounded(2);
    int numTreasures = 5 + QRandomGenerator::global()->bounded(2);

    QVector<QPair<int, int>> available;
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (!(x == 0 && y == 0) && !(x == 4 && y == 4)) {
                available.append(qMakePair(x, y));
            }
        }
    }

    std::random_shuffle(available.begin(), available.end(),
        [](int max) { return QRandomGenerator::global()->bounded(max); });

    int idx = 0;

    for (int i = 0; i < numTreasures && idx < available.size(); ++i, ++idx) {
        int x = available[idx].first;
        int y = available[idx].second;
        m_currentFloorRooms[y][x].type = RoomType::TREASURE;
        m_currentFloorRooms[y][x].description = "宝箱室，发现珍贵物品！";

        Item* item1 = generateRandomItem(m_currentFloor);
        if (item1) {
            m_currentFloorRooms[y][x].items.append(item1);
        }
        if (QRandomGenerator::global()->bounded(100) < 50) {
            Item* item2 = generateRandomItem(m_currentFloor);
            if (item2) {
                m_currentFloorRooms[y][x].items.append(item2);
            }
        }

        if (QRandomGenerator::global()->bounded(100) < 70) {
            Equipment* eq = generateRandomEquipment(m_currentFloor);
            if (eq) {
                m_currentFloorRooms[y][x].equipments.append(eq);
            }
        }
        if (QRandomGenerator::global()->bounded(100) < 30) {
            Equipment* eq2 = generateRandomEquipment(m_currentFloor);
            if (eq2) {
                m_currentFloorRooms[y][x].equipments.append(eq2);
            }
        }
    }

    for (int i = 0; i < numElites && idx < available.size(); ++i, ++idx) {
        int x = available[idx].first;
        int y = available[idx].second;
        m_currentFloorRooms[y][x].type = RoomType::ELITE;
        m_currentFloorRooms[y][x].monster = generateElite(m_currentFloor);
        m_currentFloorRooms[y][x].description = "精英怪物房，危险！";
    }

    for (int i = 0; i < numBattles && idx < available.size(); ++i, ++idx) {
        int x = available[idx].first;
        int y = available[idx].second;
        m_currentFloorRooms[y][x].type = RoomType::BATTLE;
        m_currentFloorRooms[y][x].monster = generateMonster(m_currentFloor);
        m_currentFloorRooms[y][x].description = "遭遇普通怪物";
    }

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (m_currentFloorRooms[y][x].type == RoomType::BATTLE && !m_currentFloorRooms[y][x].monster) {
                m_currentFloorRooms[y][x].monster = generateMonster(m_currentFloor);
                m_currentFloorRooms[y][x].description = "遭遇普通怪物";
            }
        }
    }
}

void Dungeon::placeBoss()
{
    m_currentFloorRooms[4][4].type = RoomType::BOSS;
    m_currentFloorRooms[4][4].monster = generateBoss(m_currentFloor);
    m_currentFloorRooms[4][4].description = "BOSS房！";
}

Character* Dungeon::generateMonster(int floor)
{
    QStringList basicMonsters = {"史莱姆", "哥布林", "蝙蝠怪"};
    QStringList mediumMonsters = {"骷髅兵", "狼人", "食尸鬼", "巨型蜘蛛"};
    QStringList toughMonsters = {"黑暗骑士", "幽灵", "石像鬼", "地狱犬"};

    QString name;
    int level = floor + QRandomGenerator::global()->bounded(2);
    if (level < 1) level = 1;

    QStringList names;
    if (floor <= 2) {
        names = basicMonsters;
    } else if (floor <= 4) {
        names = mediumMonsters;
    } else {
        names = toughMonsters;
    }
    name = names[QRandomGenerator::global()->bounded(names.size())];

    int hp = 30 + level * 15;
    int attack = 5 + level * 3;
    int defense = 3 + level * 2;
    int speed = 4 + level;
    int exp = 25 + level * 20;
    int gold = 5 + level * 5;

    return new Character(name + QString(" (Lv.%1)").arg(level), CharacterType::MONSTER,
                        hp, attack, defense, speed, level, exp, gold);
}

Character* Dungeon::generateElite(int floor)
{
    QStringList eliteNames;
    if (floor <= 2) {
        eliteNames = {"精英哥布林萨满", "骷髅骑士"};
    } else if (floor <= 4) {
        eliteNames = {"暗影刺客", "变异巨兽", "恶魔守卫"};
    } else {
        eliteNames = {"冰霜巨人", "火焰元素", "剧毒蜘蛛女王"};
    }

    QString name = eliteNames[QRandomGenerator::global()->bounded(eliteNames.size())];
    int level = floor + 1;
    if (level < 2) level = 2;

    int hp = 60 + level * 20;
    int attack = 10 + level * 4;
    int defense = 6 + level * 3;
    int speed = 5 + level;
    int exp = 60 + level * 30;
    int gold = 20 + level * 10;

    return new Character(name + QString(" (Lv.%1)").arg(level), CharacterType::ELITE,
                        hp, attack, defense, speed, level, exp, gold);
}

Character* Dungeon::generateBoss(int floor)
{
    QStringList bossNames;

    if (floor == 1) {
        bossNames = {"黑暗哥布林王", "骷髅将军"};
    } else if (floor == 2) {
        bossNames = {"巨型史莱姆王", "狼王"};
    } else if (floor == 3) {
        bossNames = {"暗影领主", "地狱三头犬"};
    } else if (floor == 4) {
        bossNames = {"远古巨龙", "深渊领主"};
    } else {
        bossNames = {"最终BOSS-毁灭者", "混沌之神", "最终黑暗君主"};
    }

    QString name = bossNames[QRandomGenerator::global()->bounded(bossNames.size())];
    int level = floor * 2 + 2;

    int hp = 150 + level * 30;
    int attack = 15 + level * 5;
    int defense = 10 + level * 4;
    int speed = 6 + level;
    int exp = 150 + level * 50;
    int gold = 50 + level * 25;

    return new Character(name + QString(" (Lv.%1)").arg(level), CharacterType::BOSS,
                        hp, attack, defense, speed, level, exp, gold);
}

Item* Dungeon::generateRandomItem(int floor)
{
    QStringList names = {"小型生命药水", "中型生命药水", "大型生命药水"};
    int healAmount = 20 + QRandomGenerator::global()->bounded(30) * floor;
    QString name = names[QRandomGenerator::global()->bounded(names.size())];

    return new Item(name, ItemType::CONSUMABLE, healAmount, ConsumableType::HP_POTION);
}

Equipment* Dungeon::generateRandomEquipment(int floor)
{
    int type = QRandomGenerator::global()->bounded(3);

    // 根据层数确定装备品质概率
    int roll = QRandomGenerator::global()->bounded(100);
    EquipmentQuality quality;

    // 层数越高，获得高品质装备的概率越高
    if (roll < 50 - floor * 5) {
        quality = EquipmentQuality::COMMON;       // 普通
    } else if (roll < 75 - floor * 3) {
        quality = EquipmentQuality::UNCOMMON;     // 优秀
    } else if (roll < 90 - floor) {
        quality = EquipmentQuality::RARE;         // 稀有
    } else if (roll < 97) {
        quality = EquipmentQuality::EPIC;         // 史诗
    } else {
        quality = EquipmentQuality::LEGENDARY;     // 传说
    }

    // 品质对应的属性倍率
    double multiplier = 1.0;
    switch (quality) {
    case EquipmentQuality::COMMON:
        multiplier = 1.0;
        break;
    case EquipmentQuality::UNCOMMON:
        multiplier = 1.3;
        break;
    case EquipmentQuality::RARE:
        multiplier = 1.6;
        break;
    case EquipmentQuality::EPIC:
        multiplier = 2.0;
        break;
    case EquipmentQuality::LEGENDARY:
        multiplier = 2.5;
        break;
    default:
        multiplier = 1.0;
    }

    int baseBonus = floor * 2;

    QString name;
    int attackBonus = 0;
    int defenseBonus = 0;
    int hpBonus = 0;

    if (type == 0) {
        // 武器
        QStringList common = {"铁剑", "短剑", "木棒"};
        QStringList uncommon = {"钢剑", "长剑", "弯刀"};
        QStringList rare = {"魔法剑", "光之剑", "寒冰剑"};
        QStringList epic = {"龙之剑", "烈焰剑", "暗影刃"};
        QStringList legendary = {"神圣裁决", "混沌之刃", "永恒之光"};

        if (quality == EquipmentQuality::COMMON) name = common[QRandomGenerator::global()->bounded(common.size())];
        else if (quality == EquipmentQuality::UNCOMMON) name = uncommon[QRandomGenerator::global()->bounded(uncommon.size())];
        else if (quality == EquipmentQuality::RARE) name = rare[QRandomGenerator::global()->bounded(rare.size())];
        else if (quality == EquipmentQuality::EPIC) name = epic[QRandomGenerator::global()->bounded(epic.size())];
        else name = legendary[QRandomGenerator::global()->bounded(legendary.size())];

        attackBonus = static_cast<int>((5 + baseBonus) * multiplier);
    } else if (type == 1) {
        // 防具
        QStringList common = {"皮甲", "布衣", "麻衣"};
        QStringList uncommon = {"锁甲", "皮甲", "皮靴"};
        QStringList rare = {"板甲", "链甲", "鳞甲"};
        QStringList epic = {"龙鳞甲", "魔法甲", "圣甲"};
        QStringList legendary = {"神圣护甲", "混沌之铠", "永恒守护"};

        if (quality == EquipmentQuality::COMMON) name = common[QRandomGenerator::global()->bounded(common.size())];
        else if (quality == EquipmentQuality::UNCOMMON) name = uncommon[QRandomGenerator::global()->bounded(uncommon.size())];
        else if (quality == EquipmentQuality::RARE) name = rare[QRandomGenerator::global()->bounded(rare.size())];
        else if (quality == EquipmentQuality::EPIC) name = epic[QRandomGenerator::global()->bounded(epic.size())];
        else name = legendary[QRandomGenerator::global()->bounded(legendary.size())];

        defenseBonus = static_cast<int>((5 + baseBonus) * multiplier);
    } else {
        // 饰品
        QStringList common = {"力量戒指", "防御戒指", "生命戒指"};
        QStringList uncommon = {"敏捷戒指", "幸运戒指", "坚韧戒指"};
        QStringList rare = {"神圣戒指", "暗影戒指", "元素戒指"};
        QStringList epic = {"龙之眼", "凤凰之羽", "恶魔之角"};
        QStringList legendary = {"创世之石", "时间沙漏", "命运之轮"};

        if (quality == EquipmentQuality::COMMON) name = common[QRandomGenerator::global()->bounded(common.size())];
        else if (quality == EquipmentQuality::UNCOMMON) name = uncommon[QRandomGenerator::global()->bounded(uncommon.size())];
        else if (quality == EquipmentQuality::RARE) name = rare[QRandomGenerator::global()->bounded(rare.size())];
        else if (quality == EquipmentQuality::EPIC) name = epic[QRandomGenerator::global()->bounded(epic.size())];
        else name = legendary[QRandomGenerator::global()->bounded(legendary.size())];

        hpBonus = static_cast<int>((20 + baseBonus * 5) * multiplier);
    }

    return new Equipment(name, static_cast<EquipmentType>(type), quality, attackBonus, defenseBonus, hpBonus);
}

QString Dungeon::getRoomDescription(RoomType type)
{
    switch (type) {
    case RoomType::ENTRANCE:
        return "地牢入口，安全区域";
    case RoomType::BATTLE:
        return "普通战斗房";
    case RoomType::ELITE:
        return "精英怪物房";
    case RoomType::TREASURE:
        return "宝箱房间";
    case RoomType::BOSS:
        return "BOSS房间";
    default:
        return "未知房间";
    }
}
