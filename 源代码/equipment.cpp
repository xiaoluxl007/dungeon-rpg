#include "equipment.h"

Equipment::Equipment()
    : m_name(""), m_type(EquipmentType::WEAPON), m_quality(EquipmentQuality::COMMON),
      m_attackBonus(0), m_defenseBonus(0), m_hpBonus(0)
{
}

Equipment::Equipment(const QString& name, EquipmentType type, int attackBonus,
                     int defenseBonus, int hpBonus)
    : m_name(name), m_type(type), m_quality(EquipmentQuality::COMMON),
      m_attackBonus(attackBonus), m_defenseBonus(defenseBonus), m_hpBonus(hpBonus)
{
}

Equipment::Equipment(const QString& name, EquipmentType type, EquipmentQuality quality,
                     int attackBonus, int defenseBonus, int hpBonus)
    : m_name(name), m_type(type), m_quality(quality),
      m_attackBonus(attackBonus), m_defenseBonus(defenseBonus), m_hpBonus(hpBonus)
{
}

Equipment::~Equipment()
{
}

QString Equipment::typeString() const
{
    switch (m_type) {
    case EquipmentType::WEAPON:
        return "武器";
    case EquipmentType::ARMOR:
        return "防具";
    case EquipmentType::ACCESSORY:
        return "饰品";
    default:
        return "未知";
    }
}

QString Equipment::qualityString() const
{
    switch (m_quality) {
    case EquipmentQuality::COMMON:
        return "普通";
    case EquipmentQuality::UNCOMMON:
        return "优秀";
    case EquipmentQuality::RARE:
        return "稀有";
    case EquipmentQuality::EPIC:
        return "史诗";
    case EquipmentQuality::LEGENDARY:
        return "传说";
    default:
        return "普通";
    }
}

QString Equipment::qualityColor() const
{
    switch (m_quality) {
    case EquipmentQuality::COMMON:
        return "#FFFFFF";    // 白色
    case EquipmentQuality::UNCOMMON:
        return "#00FF00";   // 绿色
    case EquipmentQuality::RARE:
        return "#0080FF";   // 蓝色
    case EquipmentQuality::EPIC:
        return "#8000FF";   // 紫色
    case EquipmentQuality::LEGENDARY:
        return "#FF8000";    // 橙色
    default:
        return "#FFFFFF";
    }
}

int Equipment::getPrice() const
{
    int basePrice = 10;
    int totalBonus = qAbs(m_attackBonus) + qAbs(m_defenseBonus) + qAbs(m_hpBonus) / 5;
    int qualityMultiplier = 1;

    switch (m_quality) {
    case EquipmentQuality::COMMON:
        qualityMultiplier = 1;
        break;
    case EquipmentQuality::UNCOMMON:
        qualityMultiplier = 2;
        break;
    case EquipmentQuality::RARE:
        qualityMultiplier = 4;
        break;
    case EquipmentQuality::EPIC:
        qualityMultiplier = 8;
        break;
    case EquipmentQuality::LEGENDARY:
        qualityMultiplier = 16;
        break;
    default:
        qualityMultiplier = 1;
    }

    return basePrice + totalBonus * qualityMultiplier * 5;
}
