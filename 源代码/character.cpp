#include "character.h"

Character::Character()
    : m_hp(100), m_maxHp(100), m_mana(100), m_maxMana(100),
      m_name(""), m_type(CharacterType::PLAYER),
      m_attack(10), m_defense(5), m_speed(5), m_level(1),
      m_experience(0), m_gold(0), m_expReward(0), m_goldReward(0)
{
}

Character::Character(const QString& name, int hp, int attack, int defense, int speed, int level)
    : m_hp(hp), m_maxHp(hp),
      m_mana(100 + (level - 1) * 20), m_maxMana(100 + (level - 1) * 20),
      m_name(name), m_type(CharacterType::PLAYER),
      m_attack(attack), m_defense(defense), m_speed(speed), m_level(level),
      m_experience(0), m_gold(0), m_expReward(0), m_goldReward(0)
{
}

Character::Character(const QString& name, CharacterType type, int hp, int attack,
                     int defense, int speed, int level, int expReward, int goldReward)
    : m_hp(hp), m_maxHp(hp), m_mana(0), m_maxMana(0),
      m_name(name), m_type(type),
      m_attack(attack), m_defense(defense), m_speed(speed), m_level(level),
      m_experience(0), m_gold(0), m_expReward(expReward), m_goldReward(goldReward)
{
}

Character::~Character()
{
    qDeleteAll(m_equipment);
    m_equipment.clear();
}

bool Character::takeDamage(int damage)
{
    m_hp -= damage;
    if (m_hp < 0) {
        m_hp = 0;
        return true;
    }
    return false;
}

void Character::heal(int amount)
{
    m_hp += amount;
    if (m_hp > maxHp()) {
        m_hp = maxHp();
    }
}

void Character::levelUp()
{
    m_level++;
    m_maxHp += 20;
    m_hp = maxHp();
    m_maxMana += 20;
    m_mana = maxMana();
    m_attack += 5;
    m_defense += 3;
    m_speed += 2;
}

void Character::setInitialStats()
{
    m_maxHp = 100 + (m_level - 1) * 20;
    m_hp = m_maxHp;
    m_maxMana = 100 + (m_level - 1) * 20;
    m_mana = m_maxMana;
    m_attack = 10 + (m_level - 1) * 5;
    m_defense = 5 + (m_level - 1) * 3;
    m_speed = 5 + (m_level - 1) * 2;
}

void Character::equipItem(Equipment* equipment)
{
    unequipItem(equipment->type());
    m_equipment.append(equipment);
}

void Character::unequipItem(EquipmentType type)
{
    for (int i = 0; i < m_equipment.size(); ++i) {
        if (m_equipment[i]->type() == type) {
            delete m_equipment[i];
            m_equipment.removeAt(i);
            break;
        }
    }
}

Equipment* Character::getEquipment(EquipmentType type) const
{
    for (Equipment* eq : m_equipment) {
        if (eq->type() == type) {
            return eq;
        }
    }
    return nullptr;
}

void Character::learnSkill(Skill* skill)
{
    for (Skill* s : m_skills) {
        if (s->name() == skill->name()) {
            return;
        }
    }
    m_skills.append(skill);
}

void Character::forgetSkill(int index)
{
    if (index >= 0 && index < m_skills.size()) {
        delete m_skills[index];
        m_skills.removeAt(index);
    }
}

int Character::getEquipmentAttackBonus() const
{
    int bonus = 0;
    for (const Equipment* eq : m_equipment) {
        bonus += eq->attackBonus();
    }
    return bonus;
}

int Character::getEquipmentDefenseBonus() const
{
    int bonus = 0;
    for (const Equipment* eq : m_equipment) {
        bonus += eq->defenseBonus();
    }
    return bonus;
}

int Character::getEquipmentHpBonus() const
{
    int bonus = 0;
    for (const Equipment* eq : m_equipment) {
        bonus += eq->hpBonus();
    }
    return bonus;
}
