#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_player(nullptr),
      m_dungeon(nullptr),
      m_saveManager(new SaveManager()),
      m_battleWindow(nullptr),
      m_shopWindow(nullptr),
      m_inGame(false),
      m_playerX(0),
      m_playerY(0),
      m_shopRefreshCost(50),
      m_currentFloor(1),
      m_currentSaveSlot(-1)
{
    setWindowTitle("暗狱回廊：逐暗旅人");
    setFixedSize(1100, 680);

    m_centralContainer = new QWidget(this);
    setCentralWidget(m_centralContainer);

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            m_explored[y][x] = false;
        }
    }

    createMainMenu();
    createGameScreen();
    showMainMenu();
}

MainWindow::~MainWindow()
{
    delete m_player;
    delete m_dungeon;
    delete m_saveManager;
    delete m_battleWindow;
    delete m_shopWindow;
    qDeleteAll(m_inventory);
    qDeleteAll(m_equipmentStorage);
    qDeleteAll(m_skills);
    qDeleteAll(m_shopItems);
    qDeleteAll(m_shopEquipments);
}

void MainWindow::createMainMenu()
{
    m_mainMenuWidget = new QWidget(m_centralContainer);
    QVBoxLayout* containerLayout = new QVBoxLayout(m_centralContainer);
    containerLayout->setContentsMargins(150, 50, 150, 50);
    containerLayout->addWidget(m_mainMenuWidget);
    QVBoxLayout* layout = new QVBoxLayout(m_mainMenuWidget);
    layout->setContentsMargins(60, 40, 60, 40);
    layout->setSpacing(15);

    QLabel* titleLabel = new QLabel("<h1>暗狱回廊：逐暗旅人</h1>", m_mainMenuWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    m_startGameButton = new QPushButton("开始新游戏", m_mainMenuWidget);
    m_startGameButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(m_startGameButton, SIGNAL(clicked()), this, SLOT(on_startGameButton_clicked()));
    layout->addWidget(m_startGameButton);

    m_loadGameButton = new QPushButton("读取存档", m_mainMenuWidget);
    m_loadGameButton->setEnabled(m_saveManager->hasSaveFile());
    m_loadGameButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(m_loadGameButton, SIGNAL(clicked()), this, SLOT(on_loadGameButton_clicked()));
    layout->addWidget(m_loadGameButton);

    m_deleteSaveButton = new QPushButton("删除存档", m_mainMenuWidget);
    m_deleteSaveButton->setEnabled(m_saveManager->hasSaveFile());
    m_deleteSaveButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(m_deleteSaveButton, SIGNAL(clicked()), this, SLOT(on_deleteSaveButton_clicked()));
    layout->addWidget(m_deleteSaveButton);

    m_gameHelpButton = new QPushButton("游戏说明", m_mainMenuWidget);
    m_gameHelpButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(m_gameHelpButton, SIGNAL(clicked()), this, SLOT(on_gameHelpButton_clicked()));
    layout->addWidget(m_gameHelpButton);

    m_exitButton = new QPushButton("退出游戏", m_mainMenuWidget);
    m_exitButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(m_exitButton, SIGNAL(clicked()), this, SLOT(on_exitButton_clicked()));
    layout->addWidget(m_exitButton);
}

void MainWindow::createGameScreen()
{
    m_gameScreenWidget = new QWidget(m_centralContainer);
    QVBoxLayout* containerLayout = qobject_cast<QVBoxLayout*>(m_centralContainer->layout());
    if (containerLayout) {
        containerLayout->addWidget(m_gameScreenWidget);
    }
    QHBoxLayout* mainLayout = new QHBoxLayout(m_gameScreenWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    QWidget* leftPanel = new QWidget(m_gameScreenWidget);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftPanel->setFixedWidth(160);
    leftLayout->setSpacing(6);
    leftLayout->setContentsMargins(5, 5, 5, 5);

    m_playerNameLabel = new QLabel("角色名称", leftPanel);
    m_playerNameLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(m_playerNameLabel);

    m_levelLabel = new QLabel("等级: 1", leftPanel);
    leftLayout->addWidget(m_levelLabel);

    m_hpBar = new QProgressBar(leftPanel);
    m_hpBar->setMaximum(100);
    m_hpBar->setValue(100);
    leftLayout->addWidget(m_hpBar);

    m_hpLabel = new QLabel("100/100", leftPanel);
    leftLayout->addWidget(m_hpLabel);

    m_manaBar = new QProgressBar(leftPanel);
    m_manaBar->setMaximum(100);
    m_manaBar->setValue(100);
    leftLayout->addWidget(m_manaBar);

    m_manaLabel = new QLabel("魔力: 100/100", leftPanel);
    leftLayout->addWidget(m_manaLabel);

    m_attackLabel = new QLabel("攻击: 10", leftPanel);
    leftLayout->addWidget(m_attackLabel);

    m_defenseLabel = new QLabel("防御: 5", leftPanel);
    leftLayout->addWidget(m_defenseLabel);

    m_goldLabel = new QLabel("金币: 0", leftPanel);
    leftLayout->addWidget(m_goldLabel);

    m_expLabel = new QLabel("经验: 0", leftPanel);
    leftLayout->addWidget(m_expLabel);

    leftLayout->addSpacing(10);

    QLabel* actionLabel = new QLabel("操作:", leftPanel);
    leftLayout->addWidget(actionLabel);

    m_useItemButton = new QPushButton("使用道具", leftPanel);
    connect(m_useItemButton, SIGNAL(clicked()), this, SLOT(on_useItemButton_clicked()));
    leftLayout->addWidget(m_useItemButton);

    m_equipItemButton = new QPushButton("穿戴装备", leftPanel);
    connect(m_equipItemButton, SIGNAL(clicked()), this, SLOT(on_equipItemButton_clicked()));
    leftLayout->addWidget(m_equipItemButton);

    m_unequipItemButton = new QPushButton("卸下装备", leftPanel);
    connect(m_unequipItemButton, SIGNAL(clicked()), this, SLOT(on_unequipItemButton_clicked()));
    leftLayout->addWidget(m_unequipItemButton);

    m_showSkillsButton = new QPushButton("查看技能", leftPanel);
    connect(m_showSkillsButton, SIGNAL(clicked()), this, SLOT(on_showSkillsButton_clicked()));
    leftLayout->addWidget(m_showSkillsButton);

    leftLayout->addStretch();

    m_openShopButton = new QPushButton("进入商店", leftPanel);
    connect(m_openShopButton, SIGNAL(clicked()), this, SLOT(on_openShopButton_clicked()));
    leftLayout->addWidget(m_openShopButton);

    m_backToMenuButton = new QPushButton("返回主菜单", leftPanel);
    connect(m_backToMenuButton, SIGNAL(clicked()), this, SLOT(on_backToMenuButton_clicked()));
    leftLayout->addWidget(m_backToMenuButton);

    mainLayout->addWidget(leftPanel);

    QWidget* centerPanel = new QWidget(m_gameScreenWidget);
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(5, 5, 5, 5);

    m_floorLabel = new QLabel("第 1 层 / 共 5 层", centerPanel);
    m_floorLabel->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(m_floorLabel);

    m_mapWidget = new QWidget(centerPanel);
    m_mapLayout = new QGridLayout(m_mapWidget);
    m_mapLayout->setSpacing(2);
    m_mapLayout->setContentsMargins(0, 0, 0, 0);

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            QLabel* cell = new QLabel(m_mapWidget);
            cell->setMinimumSize(60, 60);
            cell->setMaximumSize(70, 70);
            cell->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            cell->setAlignment(Qt::AlignCenter);
            m_mapCells[y][x] = cell;
            m_mapLayout->addWidget(cell, y, x);
        }
    }
    centerLayout->addWidget(m_mapWidget, 1);

    QHBoxLayout* controlLayout = new QHBoxLayout();

    QVBoxLayout* moveLayout = new QVBoxLayout();
    moveLayout->setSpacing(5);

    QHBoxLayout* upLayout = new QHBoxLayout();
    upLayout->addStretch();
    m_moveUpButton = new QPushButton("↑", centerPanel);
    m_moveUpButton->setFixedSize(50, 30);
    connect(m_moveUpButton, SIGNAL(clicked()), this, SLOT(on_moveUpButton_clicked()));
    upLayout->addWidget(m_moveUpButton);
    upLayout->addStretch();
    moveLayout->addLayout(upLayout);

    QHBoxLayout* centerButtonLayout = new QHBoxLayout();
    centerButtonLayout->setSpacing(5);
    m_moveLeftButton = new QPushButton("←", centerPanel);
    m_moveLeftButton->setFixedSize(50, 30);
    connect(m_moveLeftButton, SIGNAL(clicked()), this, SLOT(on_moveLeftButton_clicked()));
    centerButtonLayout->addWidget(m_moveLeftButton);

    m_moveRightButton = new QPushButton("→", centerPanel);
    m_moveRightButton->setFixedSize(50, 30);
    connect(m_moveRightButton, SIGNAL(clicked()), this, SLOT(on_moveRightButton_clicked()));
    centerButtonLayout->addWidget(m_moveRightButton);
    moveLayout->addLayout(centerButtonLayout);

    QHBoxLayout* downLayout = new QHBoxLayout();
    downLayout->addStretch();
    m_moveDownButton = new QPushButton("↓", centerPanel);
    m_moveDownButton->setFixedSize(50, 30);
    connect(m_moveDownButton, SIGNAL(clicked()), this, SLOT(on_moveDownButton_clicked()));
    downLayout->addWidget(m_moveDownButton);
    downLayout->addStretch();
    moveLayout->addLayout(downLayout);

    controlLayout->addLayout(moveLayout);

    QVBoxLayout* rightButtonLayout = new QVBoxLayout();
    rightButtonLayout->setSpacing(10);
    m_nextFloorButton = new QPushButton("进入下一层", centerPanel);
    connect(m_nextFloorButton, SIGNAL(clicked()), this, SLOT(on_nextFloorButton_clicked()));
    rightButtonLayout->addWidget(m_nextFloorButton);
    rightButtonLayout->addStretch();

    controlLayout->addLayout(rightButtonLayout);
    centerLayout->addLayout(controlLayout);

    mainLayout->addWidget(centerPanel);

    QWidget* rightPanel = new QWidget(m_gameScreenWidget);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightPanel->setFixedWidth(200);
    rightLayout->setContentsMargins(5, 5, 5, 5);

    QLabel* logLabel = new QLabel("日志", rightPanel);
    rightLayout->addWidget(logLabel);

    m_logText = new QTextEdit(rightPanel);
    m_logText->setReadOnly(true);
    rightLayout->addWidget(m_logText);

    mainLayout->addWidget(rightPanel);
}

void MainWindow::showMainMenu()
{
    m_mainMenuWidget->show();
    m_gameScreenWidget->hide();
    m_inGame = false;
    m_loadGameButton->setEnabled(m_saveManager->hasSaveFile());
    m_deleteSaveButton->setEnabled(m_saveManager->hasSaveFile());
}

void MainWindow::showGameScreen()
{
    m_mainMenuWidget->hide();
    m_gameScreenWidget->show();
    m_inGame = true;
}

void MainWindow::initPlayer(const QString& name)
{
    m_player = new Character(name, 100, 10, 5, 5, 1);
    m_player->setMaxMana(100);
    m_player->setMana(100);
}

void MainWindow::initDungeon()
{
    m_dungeon = new Dungeon(5);
}

void MainWindow::createSkillTemplates()
{
    m_skills.clear();
    m_skills.append(new Skill("重击", SkillType::ATTACK, SkillTarget::SINGLE_ENEMY, 10, 15, 1));
    m_skills.append(new Skill("治愈术", SkillType::HEAL, SkillTarget::SELF, 15, 30, 1));
    m_skills.append(new Skill("能量冲击", SkillType::ATTACK, SkillTarget::SINGLE_ENEMY, 15, 25, 2));
    m_skills.append(new Skill("火焰斩", SkillType::AOE, SkillTarget::ALL_ENEMIES, 25, 20, 2));
    m_skills.append(new Skill("圣光术", SkillType::HEAL, SkillTarget::SELF, 30, 50, 3));
    m_skills.append(new Skill("雷霆一击", SkillType::ATTACK, SkillTarget::SINGLE_ENEMY, 25, 40, 3));
    m_skills.append(new Skill("毒刺", SkillType::DOT, SkillTarget::SINGLE_ENEMY, 20, 10, 4));
    m_skills.append(new Skill("护体石肤", SkillType::BUFF, SkillTarget::SELF, 20, 15, 4));
    m_skills.append(new Skill("地震波", SkillType::AOE, SkillTarget::ALL_ENEMIES, 40, 35, 5));
    m_skills.append(new Skill("暗影束缚", SkillType::STUN, SkillTarget::SINGLE_ENEMY, 35, 0, 5));
    m_skills.append(new Skill("神圣制裁", SkillType::AOE, SkillTarget::ALL_ENEMIES, 60, 80, 5));
}

void MainWindow::initSkills()
{
    createSkillTemplates();
    m_player->clearSkills();
    m_player->learnSkill(m_skills[0]);
    m_player->learnSkill(m_skills[1]);
}

void MainWindow::updatePlayerStats()
{
    if (!m_player) return;

    m_playerNameLabel->setText(m_player->name());
    m_levelLabel->setText(QString("等级: %1").arg(m_player->level()));

    int hpPercent = (m_player->hp() * 100) / m_player->maxHp();
    m_hpBar->setValue(hpPercent);
    m_hpLabel->setText(QString("%1/%2").arg(m_player->hp()).arg(m_player->maxHp()));

    int manaPercent = (m_player->mana() * 100) / m_player->maxMana();
    m_manaBar->setValue(manaPercent);
    m_manaLabel->setText(QString("魔力: %1/%2").arg(m_player->mana()).arg(m_player->maxMana()));

    m_attackLabel->setText(QString("攻击: %1").arg(m_player->totalAttack()));
    m_defenseLabel->setText(QString("防御: %1").arg(m_player->totalDefense()));
    m_goldLabel->setText(QString("金币: %1").arg(m_player->gold()));
    m_expLabel->setText(QString("经验: %1").arg(m_player->experience()));
}

void MainWindow::updateDungeonMap()
{
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            Room room = m_dungeon->getRoom(x, y);
            bool isPlayerPos = (x == m_playerX && y == m_playerY);
            bool hasFog = !m_explored[y][x];
            QPixmap pixmap = createRoomPixmap(room.type, isPlayerPos, hasFog);
            m_mapCells[y][x]->setPixmap(pixmap);
        }
    }
}

void MainWindow::updateExplored()
{
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int nx = m_playerX + dx;
            int ny = m_playerY + dy;
            if (nx >= 0 && nx < 5 && ny >= 0 && ny < 5) {
                m_explored[ny][nx] = true;
            }
        }
    }
    updateDungeonMap();
}

QPixmap MainWindow::createRoomPixmap(RoomType type, bool isPlayerPos, bool hasFog)
{
    int size = 65;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);

    if (hasFog) {
        QPixmap fogPixmap("images/fog.png");
        if (!fogPixmap.isNull()) {
            painter.drawPixmap(pixmap.rect(), fogPixmap.scaled(size, size));
        } else {
            painter.fillRect(pixmap.rect(), QColor(0, 0, 0, 180));
            painter.setPen(QColor(100, 100, 100));
            painter.drawText(pixmap.rect(), Qt::AlignCenter, "?");
        }
        return pixmap;
    }

    if (isPlayerPos) {
        QPixmap playerPixmap("images/player.png");
        if (!playerPixmap.isNull()) {
            painter.drawPixmap(pixmap.rect(), playerPixmap.scaled(size, size));
        } else {
            painter.setBrush(QColor(0, 255, 0));
            painter.drawEllipse(size/2 - 9, size/2 - 9, 18, 18);
        }
        return pixmap;
    }

    switch (type) {
    case RoomType::ENTRANCE:
        painter.fillRect(pixmap.rect(), QColor(30, 100, 30, 100));
        painter.setPen(QColor(0, 255, 0));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, "入口");
        break;
    case RoomType::BATTLE:
    case RoomType::ELITE:
    {
        QPixmap battlePixmap("images/battle.png");
        if (!battlePixmap.isNull()) {
            painter.drawPixmap(pixmap.rect(), battlePixmap.scaled(size, size));
        } else {
            painter.fillRect(pixmap.rect(), QColor(100, 30, 30, 100));
            painter.setPen(QColor(255, 0, 0));
            painter.drawText(pixmap.rect(), Qt::AlignCenter, "⚔");
        }
        break;
    }
    case RoomType::TREASURE:
    {
        QPixmap treasurePixmap("images/treasure.png");
        if (!treasurePixmap.isNull()) {
            painter.drawPixmap(pixmap.rect(), treasurePixmap.scaled(size, size));
        } else {
            painter.fillRect(pixmap.rect(), QColor(100, 100, 30, 100));
            painter.setPen(QColor(255, 255, 0));
            painter.drawText(pixmap.rect(), Qt::AlignCenter, "宝箱");
        }
        break;
    }
    case RoomType::BOSS:
    {
        QPixmap bossPixmap("images/boss.png");
        if (!bossPixmap.isNull()) {
            painter.drawPixmap(pixmap.rect(), bossPixmap.scaled(size, size));
        } else {
            painter.fillRect(pixmap.rect(), QColor(80, 30, 100, 100));
            painter.setPen(QColor(255, 0, 255));
            painter.drawText(pixmap.rect(), Qt::AlignCenter, "BOSS");
        }
        break;
    }
    default:
        painter.fillRect(pixmap.rect(), QColor(40, 40, 60));
        break;
    }

    return pixmap;
}

void MainWindow::addLogMessage(const QString& message)
{
    m_logText->append(message);
    m_logText->moveCursor(QTextCursor::End);
}

void MainWindow::on_startGameButton_clicked()
{
    if (m_saveManager->getTotalSaveCount() >= SaveManager::MAX_SAVES) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("存档已满");
        msgBox.setText("当前存档已满（最多5个），请先删除存档！");
        msgBox.exec();
        return;
    }

    QString saveName;
    bool ok;
    while (true) {
        saveName = QInputDialog::getText(this, "存档命名", "请为该存档命名（最多15个字符）：",
                                         QLineEdit::Normal, "", &ok);
        if (!ok) return;
        if (saveName.isEmpty()) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("错误");
            msgBox.setText("存档名称不能为空！");
            msgBox.exec();
            continue;
        }
        if (saveName.length() > 15) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("错误");
            msgBox.setText("存档名称不能超过15个字符！");
            msgBox.exec();
            continue;
        }
        if (m_saveManager->isSaveNameExists(saveName)) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("错误");
            msgBox.setText("存档名称已存在！");
            msgBox.exec();
            continue;
        }
        break;
    }

    QString playerName;
    while (true) {
        playerName = QInputDialog::getText(this, "角色命名", "请为角色命名（最多15个字符）：",
                                           QLineEdit::Normal, "", &ok);
        if (!ok) return;
        if (playerName.isEmpty()) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("错误");
            msgBox.setText("角色名称不能为空！");
            msgBox.exec();
            continue;
        }
        if (playerName.length() > 15) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("错误");
            msgBox.setText("角色名称不能超过15个字符！");
            msgBox.exec();
            continue;
        }
        break;
    }

    m_currentSaveName = saveName;
    initPlayer(playerName);
    initDungeon();
    initSkills();
    m_playerX = 0;
    m_playerY = 0;
    m_currentFloor = 1;
    m_dungeon->setPlayerPosition(0, 0);

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            m_explored[y][x] = false;
        }
    }
    m_explored[0][0] = true;

    m_floorLabel->setText(QString("第 %1 层 / 共 5 层").arg(m_currentFloor));
    updatePlayerStats();
    updateDungeonMap();
    updateExplored();

    addLogMessage("欢迎来到地牢！");
    addLogMessage("使用方向键移动，自动触发房间事件");
    addLogMessage("击败BOSS进入下一层！");
    addLogMessage("共有5层地牢等你挑战！");

    showGameScreen();
    autoSave();
}

void MainWindow::on_loadGameButton_clicked()
{
    QVector<SaveInfo> saves = m_saveManager->getAllSaves();
    if (saves.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("没有存档，请先创建新存档！");
        msgBox.exec();
        return;
    }

    QStringList saveNames;
    for (const SaveInfo& info : saves) {
        saveNames.append(QString("%1. %2 - 第%3层").arg(info.index + 1).arg(info.name).arg(info.floor));
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "选择存档", "请选择要读取的存档：", saveNames, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;

    int selectedIndex = saveNames.indexOf(selected);
    if (selectedIndex < 0) return;

    int slot = saves[selectedIndex].index;
    QString saveName = saves[selectedIndex].name;

    Character* loadedPlayer = nullptr;
    int loadedFloor = 1;
    QVector<Item*> loadedItems;
    QVector<Equipment*> loadedEquipments;
    QVector<Equipment*> loadedEquipped;
    QVector<Skill*> loadedSkills;
    int loadedPlayerX = 0, loadedPlayerY = 0;

    if (!m_saveManager->loadGame(slot, loadedPlayer, loadedFloor, saveName, loadedItems, loadedEquipments, loadedSkills, loadedPlayerX, loadedPlayerY)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("读取存档失败！");
        msgBox.exec();
        return;
    }

    if (!loadedPlayer) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("错误");
        msgBox.setText("存档数据损坏！");
        msgBox.exec();
        return;
    }

    m_player = loadedPlayer;
    m_currentFloor = loadedFloor;
    m_currentSaveName = saveName;
    m_currentSaveSlot = slot;

    delete m_dungeon;
    m_dungeon = new Dungeon(5);
    m_dungeon->generateFloor(m_currentFloor);

    qDeleteAll(m_inventory);
    m_inventory.clear();
    for (Item* item : loadedItems) {
        m_inventory.append(item);
    }

    qDeleteAll(m_equipmentStorage);
    m_equipmentStorage.clear();
    for (Equipment* eq : loadedEquipments) {
        m_equipmentStorage.append(eq);
    }

    for (Equipment* eq : loadedEquipped) {
        m_player->equipItem(eq);
    }

    createSkillTemplates();
    m_player->clearSkills();
    for (int i = 0; i < m_player->level() && i < m_skills.size(); ++i) {
        if (i == 0 || i == 1 || i <= m_player->level()) {
            m_player->learnSkill(m_skills[i]);
        }
    }

    m_playerX = 0;
    m_playerY = 0;
    m_dungeon->setPlayerPosition(0, 0);

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            m_explored[y][x] = false;
        }
    }
    m_explored[0][0] = true;

    m_floorLabel->setText(QString("第 %1 层 / 共 5 层").arg(m_currentFloor));
    updatePlayerStats();
    updateDungeonMap();
    updateExplored();

    addLogMessage(QString("欢迎回来，%1！").arg(m_player->name()));
    addLogMessage(QString("你在第 %1 层").arg(m_currentFloor));

    showGameScreen();
}

void MainWindow::on_deleteSaveButton_clicked()
{
    QVector<SaveInfo> saves = m_saveManager->getAllSaves();
    if (saves.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("没有存档可供删除！");
        msgBox.exec();
        return;
    }

    QStringList saveNames;
    for (const SaveInfo& info : saves) {
        saveNames.append(QString("%1. %2 - 第%3层").arg(info.index + 1).arg(info.name).arg(info.floor));
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "删除存档", "请选择要删除的存档：", saveNames, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;

    int selectedIndex = saveNames.indexOf(selected);
    if (selectedIndex < 0) return;

    QMessageBox confirmMsg(this);
    confirmMsg.setIcon(QMessageBox::NoIcon);
    confirmMsg.setWindowTitle("确认删除");
    confirmMsg.setText(QString("确定要删除存档 \"%1\" 吗？此操作不可恢复。").arg(saves[selectedIndex].name));
    confirmMsg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmMsg.setDefaultButton(QMessageBox::No);

    if (confirmMsg.exec() == QMessageBox::Yes) {
        if (m_saveManager->deleteSave(saves[selectedIndex].index)) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::NoIcon);
            msgBox.setWindowTitle("删除成功");
            msgBox.setText("存档已删除！");
            msgBox.exec();
            m_loadGameButton->setEnabled(m_saveManager->hasSaveFile());
            m_deleteSaveButton->setEnabled(m_saveManager->hasSaveFile());
        } else {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("错误");
            msgBox.setText("删除存档失败！");
            msgBox.exec();
        }
    }
}

void MainWindow::on_gameHelpButton_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("游戏说明");
    dialog.setFixedSize(500, 600);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(10, 10, 10, 10);

    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(
        "========== 暗狱回廊：逐暗旅人 ==========\n\n"

        "【游戏概述】\n"
        "  你将扮演一名勇者，探索5层随机生成的地牢，\n"
        "  击败各层BOSS，最终通关全部地牢！\n\n"

        "【主菜单功能】\n"
        "  - 开始新游戏: 创建新存档，命名存档和角色后开始\n"
        "  - 读取存档: 选择已有存档继续游戏\n"
        "  - 删除存档: 选择并删除不需要的存档\n"
        "  - 游戏说明: 查看本说明\n"
        "  - 退出游戏: 关闭程序\n"
        "  (存档名和角色名最多15个字符，不能为空，\n"
        "   存档名不可重复，最多5个存档)\n\n"

        "【地牢探索】\n"
        "  地牢每层为5x5的网格地图，使用方向按钮移动。\n"
        "  地图带有迷雾系统，只能看到角色周围9格范围，\n"
        "  已探索的区域不会再次被迷雾覆盖。\n"
        "  BOSS房始终显示在地图右下角(4,4)位置。\n"
        "  左上角(0,0)为入口(安全区域)。\n\n"

        "【房间类型】\n"
        "  - 入口(绿色): 安全区域，战斗失败后在此复活\n"
        "  - 战斗房(红色): 普通怪物，难度较低\n"
        "  - 精英房(红色): 精英怪物，更强但奖励更丰富\n"
        "  - 宝箱房(金色): 获取道具和装备，无需战斗\n"
        "  - BOSS房(紫色): 击败后可进入下一层\n\n"

        "【角色属性】\n"
        "  - 生命值(HP): 归零则战斗失败\n"
        "  - 魔力值(MP): 释放技能消耗，每回合恢复(5+等级)\n"
        "  - 攻击力: 影响普通攻击和技能伤害\n"
        "  - 防御力: 减少受到的伤害\n"
        "  - 速度: 决定战斗中的出手顺序(高者先手)\n"
        "  - 等级/经验: 击败怪物获得经验，满经验自动升级\n"
        "  - 金币: 用于商店购买道具和装备\n\n"

        "【升级系统】\n"
        "  升级所需经验 = 当前等级 x 110\n"
        "  每次升级提升: 生命+20 攻击+5 防御+3 速度+2 魔力+20\n"
        "  升级后生命和魔力回满，并可能解锁新技能！\n\n"

        "【战斗系统】\n"
        "  战斗为回合制，速度高的一方先出手。\n"
        "  四种行动选项:\n"
        "  - 攻击: 普通攻击，造成(攻击力-防御力)伤害\n"
        "  - 技能: 消耗魔力释放技能(详见技能列表)\n"
        "  - 道具: 使用背包中的药水恢复生命\n"
        "  - 逃跑: 50%概率成功，失败则敌人攻击\n\n"
        "  战斗胜利奖励: 经验、金币、随机道具/装备\n"
        "  战斗失败: 在入口复活，生命回满\n"
        "  BOSS战失败: 游戏重新开始\n\n"

        "【技能列表】\n"
        "  Lv1  重击(10MP): 单体攻击，伤害15\n"
        "  Lv1  治愈术(15MP): 恢复自身30点生命\n"
        "  Lv2  能量冲击(15MP): 单体攻击，伤害25\n"
        "  Lv3  火焰斩(25MP): 范围攻击，伤害20\n"
        "  Lv4  圣光术(30MP): 恢复自身50点生命\n"
        "  Lv5  雷霆一击(25MP): 单体攻击，伤害40\n"
        "  Lv6  毒刺(20MP): 持续伤害，每回合10伤害\n"
        "  Lv7  护体石肤(20MP): 增益，防御+15\n"
        "  Lv8  地震波(40MP): 范围攻击，伤害35\n"
        "  Lv9  暗影束缚(35MP): 眩晕敌人1回合\n"
        "  Lv10 神圣制裁(60MP): 范围攻击，伤害80\n\n"

        "【道具系统】\n"
        "  消耗品(药水): 战斗或探索时使用，恢复生命值\n"
        "  - 小型药水(+30) / 中型药水(+60)\n"
        "  - 大型药水(+100) / 超级药水(+150)\n"
        "  - 万能药(+80) / 神圣药水(+200)\n"
        "  - 凤凰之羽(+100)\n"
        "  点击左侧\"使用道具\"按钮使用。\n\n"

        "【装备系统】\n"
        "  三类装备: 武器(加攻击) / 防具(加防御) / 饰品(加生命)\n"
        "  五种品质: 普通(白) > 优秀(绿) > 稀有(蓝) > 史诗(紫) > 传说(橙)\n"
        "  每类装备只能穿戴一件，新的会替换旧的。\n"
        "  点击\"穿戴装备\"选择装备，\"卸下装备\"放回背包。\n"
        "  装备来源: 宝箱、怪物掉落、商店购买\n\n"

        "【商店系统】\n"
        "  点击左侧\"进入商店\"按钮打开商店。\n"
        "  商店出售当前楼层的道具和装备，可用金币购买。\n"
        "  可花费金币刷新商品(首次50金币，每次+30)。\n"
        "  每层商店商品不同，进入新层自动刷新。\n"
        "  关闭商店点击\"关闭\"按钮或右上角x均可。\n\n"

        "【存档系统】\n"
        "  - 自动保存: 进入每层开始房间时自动保存\n"
        "  - 新建存档时自动保存到第1层入口\n"
        "  - 读取存档后角色回到该层入口位置\n"
        "  - 通关后该存档自动删除\n"
        "  - 最多5个存档，可在主菜单删除\n\n"

        "【通关条件】\n"
        "  击败第5层BOSS即可通关，存档自动删除。\n\n"

        "祝您游戏愉快！");
    layout->addWidget(textEdit);

    QPushButton* closeButton = new QPushButton("关闭", &dialog);
    closeButton->setFixedWidth(100);
    connect(closeButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeButton);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    dialog.exec();
}

void MainWindow::on_exitButton_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowTitle("退出游戏");
    msgBox.setText("是否退出游戏？");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() == QMessageBox::Yes) {
        QApplication::quit();
    }
}

void MainWindow::on_moveUpButton_clicked()
{
    if (m_playerY > 0) {
        m_playerY--;
        m_dungeon->setPlayerPosition(m_playerX, m_playerY);
        handleRoomEvent(m_dungeon->getRoom(m_playerX, m_playerY));
        updateExplored();
    }
}

void MainWindow::on_moveDownButton_clicked()
{
    if (m_playerY < 4) {
        m_playerY++;
        m_dungeon->setPlayerPosition(m_playerX, m_playerY);
        handleRoomEvent(m_dungeon->getRoom(m_playerX, m_playerY));
        updateExplored();
    }
}

void MainWindow::on_moveLeftButton_clicked()
{
    if (m_playerX > 0) {
        m_playerX--;
        m_dungeon->setPlayerPosition(m_playerX, m_playerY);
        handleRoomEvent(m_dungeon->getRoom(m_playerX, m_playerY));
        updateExplored();
    }
}

void MainWindow::on_moveRightButton_clicked()
{
    if (m_playerX < 4) {
        m_playerX++;
        m_dungeon->setPlayerPosition(m_playerX, m_playerY);
        handleRoomEvent(m_dungeon->getRoom(m_playerX, m_playerY));
        updateExplored();
    }
}

void MainWindow::handleRoomEvent(Room room)
{
    if (room.type == RoomType::ENTRANCE) {
        addLogMessage("你回到了入口");
        return;
    }

    if (room.type == RoomType::TREASURE) {
        if (!room.cleared) {
            addLogMessage("发现宝箱！");
            for (Item* item : room.items) {
                m_inventory.append(item);
                addLogMessage(QString("获得了 %1").arg(item->name()));
            }
            for (Equipment* eq : room.equipments) {
                m_equipmentStorage.append(eq);
                addLogMessage(QString("获得了装备: %1 (+攻击:%2 +防御:%3 +生命:%4)")
                              .arg(eq->name()).arg(eq->attackBonus())
                              .arg(eq->defenseBonus()).arg(eq->hpBonus()));
            }
            m_dungeon->setRoomCleared(room.x, room.y, true);
            addLogMessage("宝箱已打开");
        } else {
            addLogMessage("宝箱已经被打开了");
        }
        return;
    }

    if (room.type == RoomType::BATTLE || room.type == RoomType::ELITE || room.type == RoomType::BOSS) {
        if (!room.cleared) {
            m_battleWindow = new BattleWindow(this);
            m_battleWindow->startBattle(m_player, room.monster);
            connect(m_battleWindow, SIGNAL(battleResult(bool, int, int, QVector<Item*>, QVector<Equipment*>)),
                    this, SLOT(on_battleResult(bool, int, int, QVector<Item*>, QVector<Equipment*>)));
            connect(m_battleWindow, SIGNAL(closed()), this, SLOT(on_battleClosed()));
            connect(m_battleWindow, SIGNAL(restartGame(bool)), this, SLOT(on_restartGame(bool)));
            m_battleWindow->show();
        } else {
            addLogMessage("这里已经清理过了");
        }
    }
}

void MainWindow::on_battleResult(bool victory, int exp, int gold, QVector<Item*> items, QVector<Equipment*> equipments)
{
    if (victory) {
        m_player->setExperience(m_player->experience() + exp);
        m_player->setGold(m_player->gold() + gold);

        addLogMessage(QString("战斗胜利！获得 %1 经验，%2 金币").arg(exp).arg(gold));

        for (Item* item : items) {
            m_inventory.append(item);
            addLogMessage(QString("获得道具: %1").arg(item->name()));
        }
        for (Equipment* eq : equipments) {
            m_equipmentStorage.append(eq);
            addLogMessage(QString("获得装备: %1").arg(eq->name()));
        }

        checkLevelUp();
        updatePlayerStats();

        if (m_dungeon->currentRoom().type == RoomType::BOSS) {
            m_dungeon->setRoomCleared(m_playerX, m_playerY, true);
            m_nextFloorButton->setEnabled(true);
            addLogMessage("BOSS击败！可以进入下一层了");
        } else {
            m_dungeon->setRoomCleared(m_playerX, m_playerY, true);
        }
        updateDungeonMap();
    }
}

void MainWindow::on_battleClosed()
{
    delete m_battleWindow;
    m_battleWindow = nullptr;
}

void MainWindow::on_restartGame(bool restartFromBeginning)
{
    if (restartFromBeginning) {
        showMainMenu();
    } else {
        m_player->setHp(m_player->maxHp());
        m_player->setMana(m_player->maxMana());
        m_playerX = 0;
        m_playerY = 0;
        m_dungeon->setPlayerPosition(0, 0);
        updatePlayerStats();
        updateDungeonMap();
        addLogMessage("你在入口复活了");
    }
}

void MainWindow::checkLevelUp()
{
    while (m_player->experience() >= m_player->level() * 110) {
        m_player->levelUp();
        addLogMessage(QString("升级了！当前等级: %1").arg(m_player->level()));

        if (m_player->level() <= m_skills.size()) {
            m_player->learnSkill(m_skills[m_player->level() - 1]);
            addLogMessage(QString("解锁新技能: %1").arg(m_skills[m_player->level() - 1]->name()));
        }

        updatePlayerStats();
    }
}

void MainWindow::on_nextFloorButton_clicked()
{
    if (m_dungeon->hasNextFloor()) {
        Room bossRoom = m_dungeon->getRoom(4, 4);
        if (!bossRoom.cleared) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::NoIcon);
            msgBox.setWindowTitle("提示");
            msgBox.setText("必须先击败当前层的BOSS才能进入下一层！");
            msgBox.exec();
            return;
        }

        m_currentFloor++;
        m_dungeon->nextFloor();
        m_playerX = 0;
        m_playerY = 0;
        m_dungeon->setPlayerPosition(0, 0);

        for (int y = 0; y < 5; ++y) {
            for (int x = 0; x < 5; ++x) {
                m_explored[y][x] = false;
            }
        }
        m_explored[0][0] = true;

        m_floorLabel->setText(QString("第 %1 层 / 共 5 层").arg(m_currentFloor));
        m_nextFloorButton->setEnabled(false);

        updatePlayerStats();
        updateDungeonMap();
        updateExplored();

        addLogMessage(QString("进入第 %1 层！").arg(m_currentFloor));

        generateShopItemsForFloor();
        generateShopEquipmentsForFloor();

        autoSave();
    } else {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("恭喜通关！");
        msgBox.setText("你已经通关了所有地牢！");
        msgBox.exec();

        m_saveManager->deleteSave(m_currentSaveSlot);
        showMainMenu();
    }
}

void MainWindow::on_backToMenuButton_clicked()
{
    showMainMenu();
}

void MainWindow::on_useItemButton_clicked()
{
    showItemDialog();
}

void MainWindow::on_equipItemButton_clicked()
{
    showEquipmentDialog();
}

void MainWindow::on_unequipItemButton_clicked()
{
    Equipment* weapon = m_player->getEquipment(EquipmentType::WEAPON);
    Equipment* armor = m_player->getEquipment(EquipmentType::ARMOR);
    Equipment* accessory = m_player->getEquipment(EquipmentType::ACCESSORY);

    QStringList items;
    if (weapon) items.append(QString("武器: %1").arg(weapon->name()));
    if (armor) items.append(QString("防具: %1").arg(armor->name()));
    if (accessory) items.append(QString("饰品: %1").arg(accessory->name()));

    if (items.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("提示");
        msgBox.setText("没有装备可卸下！");
        msgBox.exec();
        return;
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "卸下装备", "选择要卸下的装备：", items, 0, false, &ok);
    if (!ok) return;

    if (selected.startsWith("武器")) {
        m_equipmentStorage.append(weapon);
        m_player->unequipItem(EquipmentType::WEAPON);
        addLogMessage(QString("卸下了 %1").arg(weapon->name()));
    } else if (selected.startsWith("防具")) {
        m_equipmentStorage.append(armor);
        m_player->unequipItem(EquipmentType::ARMOR);
        addLogMessage(QString("卸下了 %1").arg(armor->name()));
    } else if (selected.startsWith("饰品")) {
        m_equipmentStorage.append(accessory);
        m_player->unequipItem(EquipmentType::ACCESSORY);
        addLogMessage(QString("卸下了 %1").arg(accessory->name()));
    }

    updatePlayerStats();
}

void MainWindow::on_showSkillsButton_clicked()
{
    showSkillDialog();
}

void MainWindow::showItemDialog()
{
    if (m_inventory.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("提示");
        msgBox.setText("背包是空的！");
        msgBox.exec();
        return;
    }

    QStringList itemNames;
    for (Item* item : m_inventory) {
        itemNames.append(item->name());
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "使用道具", "选择要使用的道具：", itemNames, 0, false, &ok);
    if (!ok) return;

    int index = itemNames.indexOf(selected);
    if (index >= 0) {
        Item* item = m_inventory[index];
        switch (item->consumableType()) {
        case ConsumableType::HP_POTION:
            m_player->heal(item->value());
            addLogMessage(QString("使用了 %1，恢复 %2 点生命").arg(item->name()).arg(item->value()));
            break;
        case ConsumableType::MP_POTION:
            m_player->setMana(m_player->mana() + item->value());
            addLogMessage(QString("使用了 %1，恢复 %2 点魔力").arg(item->name()).arg(item->value()));
            break;
        default:
            addLogMessage(QString("使用了 %1").arg(item->name()));
        }
        m_inventory.removeAt(index);
        delete item;
        updatePlayerStats();
    }
}

void MainWindow::showEquipmentDialog()
{
    if (m_equipmentStorage.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("提示");
        msgBox.setText("没有可穿戴的装备！");
        msgBox.exec();
        return;
    }

    QStringList equipNames;
    for (Equipment* eq : m_equipmentStorage) {
        QString typeStr;
        switch (eq->type()) {
        case EquipmentType::WEAPON: typeStr = "武器"; break;
        case EquipmentType::ARMOR: typeStr = "防具"; break;
        case EquipmentType::ACCESSORY: typeStr = "饰品"; break;
        }
        equipNames.append(QString("%1: %2 (+攻:%3 +防:%4 +血:%5)").arg(typeStr).arg(eq->name())
                          .arg(eq->attackBonus()).arg(eq->defenseBonus()).arg(eq->hpBonus()));
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "穿戴装备", "选择要穿戴的装备：", equipNames, 0, false, &ok);
    if (!ok) return;

    int index = equipNames.indexOf(selected);
    if (index >= 0) {
        Equipment* eq = m_equipmentStorage[index];

        Equipment* oldEq = m_player->getEquipment(eq->type());
        m_player->equipItem(eq);

        m_equipmentStorage.removeAt(index);

        if (oldEq) {
            m_equipmentStorage.append(oldEq);
            addLogMessage(QString("卸下了 %1，装备了 %2").arg(oldEq->name()).arg(eq->name()));
        } else {
            addLogMessage(QString("装备了 %1").arg(eq->name()));
        }

        updatePlayerStats();
    }
}

void MainWindow::showSkillDialog()
{
    QString skillText = "技能列表:\n\n";
    for (Skill* skill : m_player->getSkills()) {
        skillText += QString("%1 (%2MP): ").arg(skill->name()).arg(skill->manaCost());
        switch (skill->target()) {
        case SkillTarget::SINGLE_ENEMY: skillText += "单体攻击，伤害"; break;
        case SkillTarget::ALL_ENEMIES: skillText += "范围攻击，伤害"; break;
        case SkillTarget::SELF: skillText += "恢复/增益"; break;
        case SkillTarget::SINGLE_ALLY: skillText += "单体队友"; break;
        case SkillTarget::ALL_ALLIES: skillText += "所有队友"; break;
        }
        skillText += QString("%1\n").arg(skill->damage());
    }

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowTitle("技能");
    msgBox.setText(skillText);
    msgBox.exec();
}

void MainWindow::on_openShopButton_clicked()
{
    m_shopWindow = new Shop(this);
    m_shopWindow->setGold(m_player->gold());
    m_shopWindow->setShopLevel(m_currentFloor);
    m_shopWindow->setRefreshCost(m_shopRefreshCost);
    m_shopWindow->generateShopItems();
    m_shopWindow->generateShopEquipments();
    m_shopWindow->refreshItemList();
    m_shopWindow->refreshEquipmentList();
    connect(m_shopWindow, SIGNAL(closed()), this, SLOT(on_shopClosed()));
    connect(m_shopWindow, SIGNAL(goldChanged(int)), this, SLOT(on_goldChanged(int)));
    connect(m_shopWindow, SIGNAL(itemPurchased(Item*)), this, SLOT(on_itemPurchasedFromShop(Item*)));
    connect(m_shopWindow, SIGNAL(equipmentPurchased(Equipment*)), this, SLOT(on_equipmentPurchasedFromShop(Equipment*)));
    connect(m_shopWindow, SIGNAL(refreshRequested()), this, SLOT(on_shopRefreshRequested()));
    m_shopWindow->show();
}

void MainWindow::on_shopClosed()
{
    delete m_shopWindow;
    m_shopWindow = nullptr;
}

void MainWindow::on_goldChanged(int newGold)
{
    m_player->setGold(newGold);
    updatePlayerStats();
}

void MainWindow::on_itemPurchasedFromShop(Item* item)
{
    m_inventory.append(item);
    addLogMessage(QString("购买了 %1").arg(item->name()));
}

void MainWindow::on_equipmentPurchasedFromShop(Equipment* equipment)
{
    m_equipmentStorage.append(equipment);
    addLogMessage(QString("购买了装备: %1").arg(equipment->name()));
}

void MainWindow::on_shopRefreshRequested()
{
    m_shopRefreshCost += 30;
    generateShopItemsForFloor();
    generateShopEquipmentsForFloor();

    if (m_shopWindow) {
        m_shopWindow->refreshShopItems(m_shopItems);
        m_shopWindow->refreshShopEquipments(m_shopEquipments);
        m_shopWindow->setRefreshCost(m_shopRefreshCost);
    }

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowTitle("刷新成功");
    msgBox.setText(QString("商店已刷新！下次刷新费用: %1 金币").arg(m_shopRefreshCost));
    msgBox.exec();
}

void MainWindow::generateShopItemsForFloor()
{
    qDeleteAll(m_shopItems);
    m_shopItems.clear();

    m_shopItems.append(new Item("小型药水", ItemType::CONSUMABLE, 30, ConsumableType::HP_POTION));
    m_shopItems.append(new Item("中型药水", ItemType::CONSUMABLE, 60, ConsumableType::HP_POTION));
    m_shopItems.append(new Item("大型药水", ItemType::CONSUMABLE, 100, ConsumableType::HP_POTION));
}

void MainWindow::generateShopEquipmentsForFloor()
{
    qDeleteAll(m_shopEquipments);
    m_shopEquipments.clear();

    int baseStat = m_currentFloor * 5;

    m_shopEquipments.append(new Equipment("铁剑", EquipmentType::WEAPON, baseStat, 0, 0));
    m_shopEquipments.append(new Equipment("钢甲", EquipmentType::ARMOR, 0, baseStat, 0));
    m_shopEquipments.append(new Equipment("护符", EquipmentType::ACCESSORY, 0, 0, baseStat * 2));
}

void MainWindow::autoSave()
{
    if (m_currentSaveSlot == -1) {
        m_currentSaveSlot = m_saveManager->getFreeSlot();
    }

    m_saveManager->saveGame(m_currentSaveSlot, m_currentSaveName, m_player, m_currentFloor,
                           m_inventory, m_equipmentStorage, m_player->getSkills());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowTitle("退出游戏");
    msgBox.setText("是否退出游戏？");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}
