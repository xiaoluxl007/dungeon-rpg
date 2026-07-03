#ifndef SKILL_H
#define SKILL_H

#include <QString>
#include <QVector>

enum class SkillType {
    ATTACK,      // 普通攻击技能
    HEAL,        // 治疗技能
    BUFF,        // 增益技能
    DEBUFF,      // 减益技能
    AOE,         // 范围攻击（AOE）
    DOT,         // 持续伤害
    STUN,        // 眩晕
    DODGE        // 闪避增益
};

enum class SkillTarget {
    SINGLE_ENEMY,    // 单体敌人
    ALL_ENEMIES,     // 所有敌人
    SELF,            // 自身
    SINGLE_ALLY,     // 单体队友
    ALL_ALLIES       // 所有队友
};

class Skill
{
public:
    Skill();
    Skill(const QString& name, SkillType type, int damage, int manaCost, int unlockLevel);
    Skill(const QString& name, SkillType type, SkillTarget target, int damage, int manaCost, int unlockLevel);
    ~Skill();

    QString name() const { return m_name; }
    SkillType type() const { return m_type; }
    SkillTarget target() const { return m_target; }
    int damage() const { return m_damage; }
    int manaCost() const { return m_manaCost; }
    int unlockLevel() const { return m_unlockLevel; }
    int cooldown() const { return m_cooldown; }
    int currentCooldown() const { return m_currentCooldown; }
    void setCurrentCooldown(int cd) { m_currentCooldown = cd; }
    void decrementCooldown() { if (m_currentCooldown > 0) m_currentCooldown--; }

    void setName(const QString& name) { m_name = name; }
    void setType(SkillType type) { m_type = type; }
    void setTarget(SkillTarget target) { m_target = target; }
    void setDamage(int damage) { m_damage = damage; }
    void setManaCost(int cost) { m_manaCost = cost; }
    void setUnlockLevel(int level) { m_unlockLevel = level; }
    void setCooldown(int cd) { m_cooldown = cd; m_currentCooldown = 0; }

    QString typeString() const;
    QString targetString() const;

private:
    QString m_name;
    SkillType m_type;
    SkillTarget m_target;
    int m_damage;
    int m_manaCost;
    int m_unlockLevel;
    int m_cooldown;         // 冷却回合数
    int m_currentCooldown;  // 当前冷却剩余
};

#endif // SKILL_H
