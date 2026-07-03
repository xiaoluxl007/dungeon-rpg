#include "shop.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QDialogButtonBox>

Shop::Shop(QWidget *parent)
    : QDialog(parent), m_playerGold(0), m_shopLevel(1), m_refreshCost(50)
{
    setWindowTitle("地牢商店");
    setFixedSize(500, 400);
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel* titleLabel = new QLabel("<h2>欢迎来到商店</h2>", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 金币显示
    m_goldLabel = new QLabel(QString("您当前的金币: %1").arg(m_playerGold), this);
    m_goldLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_goldLabel);

    // 商品列表区域
    QHBoxLayout* listsLayout = new QHBoxLayout();

    // 道具列表
    QVBoxLayout* itemLayout = new QVBoxLayout();
    QLabel* itemLabel = new QLabel("<b>消耗品</b>", this);
    itemLayout->addWidget(itemLabel);

    m_itemList = new QListWidget(this);
    m_itemList->setMaximumHeight(200);
    itemLayout->addWidget(m_itemList);

    m_buyItemButton = new QPushButton("购买道具", this);
    connect(m_buyItemButton, SIGNAL(clicked()), this, SLOT(onBuyItem()));
    itemLayout->addWidget(m_buyItemButton);

    listsLayout->addLayout(itemLayout);

    // 装备列表
    QVBoxLayout* equipLayout = new QVBoxLayout();
    QLabel* equipLabel = new QLabel("<b>装备</b>", this);
    equipLayout->addWidget(equipLabel);

    m_equipmentList = new QListWidget(this);
    m_equipmentList->setMaximumHeight(200);
    equipLayout->addWidget(m_equipmentList);

    m_buyEquipmentButton = new QPushButton("购买装备", this);
    connect(m_buyEquipmentButton, SIGNAL(clicked()), this, SLOT(onBuyEquipment()));
    equipLayout->addWidget(m_buyEquipmentButton);

    listsLayout->addLayout(equipLayout);

    mainLayout->addLayout(listsLayout);

    // 刷新按钮
    m_refreshButton = new QPushButton(QString("刷新商品 (消耗%1金币)").arg(m_refreshCost), this);
    connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(onRefreshShop()));
    mainLayout->addWidget(m_refreshButton);

    // 关闭按钮
    m_closeButton = new QPushButton("关闭", this);
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
    mainLayout->addWidget(m_closeButton);
}

Shop::~Shop()
{
}

void Shop::setItems(const QVector<Item*>& items)
{
    // 这个函数可以用于设置玩家背包，但我们主要是卖东西给玩家
    Q_UNUSED(items);
}

void Shop::setEquipments(const QVector<Equipment*>& equipments)
{
    // 这个函数可以用于设置玩家已有装备
    Q_UNUSED(equipments);
}

void Shop::generateShopItems()
{
    qDeleteAll(m_shopItems);
    m_shopItems.clear();

    // 根据商店等级生成道具
    QStringList itemNames;
    if (m_shopLevel <= 2) {
        itemNames = {"小型生命药水", "中型生命药水", "解毒剂"};
    } else if (m_shopLevel <= 4) {
        itemNames = {"中型生命药水", "大型生命药水", "抗毒药水", "力量药水"};
    } else {
        itemNames = {"大型生命药水", "超级生命药水", "万能药", "神圣药水", "凤凰之羽"};
    }

    int numItems = 3 + QRandomGenerator::global()->bounded(3);
    for (int i = 0; i < numItems && i < itemNames.size(); ++i) {
        Item* item = nullptr;
        if (itemNames[i].contains("小型")) {
            item = new Item("小型生命药水", ItemType::CONSUMABLE, 30, ConsumableType::HP_POTION);
        } else if (itemNames[i].contains("中型")) {
            item = new Item("中型生命药水", ItemType::CONSUMABLE, 60, ConsumableType::HP_POTION);
        } else if (itemNames[i].contains("大型")) {
            item = new Item("大型生命药水", ItemType::CONSUMABLE, 100, ConsumableType::HP_POTION);
        } else if (itemNames[i].contains("超级")) {
            item = new Item("超级生命药水", ItemType::CONSUMABLE, 150, ConsumableType::HP_POTION);
        } else if (itemNames[i].contains("万能药")) {
            item = new Item("万能药", ItemType::CONSUMABLE, 80, ConsumableType::HP_POTION);
        } else if (itemNames[i].contains("神圣")) {
            item = new Item("神圣药水", ItemType::CONSUMABLE, 200, ConsumableType::HP_POTION);
        } else if (itemNames[i].contains("解毒")) {
            item = new Item("解毒剂", ItemType::CONSUMABLE, 0, ConsumableType::BUFF_ITEM);
        } else if (itemNames[i].contains("力量")) {
            item = new Item("力量药水", ItemType::CONSUMABLE, 0, ConsumableType::BUFF_ITEM);
        } else {
            item = new Item("凤凰之羽", ItemType::CONSUMABLE, 100, ConsumableType::HP_POTION);
        }

        if (item) {
            m_shopItems.append(item);
        }
    }
}

void Shop::generateShopEquipments()
{
    qDeleteAll(m_shopEquipments);
    m_shopEquipments.clear();

    // 根据商店等级生成装备
    int numEquips = 2 + QRandomGenerator::global()->bounded(3);

    for (int i = 0; i < numEquips; ++i) {
        int type = QRandomGenerator::global()->bounded(3);
        int roll = QRandomGenerator::global()->bounded(100);

        // 商店装备品质概率
        EquipmentQuality quality;
        if (roll < 40) {
            quality = EquipmentQuality::COMMON;
        } else if (roll < 70) {
            quality = EquipmentQuality::UNCOMMON;
        } else if (roll < 90) {
            quality = EquipmentQuality::RARE;
        } else {
            quality = EquipmentQuality::EPIC;
        }

        QString name;
        int attackBonus = 0;
        int defenseBonus = 0;
        int hpBonus = 0;

        if (type == 0) {
            // 武器
            QStringList names;
            switch (quality) {
            case EquipmentQuality::COMMON:
                names = {"铁剑", "短剑", "木棒"};
                break;
            case EquipmentQuality::UNCOMMON:
                names = {"钢剑", "长剑", "弯刀"};
                break;
            case EquipmentQuality::RARE:
                names = {"魔法剑", "光之剑", "寒冰剑"};
                break;
            case EquipmentQuality::EPIC:
                names = {"龙之剑", "烈焰剑", "暗影刃"};
                break;
            default:
                names = {"铁剑"};
            }
            name = names[QRandomGenerator::global()->bounded(names.size())];
            int qualityValue = static_cast<int>(quality);
            attackBonus = static_cast<int>((5 + m_shopLevel * 2) * (1.0 + qualityValue * 0.3));
        } else if (type == 1) {
            // 防具
            QStringList names;
            switch (quality) {
            case EquipmentQuality::COMMON:
                names = {"皮甲", "布衣", "麻衣"};
                break;
            case EquipmentQuality::UNCOMMON:
                names = {"锁甲", "强化皮甲", "皮靴"};
                break;
            case EquipmentQuality::RARE:
                names = {"板甲", "链甲", "鳞甲"};
                break;
            case EquipmentQuality::EPIC:
                names = {"龙鳞甲", "魔法甲", "圣甲"};
                break;
            default:
                names = {"皮甲"};
            }
            name = names[QRandomGenerator::global()->bounded(names.size())];
            int qualityValue2 = static_cast<int>(quality);
            defenseBonus = static_cast<int>((5 + m_shopLevel * 2) * (1.0 + qualityValue2 * 0.3));
        } else {
            // 饰品
            QStringList names;
            switch (quality) {
            case EquipmentQuality::COMMON:
                names = {"力量戒指", "防御戒指", "生命戒指"};
                break;
            case EquipmentQuality::UNCOMMON:
                names = {"敏捷戒指", "幸运戒指", "坚韧戒指"};
                break;
            case EquipmentQuality::RARE:
                names = {"神圣戒指", "暗影戒指", "元素戒指"};
                break;
            case EquipmentQuality::EPIC:
                names = {"龙之眼", "凤凰之羽", "恶魔之角"};
                break;
            default:
                names = {"力量戒指"};
            }
            name = names[QRandomGenerator::global()->bounded(names.size())];
            int qualityValue3 = static_cast<int>(quality);
            hpBonus = static_cast<int>((20 + m_shopLevel * 10) * (1.0 + qualityValue3 * 0.3));
        }

        Equipment* eq = new Equipment(name, static_cast<EquipmentType>(type), quality,
                                      attackBonus, defenseBonus, hpBonus);
        m_shopEquipments.append(eq);
    }
}

void Shop::updateGoldDisplay()
{
    m_goldLabel->setText(QString("您当前的金币: %1").arg(m_playerGold));
}

void Shop::refreshItemList()
{
    m_itemList->clear();
    for (Item* item : m_shopItems) {
        int price = item->value() * 2;
        QString display = QString("%1 - %2 金币").arg(item->name()).arg(price);
        QListWidgetItem* listItem = new QListWidgetItem(display, m_itemList);
        listItem->setData(Qt::UserRole, QVariant::fromValue<void*>(item));
    }
}

void Shop::refreshEquipmentList()
{
    m_equipmentList->clear();
    for (Equipment* eq : m_shopEquipments) {
        int price = eq->getPrice();
        QString bonusStr;
        if (eq->type() == EquipmentType::WEAPON) {
            bonusStr = QString("攻击+%1").arg(eq->attackBonus());
        } else if (eq->type() == EquipmentType::ARMOR) {
            bonusStr = QString("防御+%1").arg(eq->defenseBonus());
        } else {
            bonusStr = QString("生命+%1").arg(eq->hpBonus());
        }

        QString display = QString("[%2] %1 - %3 金币 (%4)")
                              .arg(eq->name())
                              .arg(eq->qualityString())
                              .arg(price)
                              .arg(bonusStr);

        QListWidgetItem* listItem = new QListWidgetItem(display, m_equipmentList);

        // 根据品质设置颜色
        QString color = eq->qualityColor();
        listItem->setForeground(QColor(color));
    }
}

void Shop::onBuyItem()
{
    int row = m_itemList->currentRow();
    if (row < 0 || row >= m_shopItems.size()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("提示");
        msgBox.setText("请先选择要购买的道具！");
        msgBox.exec();
        return;
    }

    Item* item = m_shopItems[row];
    int price = item->value() * 2;

    if (m_playerGold < price) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("金币不足");
        msgBox.setText(QString("购买 %1 需要 %2 金币，您只有 %3 金币！")
                            .arg(item->name()).arg(price).arg(m_playerGold));
        msgBox.exec();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowTitle("确认购买");
    msgBox.setText(QString("确定购买 %1 吗？\n将消耗 %2 金币").arg(item->name()).arg(price));
    msgBox.addButton("确定", QMessageBox::YesRole);
    msgBox.addButton("取消", QMessageBox::NoRole);

    if (msgBox.exec() == 0) {
        m_playerGold -= price;
        updateGoldDisplay();
        emit goldChanged(m_playerGold);
        emit itemPurchased(item);

        // 从商店移除已购买的道具
        m_shopItems.removeAt(row);
        delete item;
        refreshItemList();

        QMessageBox aboutBox(this);
        aboutBox.setIcon(QMessageBox::NoIcon);
        aboutBox.setWindowTitle("购买成功");
        aboutBox.setText(QString("成功购买 %1！").arg(item->name()));
        aboutBox.exec();
    }
}

void Shop::onBuyEquipment()
{
    int row = m_equipmentList->currentRow();
    if (row < 0 || row >= m_shopEquipments.size()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("提示");
        msgBox.setText("请先选择要购买的装备！");
        msgBox.exec();
        return;
    }

    Equipment* eq = m_shopEquipments[row];
    int price = eq->getPrice();

    if (m_playerGold < price) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("金币不足");
        msgBox.setText(QString("购买 %1 需要 %2 金币，您只有 %3 金币！")
                            .arg(eq->name()).arg(price).arg(m_playerGold));
        msgBox.exec();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowTitle("确认购买");
    msgBox.setText(QString("确定购买 [%1]%2 吗？\n将消耗 %3 金币\n属性: %4")
        .arg(eq->qualityString()).arg(eq->name()).arg(price)
        .arg(eq->type() == EquipmentType::WEAPON ? QString("攻击+%1").arg(eq->attackBonus()) :
             eq->type() == EquipmentType::ARMOR ? QString("防御+%1").arg(eq->defenseBonus()) :
             QString("生命+%1").arg(eq->hpBonus())));
    msgBox.addButton("确定", QMessageBox::YesRole);
    msgBox.addButton("取消", QMessageBox::NoRole);

    if (msgBox.exec() == 0) {
        m_playerGold -= price;
        updateGoldDisplay();
        emit goldChanged(m_playerGold);
        emit equipmentPurchased(eq);

        // 从商店移除已购买的装备
        m_shopEquipments.removeAt(row);
        delete eq;
        refreshEquipmentList();

        QMessageBox aboutBox(this);
        aboutBox.setIcon(QMessageBox::NoIcon);
        aboutBox.setWindowTitle("购买成功");
        aboutBox.setText(QString("成功购买 [%1]%2！")
            .arg(eq->qualityString()).arg(eq->name()));
        aboutBox.exec();
    }
}

void Shop::setRefreshCost(int cost)
{
    m_refreshCost = cost;
    m_refreshButton->setText(QString("刷新商品 (消耗%1金币)").arg(m_refreshCost));
}

void Shop::refreshShopItems(const QVector<Item*>& items)
{
    m_shopItems = items;
    refreshItemList();
}

void Shop::refreshShopEquipments(const QVector<Equipment*>& equipments)
{
    m_shopEquipments = equipments;
    refreshEquipmentList();
}

void Shop::onRefreshShop()
{
    if (m_playerGold < m_refreshCost) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::NoIcon);
        msgBox.setWindowTitle("金币不足");
        msgBox.setText(QString("刷新商店需要消耗%1金币！").arg(m_refreshCost));
        msgBox.exec();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowTitle("确认刷新");
    msgBox.setText(QString("确定刷新商店商品吗？\n将消耗%1金币").arg(m_refreshCost));
    msgBox.addButton("确定", QMessageBox::YesRole);
    msgBox.addButton("取消", QMessageBox::NoRole);

    if (msgBox.exec() == 0) {
        m_playerGold -= m_refreshCost;
        updateGoldDisplay();
        emit goldChanged(m_playerGold);
        emit refreshRequested();
    }
}

void Shop::onClose()
{
    accept();
    emit closed();
}

void Shop::closeEvent(QCloseEvent* event)
{
    emit closed();
    QDialog::closeEvent(event);
}
