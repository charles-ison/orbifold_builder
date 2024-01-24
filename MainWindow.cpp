#include "Arrow.h"
#include "diagrams/DiagramItem.h"
#include "diagrams/DiagramScene.h"
#include "MainWindow.h"
#include "ResultsWidget.h"

const int InsertTextButton = 10;

MainWindow::MainWindow() {
    createActions();
    createMenus();

    fundamentalPolygonScene = new DiagramScene(itemMenu, this);
    fundamentalPolygonScene->setSceneRect(QRectF(0, 0, 1000, 1000));

    connect(fundamentalPolygonScene, &DiagramScene::itemInserted,this, &MainWindow::itemInserted);
    connect(fundamentalPolygonScene, &DiagramScene::textInserted,this, &MainWindow::textInserted);
    createToolbars();

    fundamentalPolygonView = new QGraphicsView(fundamentalPolygonScene);

    fundamentalPolygonToolBox = new QToolBox;
    fundamentalPolygonToolBox->addItem(fundamentalPolygonView, tr("Fundamental Polygon"));
    fundamentalPolygonToolBox->setMinimumSize(panelWidth, panelHeight);

    resultsWidget = new ResultsWidget();
    resultsWidget->setMinimumSize(panelWidth, panelHeight);

    resultsToolBox = new QToolBox;
    resultsToolBox->addItem(resultsWidget, tr("Resulting Surface/Orbifold"));
    resultsToolBox->setMinimumSize(panelWidth, panelWidth);

    centralLayout = new QGridLayout;
    centralLayout->addWidget(fundamentalPolygonToolBar, 0, 0);
    centralLayout->addWidget(resultsToolBar, 0, 1);
    centralLayout->addWidget(fundamentalPolygonToolBox, 1, 0);
    centralLayout->addWidget(resultsToolBox, 1, 1);

    hideFundamentalPolygon = true;
    fundamentalPolygonToolBox->setHidden(hideFundamentalPolygon);
    fundamentalPolygonToolBar->setHidden(hideFundamentalPolygon);

    centralWidget = new QWidget;
    centralWidget->setLayout(centralLayout);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("Orbifold Builder"));
    setUnifiedTitleAndToolBarOnMac(true);
    setGeometry(100, 100, 1000, 500);
    initStyle();
}

void MainWindow::initStyle() {
    resultsToolBox->setStyleSheet("background-color:rgb(96,96,96); border:none;");
    fundamentalPolygonToolBox->setStyleSheet("background-color:rgb(96,96,96); border:none;");
    resultsToolBar->setStyleSheet("border-color:rgb(96,96,96);");
    fundamentalPolygonToolBar->setStyleSheet("border-color:rgb(96,96,96);");
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
        fundamentalPolygonScene->setMode(DiagramScene::InsertText);
    } else {
        fundamentalPolygonScene->setItemType(DiagramItem::DiagramType(id));
        fundamentalPolygonScene->setMode(DiagramScene::InsertItem);
    }
}

void MainWindow::deleteItem() {
    QList<QGraphicsItem *> selectedItems = fundamentalPolygonScene->selectedItems();
    for (QGraphicsItem *item : std::as_const(selectedItems)) {
        if (item->type() == Arrow::Type) {
            fundamentalPolygonScene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        }
    }

    selectedItems = fundamentalPolygonScene->selectedItems();
    for (QGraphicsItem *item : std::as_const(selectedItems)) {
         if (item->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
         fundamentalPolygonScene->removeItem(item);
         delete item;
     }
}

void MainWindow::addSurface() {
    addSurfaceAction = qobject_cast<QAction *>(sender());
    addSurfaceButton->menu()->setDefaultAction(addSurfaceAction);
    ResultsWidget::surface surface  = qvariant_cast<ResultsWidget::surface>(addSurfaceAction->data());
    resultsWidget->addSurface(surface);
}

void MainWindow::toggleFundamentalPolygon() {
    hideFundamentalPolygon = !hideFundamentalPolygon;
    fundamentalPolygonToolBar->setHidden(hideFundamentalPolygon);
    fundamentalPolygonToolBox->setHidden(hideFundamentalPolygon);
}

void MainWindow::drawingLineColorChanged() {
    drawLineAction = qobject_cast<QAction *>(sender());
    drawLineButton->menu()->setDefaultAction(drawLineAction);
    QColor newColor = qvariant_cast<QColor>(drawLineAction->data());
    drawLineButton->setIcon(createColorToolButtonIcon(":/images/linecolor.png",newColor));
    resultsWidget->setLineDrawingColor(newColor);
}

void MainWindow::pointerGroupClicked() {
    fundamentalPolygonScene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::itemInserted(DiagramItem *item) {
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    fundamentalPolygonScene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    buttonGroup->button(int(item->diagramType()))->setChecked(false);
}

void MainWindow::textInserted(QGraphicsTextItem *) {
    buttonGroup->button(InsertTextButton)->setChecked(false);
    fundamentalPolygonScene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::textColorChanged() {
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png",qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void MainWindow::itemColorChanged() {
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(":/images/floodfill.png",qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void MainWindow::lineColorChanged() {
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(":/images/linecolor.png",qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

void MainWindow::zoomScaleChanged() {
    zoomAction = qobject_cast<QAction *>(sender());
    zoomButton->menu()->setDefaultAction(zoomAction);
    zoomScale = qvariant_cast<double>(zoomAction->data());
    resultsWidget->setMinimumSize(zoomScale*resultsToolBox->width(), zoomScale*this->resultsToolBox->height());
}

void MainWindow::textButtonTriggered() {
    fundamentalPolygonScene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainWindow::fillButtonTriggered() {
    fundamentalPolygonScene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void MainWindow::lineButtonTriggered() {
    fundamentalPolygonScene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
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

    fundamentalPolygonAction = new QAction(tr("Toggle Fundamental Polygon View"), this);
    connect(fundamentalPolygonAction, &QAction::triggered, this, &MainWindow::toggleFundamentalPolygon);

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

    itemMenu = menuBar()->addMenu(tr("&View"));
    itemMenu->addAction(fundamentalPolygonAction);
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

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(&MainWindow::itemColorChanged, Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(":/images/floodfill.png", Qt::white));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(&MainWindow::textColorChanged, Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(&MainWindow::lineColorChanged, Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(":/images/linecolor.png", Qt::black));

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

    fundamentalPolygonToolBar = new QToolBar;
    fundamentalPolygonToolBar->addWidget(polygonButton);
    fundamentalPolygonToolBar->addWidget(textButton);
    fundamentalPolygonToolBar->addWidget(fillColorToolButton);
    fundamentalPolygonToolBar->addWidget(fontColorToolButton);
    fundamentalPolygonToolBar->addWidget(lineColorToolButton);
    fundamentalPolygonToolBar->addWidget(pointerButton);
    fundamentalPolygonToolBar->addWidget(linePointerButton);
    fundamentalPolygonToolBar->addAction(deleteAction);
    fundamentalPolygonToolBar->setMovable(false);

    zoomButton = new QToolButton;
    zoomButton->setPopupMode(QToolButton::MenuButtonPopup);
    zoomButton->setMenu(createZoomMenu(&MainWindow::zoomScaleChanged));
    zoomAction = zoomButton->menu()->defaultAction();
    zoomButton->setText("Zoom");
    zoomButton->setMinimumHeight(fundamentalPolygonToolBar->sizeHint().height());

    addSurfaceButton = new QToolButton;
    addSurfaceButton->setPopupMode(QToolButton::MenuButtonPopup);
    addSurfaceButton->setMenu(createAddSurfaceMenu(&MainWindow::addSurface));
    addSurfaceAction = addSurfaceButton->menu()->defaultAction();
    addSurfaceButton->setText("Add");
    addSurfaceButton->setMinimumHeight(fundamentalPolygonToolBar->sizeHint().height());

    drawLineButton = new QToolButton;
    drawLineButton->setPopupMode(QToolButton::MenuButtonPopup);
    drawLineButton->setMenu(createColorMenu(&MainWindow::drawingLineColorChanged, Qt::white));
    drawLineAction = drawLineButton->menu()->defaultAction();
    drawLineButton->setIcon(createColorToolButtonIcon(":/images/linecolor.png", Qt::white));
    drawLineButton->setMinimumHeight(fundamentalPolygonToolBar->sizeHint().height());

    resultsToolBar = new QToolBar;
    resultsToolBar->addWidget(zoomButton);
    resultsToolBar->addWidget(addSurfaceButton);
    resultsToolBar->addWidget(drawLineButton);
    resultsToolBar->setMovable(false);
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

template<typename PointerToMemberFunction> QMenu *MainWindow::createColorMenu(const PointerToMemberFunction &slot, QColor defaultColor) {
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

template<typename PointerToMemberFunction> QMenu *MainWindow::createZoomMenu(const PointerToMemberFunction &slot) {
    QList<double> zoomScales;
    zoomScales << 0.8 << 1.0 << 1.2 << 1.4 << 1.6 << 1.8 << 2.0;
    QStringList zoomScaleNames;
    zoomScaleNames << tr("80%") << tr("100%") << tr("120%") << tr("140%") << tr("160%") << tr("180%") << tr("200%");

    QMenu *zoomMenu = new QMenu(this);
    for (int i = 0; i < zoomScales.count(); ++i) {
        QAction *action = new QAction(zoomScaleNames.at(i), this);
        action->setData(zoomScales.at(i));
        connect(action, &QAction::triggered, this, slot);
        zoomMenu->addAction(action);
        if (zoomScales.at(i) == 1.0)
            zoomMenu->setDefaultAction(action);
    }
    return zoomMenu;
}

template<typename PointerToMemberFunction> QMenu *MainWindow::createAddSurfaceMenu(const PointerToMemberFunction &slot) {
    QList<ResultsWidget::surface> surfaceEnums;
    surfaceEnums << ResultsWidget::surface::cube << ResultsWidget::surface::sphere << ResultsWidget::surface::torus << ResultsWidget::surface::mobiusStrip << ResultsWidget::surface::crossCap << ResultsWidget::surface::kleinBottle;
    QStringList surfaceNames;
    surfaceNames << tr("Cube") << tr("Sphere") << tr("Torus") << tr("Mobius Strip") << tr("Cross-cap") << tr("Klein Bottle");

    QMenu *surfaceMenu = new QMenu(this);
    for (int i = 0; i < surfaceEnums.count(); ++i) {
        QAction *action = new QAction(surfaceNames.at(i), this);
        action->setData(surfaceEnums.at(i));
        connect(action, &QAction::triggered, this, slot);
        surfaceMenu->addAction(action);
        if (surfaceEnums.at(i) == 1.0)
            surfaceMenu->setDefaultAction(action);
    }
    return surfaceMenu;
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
