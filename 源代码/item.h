#ifndef ITEM_H
#define ITEM_H

#include <QString>

enum class ItemType {
    CONSUMABLE,
    EQUIPMENT,
    KEY_ITEM
};

enum class ConsumableType {
    HP_POTION,
    MP_POTION,
    BUFF_ITEM
};

class Item
{
public:
    Item();
    Item(const QString& name, ItemType type, int value, ConsumableType consumableType = ConsumableType::HP_POTION);
    ~Item();

    QString name() const { return m_name; }
    ItemType type() const { return m_type; }
    ConsumableType consumableType() const { return m_consumableType; }
    int value() const { return m_value; }

    void setName(const QString& name) { m_name = name; }
    void setType(ItemType type) { m_type = type; }
    void setConsumableType(ConsumableType type) { m_consumableType = type; }
    void setValue(int value) { m_value = value; }

    QString typeString() const;
    QString consumableTypeString() const;

private:
    QString m_name;
    ItemType m_type;
    ConsumableType m_consumableType;
    int m_value;
};

#endif // ITEM_H
