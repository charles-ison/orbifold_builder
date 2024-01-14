#include "Arrow.h"
#include "diagrams/DiagramItem.h"
#include "diagrams/DiagramScene.h"
#include "MainWindow.h"
#include "ResultsWidget.h"

const int InsertTextButton = 10;

MainWindow::MainWindow() {
    createActions();
    createMenus();

    builderScene = new DiagramScene(itemMenu, this);
    builderScene->setSceneRect(QRectF(0, 0, 1000, 1000));

    connect(builderScene, &DiagramScene::itemInserted,this, &MainWindow::itemInserted);
    connect(builderScene, &DiagramScene::textInserted,this, &MainWindow::textInserted);
    createToolbars();

    builderView = new QGraphicsView(builderScene);

    builderToolBox = new QToolBox;
    builderToolBox->addItem(builderView, tr("Fundamental Polygon Builder"));
    builderToolBox->setMinimumSize(500, 500);

    resultsWidget = new ResultsWidget();
    resultsWidget->setMinimumSize(500, 500);

    resultsToolBox = new QToolBox;
    resultsToolBox->addItem(resultsWidget, tr("Resulting Surface/Orbifold"));
    resultsToolBox->setMinimumSize(500, 500);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(builderToolBox, 0, 1);
    layout->addWidget(resultsToolBox, 0, 2);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("Orbifold Builder"));
    setUnifiedTitleAndToolBarOnMac(true);
    setGeometry(100, 100, 1000, 500);
    initStyle();
}

void MainWindow::initStyle() {
    builderToolBox->setStyleSheet("background-color:rgb(96,96,96); border:none;");
    resultsToolBox->setStyleSheet("background-color:rgb(96,96,96); border:none;");
    insertToolBar->setStyleSheet("border-color:rgb(96,96,96);");
    colorToolBar->setStyleSheet("border-color:rgb(96,96,96);");
    selectorToolbar->setStyleSheet("border-color:rgb(96,96,96);");
    buildToolbar->setStyleSheet("border-color:rgb(96,96,96);");
    setStyleSheet("background-color:rgb(169,169,169);");
}

void MainWindow::buttonGroupClicked(QAbstractButton *button) {
    const QList<QAbstractButton *> buttons = buttonGroup->buttons();
    for (QAbstractButton *myButton : buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    const int id = buttonGroup->id(button);
    if (id == InsertTextButton) {
        builderScene->setMode(DiagramScene::InsertText);
    } else {
        builderScene->setItemType(DiagramItem::DiagramType(id));
        builderScene->setMode(DiagramScene::InsertItem);
    }
}

void MainWindow::deleteItem() {
    QList<QGraphicsItem *> selectedItems = builderScene->selectedItems();
    for (QGraphicsItem *item : std::as_const(selectedItems)) {
        if (item->type() == Arrow::Type) {
            builderScene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        }
    }

    selectedItems = builderScene->selectedItems();
    for (QGraphicsItem *item : std::as_const(selectedItems)) {
         if (item->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
        builderScene->removeItem(item);
         delete item;
     }
}

void MainWindow::buildOrbifold() {
    resultsWidget->setShouldPaintGL(true);
}

void MainWindow::pointerGroupClicked() {
    builderScene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::itemInserted(DiagramItem *item) {
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    builderScene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    buttonGroup->button(int(item->diagramType()))->setChecked(false);
}

void MainWindow::textInserted(QGraphicsTextItem *) {
    buttonGroup->button(InsertTextButton)->setChecked(false);
    builderScene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::textColorChanged() {
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void MainWindow::itemColorChanged() {
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void MainWindow::lineColorChanged() {
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

void MainWindow::textButtonTriggered() {
    builderScene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainWindow::fillButtonTriggered() {
    builderScene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void MainWindow::lineButtonTriggered() {
    builderScene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Orbifold Builder"),
                       tr("The <b>Orbifold Builder</b> allows for the construction of orbifolds from a fundamental polygon."));
}

void MainWindow::createActions() {
    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteItem);

    buildAction = new QAction("Build", this);
    connect(buildAction, &QAction::triggered, this, &MainWindow::buildOrbifold);

    exitAction = new QAction(tr("Exit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    aboutAction = new QAction(tr("About"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolbars() {
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),this, &MainWindow::buttonGroupClicked);

    polygonButton = createButton(tr("Polygon"), DiagramItem::Step);

    textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap(":/images/textpointer.png")));

    insertToolBar = addToolBar(tr("Edit"));
    insertToolBar->addWidget(polygonButton);
    insertToolBar->addWidget(textButton);
    insertToolBar->setMovable(false);

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(&MainWindow::itemColorChanged, Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
            ":/images/floodfill.png", Qt::white));
    connect(fillColorToolButton, &QAbstractButton::clicked,this, &MainWindow::fillButtonTriggered);

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(&MainWindow::textColorChanged, Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, &QAbstractButton::clicked,this, &MainWindow::textButtonTriggered);

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(&MainWindow::lineColorChanged, Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png", Qt::black));
    connect(lineColorToolButton, &QAbstractButton::clicked,this, &MainWindow::lineButtonTriggered);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);
    colorToolBar->setMovable(false);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),this, &MainWindow::pointerGroupClicked);

    selectorToolbar = addToolBar(tr("Selector type"));
    selectorToolbar->addWidget(pointerButton);
    selectorToolbar->addWidget(linePointerButton);
    selectorToolbar->addAction(deleteAction);
    selectorToolbar->setMovable(false);

    buildToolbar = addToolBar(tr("Build"));
    buildToolbar->addAction(buildAction);
    buildToolbar->widgetForAction(buildAction)->setMinimumHeight(selectorToolbar->sizeHint().height());
    buildToolbar->setMovable(false);
}

QToolButton *MainWindow::createButton(const QString &text, DiagramItem::DiagramType type) {
    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    button->setStyleSheet("background-color:rgb(169,169,169);");
    buttonGroup->addButton(button, int(type));
    return button;
}

template<typename PointerToMemberFunction>
QMenu *MainWindow::createColorMenu(const PointerToMemberFunction &slot, QColor defaultColor) {
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue") << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, &QAction::triggered, this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color) {
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);
    return QIcon(pixmap);
}

QIcon MainWindow::createColorIcon(QColor color) {
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);
    return QIcon(pixmap);
}
