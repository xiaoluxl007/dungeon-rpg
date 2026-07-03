#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <QString>

enum class EquipmentType {
    WEAPON,
    ARMOR,
    ACCESSORY
};

enum class EquipmentQuality {
    COMMON,     // 普通 - 白色
    UNCOMMON,   // 优秀 - 绿色
    RARE,       // 稀有 - 蓝色
    EPIC,       // 史诗 - 紫色
    LEGENDARY   // 传说 - 橙色
};

class Equipment
{
public:
    Equipment();
    Equipment(const QString& name, EquipmentType type, int attackBonus, int defenseBonus, int hpBonus);
    Equipment(const QString& name, EquipmentType type, EquipmentQuality quality,
              int attackBonus, int defenseBonus, int hpBonus);
    ~Equipment();

    QString name() const { return m_name; }
    EquipmentType type() const { return m_type; }
    EquipmentQuality quality() const { return m_quality; }
    int attackBonus() const { return m_attackBonus; }
    int defenseBonus() const { return m_defenseBonus; }
    int hpBonus() const { return m_hpBonus; }

    void setName(const QString& name) { m_name = name; }
    void setType(EquipmentType type) { m_type = type; }
    void setQuality(EquipmentQuality quality) { m_quality = quality; }
    void setAttackBonus(int bonus) { m_attackBonus = bonus; }
    void setDefenseBonus(int bonus) { m_defenseBonus = bonus; }
    void setHpBonus(int bonus) { m_hpBonus = bonus; }

    QString typeString() const;
    QString qualityString() const;
    QString qualityColor() const;  // 返回CSS颜色代码
    int getPrice() const;          // 获取售价

private:
    QString m_name;
    EquipmentType m_type;
    EquipmentQuality m_quality;
    int m_attackBonus;
    int m_defenseBonus;
    int m_hpBonus;
};

#endif // EQUIPMENT_H
