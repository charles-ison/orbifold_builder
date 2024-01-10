#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DiagramItem.h"
#include "ResultsWidget.h"
#include <QMainWindow>
#include <QtWidgets>

class DiagramScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QButtonGroup;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
   MainWindow();

private slots:
    void buttonGroupClicked(QAbstractButton *button);
    void deleteItem();
    void pointerGroupClicked();
    void itemInserted(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void textColorChanged();
    void itemColorChanged();
    void lineColorChanged();
    void textButtonTriggered();
    void fillButtonTriggered();
    void lineButtonTriggered();
    void about();

private:
    void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();
    QWidget *createCellWidget(const QString &text, DiagramItem::DiagramType type);

    template<typename PointerToMemberFunction>
    QMenu *createColorMenu(const PointerToMemberFunction &slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);

    DiagramScene *scene;
    QGraphicsView *view;
    ResultsWidget *resultsWidget;
    QScrollArea *scrollArea;

    QAction *exitAction;
    QAction *deleteAction;
    QAction *aboutAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *editToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;

    QToolBox *toolBox;
    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;
};

#endif
