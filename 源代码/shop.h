#ifndef SHOP_H
#define SHOP_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QVector>
#include "item.h"
#include "equipment.h"

class Shop : public QDialog
{
    Q_OBJECT

public:
    explicit Shop(QWidget *parent = nullptr);
    ~Shop();

    void setGold(int gold) { m_playerGold = gold; updateGoldDisplay(); }
    void setShopLevel(int level) { m_shopLevel = level; }
    void setItems(const QVector<Item*>& items);
    void setEquipments(const QVector<Equipment*>& equipments);
    void setRefreshCost(int cost);
    void refreshShopItems(const QVector<Item*>& items);
    void refreshShopEquipments(const QVector<Equipment*>& equipments);
    void generateShopItems();
    void generateShopEquipments();
    void refreshItemList();
    void refreshEquipmentList();

signals:
    void itemPurchased(Item* item);
    void equipmentPurchased(Equipment* equipment);
    void goldChanged(int newGold);
    void closed();
    void refreshRequested();

private slots:
    void onBuyItem();
    void onBuyEquipment();
    void onRefreshShop();
    void onClose();

protected:
    void closeEvent(QCloseEvent* event);

private:
    int m_playerGold;
    int m_shopLevel;
    int m_refreshCost;

    QVector<Item*> m_shopItems;
    QVector<Equipment*> m_shopEquipments;

    QLabel* m_goldLabel;
    QListWidget* m_itemList;
    QListWidget* m_equipmentList;
    QPushButton* m_buyItemButton;
    QPushButton* m_buyEquipmentButton;
    QPushButton* m_refreshButton;
    QPushButton* m_closeButton;

    void updateGoldDisplay();
};

#endif // SHOP_H
