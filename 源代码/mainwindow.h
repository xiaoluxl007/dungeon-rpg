#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPixmap>
#include <QPainter>
#include <QVector>
#include "character.h"
#include "dungeon.h"
#include "item.h"
#include "equipment.h"
#include "skill.h"
#include "battlewindow.h"
#include "savemanager.h"
#include "shop.h"

struct Room;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startGameButton_clicked();
    void on_loadGameButton_clicked();
    void on_deleteSaveButton_clicked();
    void on_gameHelpButton_clicked();
    void on_exitButton_clicked();
    void on_battleResult(bool victory, int exp, int gold, QVector<Item*> items, QVector<Equipment*> equipments);
    void on_battleClosed();
    void on_restartGame(bool restartFromBeginning);

    void on_moveUpButton_clicked();
    void on_moveDownButton_clicked();
    void on_moveLeftButton_clicked();
    void on_moveRightButton_clicked();

    void on_nextFloorButton_clicked();
    void on_backToMenuButton_clicked();

    void on_useItemButton_clicked();
    void on_equipItemButton_clicked();
    void on_unequipItemButton_clicked();
    void on_showSkillsButton_clicked();

    void on_openShopButton_clicked();
    void on_shopClosed();
    void on_goldChanged(int newGold);
    void on_itemPurchasedFromShop(Item* item);
    void on_equipmentPurchasedFromShop(Equipment* equipment);
    void on_shopRefreshRequested();

    void closeEvent(QCloseEvent* event) override;

private:
    Character* m_player;
    Dungeon* m_dungeon;
    SaveManager* m_saveManager;
    QVector<Item*> m_inventory;
    QVector<Equipment*> m_equipmentStorage;
    QVector<Skill*> m_skills;
    BattleWindow* m_battleWindow;
    Shop* m_shopWindow;
    bool m_inGame;
    int m_playerX, m_playerY;
    bool m_explored[5][5];

    QVector<Item*> m_shopItems;
    QVector<Equipment*> m_shopEquipments;
    int m_shopRefreshCost;
    int m_currentFloor;
    int m_currentSaveSlot;
    QString m_currentSaveName;

    QWidget* m_centralContainer;
    QWidget* m_mainMenuWidget;
    QWidget* m_gameScreenWidget;
    QWidget* m_mapWidget;
    QGridLayout* m_mapLayout;
    QLabel* m_mapCells[5][5];

    QLabel* m_playerNameLabel;
    QLabel* m_levelLabel;
    QProgressBar* m_hpBar;
    QLabel* m_hpLabel;
    QProgressBar* m_manaBar;
    QLabel* m_manaLabel;
    QLabel* m_attackLabel;
    QLabel* m_defenseLabel;
    QLabel* m_goldLabel;
    QLabel* m_expLabel;

    QLabel* m_floorLabel;
    QTextEdit* m_logText;

    QPushButton* m_startGameButton;
    QPushButton* m_loadGameButton;
    QPushButton* m_deleteSaveButton;
    QPushButton* m_gameHelpButton;
    QPushButton* m_exitButton;

    QPushButton* m_moveUpButton;
    QPushButton* m_moveDownButton;
    QPushButton* m_moveLeftButton;
    QPushButton* m_moveRightButton;
    QPushButton* m_nextFloorButton;
    QPushButton* m_backToMenuButton;
    QPushButton* m_useItemButton;
    QPushButton* m_equipItemButton;
    QPushButton* m_unequipItemButton;
    QPushButton* m_showSkillsButton;
    QPushButton* m_openShopButton;

    void initPlayer(const QString& name);
    void initDungeon();
    void initSkills();
    void createSkillTemplates();
    void createMainMenu();
    void createGameScreen();
    void showMainMenu();
    void showGameScreen();
    void updatePlayerStats();
    void updateDungeonMap();
    void handleRoomEvent(Room room);
    void addLogMessage(const QString& message);
    void checkLevelUp();
    void showItemDialog();
    void showEquipmentDialog();
    void showSkillDialog();
    void updateExplored();
    QPixmap createRoomPixmap(RoomType type, bool isPlayerPos, bool hasFog);
    void generateShopItemsForFloor();
    void generateShopEquipmentsForFloor();
    void autoSave();
};

#endif // MAINWINDOW_H