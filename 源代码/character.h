#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>
#include <QVector>
#include "equipment.h"
#include "skill.h"

enum class CharacterType {
    PLAYER,
    MONSTER,
    ELITE,
    BOSS
};

class Character
{
public:
    Character();
    Character(const QString& name, int hp, int attack, int defense, int speed, int level);
    Character(const QString& name, CharacterType type, int hp, int attack,
              int defense, int speed, int level, int expReward = 0, int goldReward = 0);
    ~Character();

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    CharacterType type() const { return m_type; }
    void setType(CharacterType type) { m_type = type; }

    int hp() const { return m_hp; }
    void setHp(int hp) { m_hp = hp; if (m_hp < 0) m_hp = 0; if (m_hp > maxHp()) m_hp = maxHp(); }

    int maxHp() const { return m_maxHp + getEquipmentHpBonus(); }
    int baseMaxHp() const { return m_maxHp; }
    void setMaxHp(int maxHp) { m_maxHp = maxHp; }
    int m_hp, m_maxHp;
    int m_mana, m_maxMana;

    int mana() const { return m_mana; }
    void setMana(int mana) { m_mana = mana; if (m_mana < 0) m_mana = 0; if (m_mana > maxMana()) m_mana = maxMana(); }
    int maxMana() const { return m_maxMana; }
    void setMaxMana(int maxMana) { m_maxMana = maxMana; }

    int baseAttack() const { return m_attack; }
    int totalAttack() const { return m_attack + getEquipmentAttackBonus(); }
    void setAttack(int attack) { m_attack = attack; }

    int baseDefense() const { return m_defense; }
    int totalDefense() const { return m_defense + getEquipmentDefenseBonus(); }
    void setDefense(int defense) { m_defense = defense; }

    int speed() const { return m_speed; }
    void setSpeed(int speed) { m_speed = speed; }

    int level() const { return m_level; }
    void setLevel(int level) { m_level = level; }

    int experience() const { return m_experience; }
    void setExperience(int exp) { m_experience = exp; }

    int gold() const { return m_gold; }
    void setGold(int gold) { m_gold = gold; }

    int expReward() const { return m_expReward; }
    void setExpReward(int exp) { m_expReward = exp; }

    int goldReward() const { return m_goldReward; }
    void setGoldReward(int gold) { m_goldReward = gold; }

    bool takeDamage(int damage);
    void heal(int amount);
    void levelUp();

    bool isDead() const { return m_hp <= 0; }

    void equipItem(Equipment* equipment);
    void unequipItem(EquipmentType type);
    Equipment* getEquipment(EquipmentType type) const;
    QVector<Equipment*>& getEquipmentList() { return m_equipment; }

    void learnSkill(Skill* skill);
    void forgetSkill(int index);
    void clearSkills() { m_skills.clear(); }
    QVector<Skill*>& getSkills() { return m_skills; }
    Skill* getSkill(int index) { return index >= 0 && index < m_skills.size() ? m_skills[index] : nullptr; }

    int getEquipmentAttackBonus() const;
    int getEquipmentDefenseBonus() const;
    int getEquipmentHpBonus() const;

    void setInitialStats();

private:
    QString m_name;
    CharacterType m_type;
    int m_attack;
    int m_defense;
    int m_speed;
    int m_level;
    int m_experience;
    int m_gold;
    int m_expReward;
    int m_goldReward;
    QVector<Equipment*> m_equipment;
    QVector<Skill*> m_skills;
};

#endif // CHARACTER_H
