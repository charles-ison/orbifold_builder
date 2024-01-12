#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "diagrams/DiagramItem.h"
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
    void buildOrbifold();
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
    void createActions();
    void createMenus();
    void createToolbars();
    void initStyle();
    QToolButton *createButton(const QString &text, DiagramItem::DiagramType type);

    template<typename PointerToMemberFunction>
    QMenu *createColorMenu(const PointerToMemberFunction &slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);

    DiagramScene *builderScene;
    QGraphicsView *builderView;
    QToolBox *builderToolBox;

    ResultsWidget *resultsWidget;
    QScrollArea *resultsScrollArea;
    QToolBox *resultsToolBox;

    QAction *exitAction;
    QAction *deleteAction;
    QAction *aboutAction;
    QAction *buildAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *insertToolBar;
    QToolBar *colorToolBar;
    QToolBar *selectorToolbar;
    QToolBar *buildToolbar;

    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QToolButton *polygonButton;
    QToolButton *textButton;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;
};

#endif
