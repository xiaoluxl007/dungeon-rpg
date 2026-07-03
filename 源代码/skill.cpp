#include "skill.h"

Skill::Skill()
    : m_name(""), m_type(SkillType::ATTACK), m_target(SkillTarget::SINGLE_ENEMY),
      m_damage(0), m_manaCost(0), m_unlockLevel(1), m_cooldown(0), m_currentCooldown(0)
{
}

Skill::Skill(const QString& name, SkillType type, int damage, int manaCost, int unlockLevel)
    : m_name(name), m_type(type), m_target(SkillTarget::SINGLE_ENEMY),
      m_damage(damage), m_manaCost(manaCost), m_unlockLevel(unlockLevel),
      m_cooldown(0), m_currentCooldown(0)
{
}

Skill::Skill(const QString& name, SkillType type, SkillTarget target, int damage, int manaCost, int unlockLevel)
    : m_name(name), m_type(type), m_target(target),
      m_damage(damage), m_manaCost(manaCost), m_unlockLevel(unlockLevel),
      m_cooldown(0), m_currentCooldown(0)
{
}

Skill::~Skill()
{
}

QString Skill::typeString() const
{
    switch (m_type) {
    case SkillType::ATTACK:
        return "攻击";
    case SkillType::HEAL:
        return "治疗";
    case SkillType::BUFF:
        return "增益";
    case SkillType::DEBUFF:
        return "减益";
    case SkillType::AOE:
        return "范围攻击";
    case SkillType::DOT:
        return "持续伤害";
    case SkillType::STUN:
        return "眩晕";
    case SkillType::DODGE:
        return "闪避";
    default:
        return "未知";
    }
}

QString Skill::targetString() const
{
    switch (m_target) {
    case SkillTarget::SINGLE_ENEMY:
        return "单体敌人";
    case SkillTarget::ALL_ENEMIES:
        return "所有敌人";
    case SkillTarget::SELF:
        return "自身";
    case SkillTarget::SINGLE_ALLY:
        return "单体队友";
    case SkillTarget::ALL_ALLIES:
        return "所有队友";
    default:
        return "未知";
    }
}
