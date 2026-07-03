#include "item.h"

Item::Item()
    : m_name(""), m_type(ItemType::CONSUMABLE), m_consumableType(ConsumableType::HP_POTION), m_value(0)
{
}

Item::Item(const QString& name, ItemType type, int value, ConsumableType consumableType)
    : m_name(name), m_type(type), m_consumableType(consumableType), m_value(value)
{
}

Item::~Item()
{
}

QString Item::typeString() const
{
    switch (m_type) {
    case ItemType::CONSUMABLE:
        return "消耗品";
    case ItemType::EQUIPMENT:
        return "装备";
    case ItemType::KEY_ITEM:
        return "关键道具";
    default:
        return "未知";
    }
}

QString Item::consumableTypeString() const
{
    switch (m_consumableType) {
    case ConsumableType::HP_POTION:
        return "生命药水";
    case ConsumableType::MP_POTION:
        return "魔法药水";
    case ConsumableType::BUFF_ITEM:
        return "增益道具";
    default:
        return "未知";
    }
}
