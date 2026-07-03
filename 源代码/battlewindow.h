#ifndef BATTLEWINDOW_H
#define BATTLEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include "character.h"
#include "item.h"
#include "skill.h"

enum class BattleState {
    PLAYER_TURN,
    ENEMY_TURN,
    VICTORY,
    DEFEAT
};

class BattleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BattleWindow(QWidget *parent = nullptr);
    ~BattleWindow();

    void startBattle(Character* player, Character* enemy);
    void setInventory(QVector<Item*> inventory);

signals:
    void battleResult(bool victory, int exp, int gold, QVector<Item*> items, QVector<Equipment*> equipments);
    void closed();
    void restartGame(bool restartFromBeginning);

private slots:
    void on_attackButton_clicked();
    void on_skillButton_clicked();
    void on_itemButton_clicked();
    void on_escapeButton_clicked();
    void on_continueButton_clicked();
    void on_restartButton_clicked();
    void on_menuButton_clicked();
    void enemyTurn();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Character* m_player;
    Character* m_enemy;
    QVector<Item*> m_inventory;
    QVector<Item*> m_rewardItems;
    QVector<Equipment*> m_rewardEquipments;
    BattleState m_battleState;

    QLabel* m_turnLabel;
    QLabel* m_playerNameLabel;
    QLabel* m_enemyNameLabel;
    QProgressBar* m_playerHpBar;
    QProgressBar* m_enemyHpBar;
    QLabel* m_playerHpLabel;
    QLabel* m_enemyHpLabel;
    QLabel* m_playerAttackLabel;
    QLabel* m_playerDefenseLabel;
    QProgressBar* m_playerManaBar;
    QLabel* m_playerManaLabel;
    QLabel* m_enemyAttackLabel;
    QLabel* m_enemyDefenseLabel;
    QTextEdit* m_battleLogText;
    QPushButton* m_attackButton;
    QPushButton* m_skillButton;
    QPushButton* m_itemButton;
    QPushButton* m_escapeButton;
    QPushButton* m_continueButton;
    QPushButton* m_restartButton;
    QPushButton* m_menuButton;

    QWidget* m_settlementWidget;
    QLabel* m_settlementLabel;
    QTextEdit* m_rewardText;

    void createUI();
    void updateUI();
    void playerAttack();
    void playerUseSkill(int skillIndex);
    void useItem(int itemIndex);
    void escape();
    void calculateDamage(Character* attacker, Character* defender, int& damage, QString& log);
    void showSettlement(bool victory);
    void addBattleLog(const QString& log);
};

#endif // BATTLEWINDOW_H
