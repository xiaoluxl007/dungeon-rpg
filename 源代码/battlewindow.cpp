#include "battlewindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QRandomGenerator>
#include <QTimer>
#include <QScrollBar>
#include <QCloseEvent>

BattleWindow::BattleWindow(QWidget *parent) :
    QWidget(parent),
    m_player(nullptr),
    m_enemy(nullptr),
    m_battleState(BattleState::PLAYER_TURN)
{
    setWindowTitle("战斗");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    if (parent) {
        setGeometry(parent->geometry());
    } else {
        setFixedSize(800, 600);
    }
    createUI();
}

BattleWindow::~BattleWindow()
{
}

void BattleWindow::createUI()
{
    setStyleSheet("background-color: #1a1a2e;");
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    m_turnLabel = new QLabel("你的回合", this);
    m_turnLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_turnLabel);

    QHBoxLayout* battleLayout = new QHBoxLayout();

    QWidget* playerPanel = new QWidget(this);
    QVBoxLayout* playerLayout = new QVBoxLayout(playerPanel);

    m_playerNameLabel = new QLabel("玩家", playerPanel);
    m_playerNameLabel->setAlignment(Qt::AlignCenter);
    playerLayout->addWidget(m_playerNameLabel);

    m_playerHpBar = new QProgressBar(playerPanel);
    m_playerHpBar->setMaximum(100);
    m_playerHpBar->setValue(100);
    playerLayout->addWidget(m_playerHpBar);

    m_playerHpLabel = new QLabel("100/100", playerPanel);
    m_playerHpLabel->setAlignment(Qt::AlignCenter);
    playerLayout->addWidget(m_playerHpLabel);

    m_playerManaBar = new QProgressBar(playerPanel);
    m_playerManaBar->setMaximum(100);
    m_playerManaBar->setValue(100);
    playerLayout->addWidget(m_playerManaBar);

    m_playerManaLabel = new QLabel("魔力: 100/100", playerPanel);
    m_playerManaLabel->setAlignment(Qt::AlignCenter);
    playerLayout->addWidget(m_playerManaLabel);

    m_playerAttackLabel = new QLabel("攻击: 10", playerPanel);
    playerLayout->addWidget(m_playerAttackLabel);

    m_playerDefenseLabel = new QLabel("防御: 5", playerPanel);
    playerLayout->addWidget(m_playerDefenseLabel);

    battleLayout->addWidget(playerPanel);

    QLabel* vsLabel = new QLabel("<h1>VS</h1>", this);
    vsLabel->setAlignment(Qt::AlignCenter);
    battleLayout->addWidget(vsLabel);

    QWidget* enemyPanel = new QWidget(this);
    QVBoxLayout* enemyLayout = new QVBoxLayout(enemyPanel);

    m_enemyNameLabel = new QLabel("敌人", enemyPanel);
    m_enemyNameLabel->setAlignment(Qt::AlignCenter);
    enemyLayout->addWidget(m_enemyNameLabel);

    m_enemyHpBar = new QProgressBar(enemyPanel);
    m_enemyHpBar->setMaximum(100);
    m_enemyHpBar->setValue(100);
    enemyLayout->addWidget(m_enemyHpBar);

    m_enemyHpLabel = new QLabel("100/100", enemyPanel);
    m_enemyHpLabel->setAlignment(Qt::AlignCenter);
    enemyLayout->addWidget(m_enemyHpLabel);

    m_enemyAttackLabel = new QLabel("攻击: 10", enemyPanel);
    enemyLayout->addWidget(m_enemyAttackLabel);

    m_enemyDefenseLabel = new QLabel("防御: 5", enemyPanel);
    enemyLayout->addWidget(m_enemyDefenseLabel);

    battleLayout->addWidget(enemyPanel);

    mainLayout->addLayout(battleLayout);

    m_battleLogText = new QTextEdit(this);
    m_battleLogText->setReadOnly(true);
    mainLayout->addWidget(m_battleLogText);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_attackButton = new QPushButton("攻击", this);
    connect(m_attackButton, SIGNAL(clicked()), this, SLOT(on_attackButton_clicked()));
    buttonLayout->addWidget(m_attackButton);

    m_skillButton = new QPushButton("技能", this);
    connect(m_skillButton, SIGNAL(clicked()), this, SLOT(on_skillButton_clicked()));
    buttonLayout->addWidget(m_skillButton);

    m_itemButton = new QPushButton("道具", this);
    connect(m_itemButton, SIGNAL(clicked()), this, SLOT(on_itemButton_clicked()));
    buttonLayout->addWidget(m_itemButton);

    m_escapeButton = new QPushButton("逃跑", this);
    connect(m_escapeButton, SIGNAL(clicked()), this, SLOT(on_escapeButton_clicked()));
    buttonLayout->addWidget(m_escapeButton);

    mainLayout->addLayout(buttonLayout);

    m_settlementWidget = new QWidget(this);
    m_settlementWidget->hide();
    QVBoxLayout* settlementLayout = new QVBoxLayout(m_settlementWidget);

    m_settlementLabel = new QLabel("", m_settlementWidget);
    m_settlementLabel->setAlignment(Qt::AlignCenter);
    settlementLayout->addWidget(m_settlementLabel);

    m_rewardText = new QTextEdit(m_settlementWidget);
    m_rewardText->setReadOnly(true);
    m_rewardText->setMaximumHeight(100);
    settlementLayout->addWidget(m_rewardText);

    QHBoxLayout* settlementButtonLayout = new QHBoxLayout();
    m_continueButton = new QPushButton("继续闯关", m_settlementWidget);
    connect(m_continueButton, SIGNAL(clicked()), this, SLOT(on_continueButton_clicked()));
    settlementButtonLayout->addWidget(m_continueButton);

    m_restartButton = new QPushButton("重新开始", m_settlementWidget);
    connect(m_restartButton, SIGNAL(clicked()), this, SLOT(on_restartButton_clicked()));
    settlementButtonLayout->addWidget(m_restartButton);

    m_menuButton = new QPushButton("返回主菜单", m_settlementWidget);
    connect(m_menuButton, SIGNAL(clicked()), this, SLOT(on_menuButton_clicked()));
    settlementButtonLayout->addWidget(m_menuButton);

    settlementLayout->addLayout(settlementButtonLayout);
    mainLayout->addWidget(m_settlementWidget);
}

void BattleWindow::startBattle(Character* player, Character* enemy)
{
    m_player = player;
    m_enemy = enemy;
    m_rewardItems.clear();
    m_rewardEquipments.clear();

    if (player->speed() >= enemy->speed()) {
        m_battleState = BattleState::PLAYER_TURN;
    } else {
        m_battleState = BattleState::ENEMY_TURN;
    }

    m_settlementWidget->hide();
    updateUI();
    addBattleLog("====================");
    addBattleLog("战斗开始！");
    addBattleLog(QString("你 VS %1").arg(enemy->name()));
    addBattleLog("====================");

    if (m_battleState == BattleState::ENEMY_TURN) {
        addBattleLog("敌人先行动！");
        QTimer::singleShot(1000, this, SLOT(enemyTurn()));
    }
}

void BattleWindow::setInventory(QVector<Item*> inventory)
{
    m_inventory = inventory;
    m_itemButton->setEnabled(!inventory.isEmpty());
}

void BattleWindow::updateUI()
{
    if (!m_player || !m_enemy) return;

    m_playerNameLabel->setText(m_player->name());
    m_enemyNameLabel->setText(m_enemy->name());

    m_playerHpBar->setMaximum(m_player->maxHp());
    m_playerHpBar->setValue(m_player->hp());
    m_playerHpLabel->setText(QString("%1/%2").arg(m_player->hp()).arg(m_player->maxHp()));

    m_playerManaBar->setMaximum(m_player->maxMana());
    m_playerManaBar->setValue(m_player->mana());
    m_playerManaLabel->setText(QString("魔力: %1/%2").arg(m_player->mana()).arg(m_player->maxMana()));

    m_enemyHpBar->setMaximum(m_enemy->maxHp());
    m_enemyHpBar->setValue(m_enemy->hp());
    m_enemyHpLabel->setText(QString("%1/%2").arg(m_enemy->hp()).arg(m_enemy->maxHp()));

    m_playerAttackLabel->setText(QString("攻击: %1").arg(m_player->totalAttack()));
    m_playerDefenseLabel->setText(QString("防御: %1").arg(m_player->totalDefense()));
    m_enemyAttackLabel->setText(QString("攻击: %1").arg(m_enemy->totalAttack()));
    m_enemyDefenseLabel->setText(QString("防御: %1").arg(m_enemy->totalDefense()));

    switch (m_battleState) {
    case BattleState::PLAYER_TURN:
        m_turnLabel->setText("你的回合");
        m_attackButton->setEnabled(true);
        m_skillButton->setEnabled(!m_player->getSkills().isEmpty());
        m_itemButton->setEnabled(!m_inventory.isEmpty());
        m_escapeButton->setEnabled(true);
        break;
    case BattleState::ENEMY_TURN:
        m_turnLabel->setText("敌人回合");
        m_attackButton->setEnabled(false);
        m_skillButton->setEnabled(false);
        m_itemButton->setEnabled(false);
        m_escapeButton->setEnabled(false);
        break;
    case BattleState::VICTORY:
    case BattleState::DEFEAT:
        m_turnLabel->setText(m_battleState == BattleState::VICTORY ? "胜利！" : "失败...");
        m_attackButton->setEnabled(false);
        m_skillButton->setEnabled(false);
        m_itemButton->setEnabled(false);
        m_escapeButton->setEnabled(false);
        break;
    }
}

void BattleWindow::addBattleLog(const QString& log)
{
    m_battleLogText->append(log);
    m_battleLogText->verticalScrollBar()->setValue(m_battleLogText->verticalScrollBar()->maximum());
}

void BattleWindow::on_attackButton_clicked()
{
    if (m_battleState != BattleState::PLAYER_TURN) return;
    playerAttack();
}

void BattleWindow::playerAttack()
{
    int damage;
    QString log;
    calculateDamage(m_player, m_enemy, damage, log);
    addBattleLog(log);

    bool enemyDead = m_enemy->takeDamage(damage);
    updateUI();

    if (enemyDead) {
        m_battleState = BattleState::VICTORY;
        updateUI();
        showSettlement(true);
        return;
    }

    m_battleState = BattleState::ENEMY_TURN;
    updateUI();
    QTimer::singleShot(1000, this, SLOT(enemyTurn()));
}

void BattleWindow::calculateDamage(Character* attacker, Character* defender, int& damage, QString& log)
{
    damage = attacker->totalAttack();
    int actualDamage = qMax(1, damage - defender->totalDefense());
    log = QString("%1 攻击 %2，造成 %3 点伤害").arg(attacker->name()).arg(defender->name()).arg(actualDamage);
    damage = actualDamage;
}

void BattleWindow::enemyTurn()
{
    if (m_battleState != BattleState::ENEMY_TURN) return;

    if (m_enemy->type() == CharacterType::BOSS && QRandomGenerator::global()->bounded(100) < 30) {
        int bossSkillDamage = m_enemy->totalAttack() * 2;
        int actualDamage = qMax(1, bossSkillDamage - m_player->totalDefense());
        addBattleLog(QString("%1 使出全力一击！造成 %2 点伤害！").arg(m_enemy->name()).arg(actualDamage));
        bool playerDead = m_player->takeDamage(actualDamage);
        updateUI();

        if (playerDead) {
            m_battleState = BattleState::DEFEAT;
            updateUI();
            showSettlement(false);
            return;
        }
    } else {
        int damage;
        QString log;
        calculateDamage(m_enemy, m_player, damage, log);
        addBattleLog(log);

        bool playerDead = m_player->takeDamage(damage);
        updateUI();

        if (playerDead) {
            m_battleState = BattleState::DEFEAT;
            updateUI();
            showSettlement(false);
            return;
        }
    }

    m_battleState = BattleState::PLAYER_TURN;
    int manaRegen = 5 + m_player->level();
    m_player->setMana(m_player->mana() + manaRegen);
    addBattleLog(QString("魔力恢复了 %1 点！").arg(manaRegen));
    updateUI();
}

void BattleWindow::on_skillButton_clicked()
{
    if (m_battleState != BattleState::PLAYER_TURN) return;

    QVector<Skill*>& skills = m_player->getSkills();
    if (skills.isEmpty()) {
        addBattleLog("你没有学会任何技能！");
        return;
    }

    QString skillList = "选择技能:\n";
    for (int i = 0; i < skills.size(); ++i) {
        Skill* skill = skills[i];
        QString manaStatus = m_player->mana() >= skill->manaCost() ? "" : " (魔力不足)";
        skillList += QString("%1. %2 (%3伤害, 消耗%4魔力)%5\n").arg(i + 1).arg(skill->name()).arg(skill->damage()).arg(skill->manaCost()).arg(manaStatus);
    }
    skillList += "0. 取消";

    bool ok;
    QString input = QInputDialog::getText(this, "选择技能", skillList, QLineEdit::Normal, "", &ok);
    if (ok && !input.isEmpty()) {
        int skillIndex = input.toInt() - 1;
        if (skillIndex >= 0 && skillIndex < skills.size()) {
            playerUseSkill(skillIndex);
        }
    }
}

void BattleWindow::playerUseSkill(int skillIndex)
{
    Skill* skill = m_player->getSkill(skillIndex);
    if (!skill) return;

    if (m_player->mana() < skill->manaCost()) {
        addBattleLog("魔力不足！无法使用技能！");
        return;
    }

    m_player->setMana(m_player->mana() - skill->manaCost());
    addBattleLog(QString("%1 使用了 %2！").arg(m_player->name()).arg(skill->name()));

    if (skill->type() == SkillType::ATTACK) {
        int damage = skill->damage() + m_player->totalAttack();
        int actualDamage = qMax(1, damage - m_enemy->totalDefense());
        addBattleLog(QString("造成 %1 点伤害！").arg(actualDamage));
        bool enemyDead = m_enemy->takeDamage(actualDamage);
        updateUI();

        if (enemyDead) {
            m_battleState = BattleState::VICTORY;
            updateUI();
            showSettlement(true);
            return;
        }
    } else if (skill->type() == SkillType::HEAL) {
        m_player->heal(skill->damage());
        addBattleLog(QString("恢复了 %1 点生命！").arg(skill->damage()));
        updateUI();
    }

    m_battleState = BattleState::ENEMY_TURN;
    updateUI();
    QTimer::singleShot(1000, this, SLOT(enemyTurn()));
}

void BattleWindow::on_itemButton_clicked()
{
    if (m_battleState != BattleState::PLAYER_TURN || m_inventory.isEmpty()) return;

    QString itemList = "选择道具:\n";
    for (int i = 0; i < m_inventory.size(); ++i) {
        Item* item = m_inventory[i];
        itemList += QString("%1. %2 (+%3 %4)\n").arg(i + 1).arg(item->name())
                                              .arg(item->value())
                                              .arg(item->consumableTypeString());
    }
    itemList += "0. 取消";

    bool ok;
    QString input = QInputDialog::getText(this, "选择道具", itemList, QLineEdit::Normal, "", &ok);
    if (ok && !input.isEmpty()) {
        int itemIndex = input.toInt() - 1;
        if (itemIndex >= 0 && itemIndex < m_inventory.size()) {
            useItem(itemIndex);
        }
    }
}

void BattleWindow::useItem(int itemIndex)
{
    if (itemIndex < 0 || itemIndex >= m_inventory.size()) return;

    Item* potion = m_inventory[itemIndex];
    if (potion->consumableType() == ConsumableType::HP_POTION) {
        addBattleLog(QString("使用了 %1，恢复 %2 点生命值").arg(potion->name()).arg(potion->value()));
        m_player->heal(potion->value());
        delete potion;
        m_inventory.removeAt(itemIndex);
        updateUI();

        if (m_inventory.isEmpty()) {
            m_itemButton->setEnabled(false);
        }

        m_battleState = BattleState::ENEMY_TURN;
        updateUI();
        QTimer::singleShot(1000, this, SLOT(enemyTurn()));
    }
}

void BattleWindow::on_escapeButton_clicked()
{
    if (m_battleState != BattleState::PLAYER_TURN) return;

    int chance = QRandomGenerator::global()->bounded(100);
    if (chance < 50) {
        addBattleLog("逃跑成功！");
        m_battleState = BattleState::DEFEAT;
        updateUI();
        emit battleResult(false, 0, 0, QVector<Item*>(), QVector<Equipment*>());
        close();
    } else {
        addBattleLog("逃跑失败！");
        m_battleState = BattleState::ENEMY_TURN;
        updateUI();
        QTimer::singleShot(1000, this, SLOT(enemyTurn()));
    }
}

void BattleWindow::showSettlement(bool victory)
{
    m_settlementWidget->show();

    if (victory) {
        m_settlementLabel->setText("<h2><font color='green'>战斗胜利！</font></h2>");
        m_continueButton->show();

        int exp = m_enemy->expReward();
        int gold = m_enemy->goldReward() + QRandomGenerator::global()->bounded(10);

        m_rewardText->clear();
        m_rewardText->append(QString("获得经验: %1").arg(exp));
        m_rewardText->append(QString("获得金币: %1").arg(gold));

        if (m_enemy->type() == CharacterType::BOSS || m_enemy->type() == CharacterType::ELITE) {
            if (QRandomGenerator::global()->bounded(100) < 50) {
                Item* item = new Item("生命药水", ItemType::CONSUMABLE, 50, ConsumableType::HP_POTION);
                m_rewardItems.append(item);
                m_rewardText->append(QString("获得道具: %1").arg(item->name()));
            }
            if (QRandomGenerator::global()->bounded(100) < 30) {
                Equipment* eq = new Equipment("铁剑", EquipmentType::WEAPON, 5, 0, 0);
                m_rewardEquipments.append(eq);
                m_rewardText->append(QString("获得装备: %1 (+%2 攻击)").arg(eq->name()).arg(eq->attackBonus()));
            }
        }

        emit battleResult(true, exp, gold, m_rewardItems, m_rewardEquipments);
    } else {
        m_settlementLabel->setText("<h2><font color='red'>战斗失败...</font></h2>");
        m_continueButton->hide();
        m_rewardText->clear();
        m_rewardText->append("你被击败了...");
        m_rewardText->append("点击\"重新开始\"再次挑战");
        m_rewardText->append("或点击\"返回主菜单\"");
    }
}

void BattleWindow::on_continueButton_clicked()
{
    close();
}

void BattleWindow::on_restartButton_clicked()
{
    bool isBossBattle = (m_enemy && (m_enemy->type() == CharacterType::BOSS));
    emit restartGame(isBossBattle);
    close();
}

void BattleWindow::on_menuButton_clicked()
{
    emit battleResult(false, 0, 0, QVector<Item*>(), QVector<Equipment*>());
    close();
}

void BattleWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    QWidget::closeEvent(event);
}
