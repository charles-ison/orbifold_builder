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
    void addSurface();
    void cutSurface();
    void glue();
    void smooth();
    void toggleShouldDeleteSurface();
    void toggleFundamentalPolygon();
    void drawingColorChanged();
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
    void zoomScaleChanged();

private:
    bool hideFundamentalPolygon;
    void createActions();
    void createMenus();
    void createToolbars();
    void initStyle();
    QToolButton *createButton(const QString &text, DiagramItem::DiagramType type);

    template<typename PointerToMemberFunction> QMenu *createColorMenu(const PointerToMemberFunction &slot, QColor defaultColor);
    template<typename PointerToMemberFunction> QMenu *createZoomMenu(const PointerToMemberFunction &slot);
    template<typename PointerToMemberFunction> QMenu *createAddSurfaceMenu(const PointerToMemberFunction &slot);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);

    DiagramScene *fundamentalPolygonScene;
    QGraphicsView *fundamentalPolygonView;
    QToolBox *fundamentalPolygonToolBox;

    int panelWidth = 500;
    int panelHeight = 500;
    double zoomScale = 1.0;
    ResultsWidget *resultsWidget;
    QToolBox *resultsToolBox;

    QAction *exitAction;
    QAction *deleteAction;
    QAction *aboutAction;
    QAction *addSurfaceAction;
    QAction *zoomAction;
    QAction *drawAction;
    QAction *fundamentalPolygonAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *resultsToolBar;
    QToolBar *fundamentalPolygonToolBar;

    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QToolButton *polygonButton;
    QToolButton *textButton;
    QToolButton *zoomButton;
    QToolButton *addSurfaceButton;
    QToolButton *drawButton;
    QToolButton *cutSurfaceButton;
    QToolButton *deleteSurfaceButton;
    QToolButton *glueButton;
    QToolButton *smoothButton;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;

    QWidget *centralWidget;
    QGridLayout *centralLayout;
};

#endif
