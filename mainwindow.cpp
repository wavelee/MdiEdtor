#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/ICON/ICON/64x64/notepad.png"));
    actionSeparator = new QAction(this);
    actionSeparator->setSeparator(true);
    updateMenus();
    connect(ui->mdiArea,
            SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this,SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper,SIGNAL(mapped(QWidget*)),
            this,SLOT(setActiveSubWindow(QWidget*)));
    updateMenus();
    connect(ui->menu_Window,SIGNAL(aboutToShow()),
            this,SLOT(updateWindowMenu()));

    readSettings();
    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_New_triggered()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdichild() != 0);
    ui->action_Save->setEnabled(hasMdiChild);
    ui->action_SaveAs->setEnabled(hasMdiChild);
    ui->action_Paste->setEnabled(hasMdiChild);
    ui->action_Close->setEnabled(hasMdiChild);
    ui->action_CloseAll->setEnabled(hasMdiChild);
    ui->action_Tile->setEnabled(hasMdiChild);
    ui->action_Cascase->setEnabled(hasMdiChild);
    ui->action_Next->setEnabled(hasMdiChild);
    ui->action_Previous->setEnabled(hasMdiChild);

    actionSeparator->setVisible(hasMdiChild);

    bool hasSelection = (activeMdichild()
                         && activeMdichild()->textCursor().hasSelection());
    ui->action_Cut->setEnabled(hasSelection);
    ui->action_Copy->setEnabled(hasSelection);

    ui->action_Undo->setEnabled(activeMdichild()
                                &&activeMdichild()->document()->isUndoAvailable());
    ui->action_Redo->setEnabled(activeMdichild()
                                && activeMdichild()->document()->isRedoAvailable());
}

MdiChild *MainWindow::activeMdichild()
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child =  new MdiChild;
    ui->mdiArea->addSubWindow(child);
    connect(child,SIGNAL(copyAvailable(bool)),
            ui->action_Cut,SLOT(setEnabled(bool)));
    connect(child,SIGNAL(copyAvailable(bool)),
            ui->action_Copy,SLOT(setEnabled(bool)));
    connect(child->document(),SIGNAL(undoAvailable(bool)),
            ui->action_Undo,SLOT(setEnabled(bool)));

    connect(child,SIGNAL(cursorPositionChanged()),
            this,SLOT(showTextRowAndCol()));
    return child;
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing)
        {
            ui->mdiArea->setActiveSubWindow(existing);
            return;
        }
        MdiChild *child = createMdiChild();

        if (child->loadFile(fileName))
        {
            ui->statusBar->showMessage(tr("打开文件成功"),2000);
            child->show();
        }
        else
        {
            child->close();
        }
    }
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &filename)
{
    QString canonicalFilePath = QFileInfo(filename).canonicalFilePath();
    foreach(QMdiSubWindow *window,ui->mdiArea->subWindowList())
    {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }

    return 0;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::updateWindowMenu()
{
    ui->menu_Window->clear();
    ui->menu_Window->addAction(ui->action_Close);
    ui->menu_Window->addAction(ui->action_CloseAll);
    ui->menu_Window->addSeparator();
    ui->menu_Window->addAction(ui->action_Tile);
    ui->menu_Window->addAction(ui->action_Next);
    ui->menu_Window->addAction(ui->action_Previous);
    ui->menu_Window->addAction(actionSeparator);
    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    actionSeparator->setVisible(!windows.isEmpty());

    for (int i=0; i < windows.size();++i)
    {
        MdiChild *child = qobject_cast<MdiChild *> (windows.at(i)->widget());
        QString text;
        if (i < 9)
        {
            text = tr("&%1 %2").arg(i+1)
                    .arg(child->userFriendlyCurrentFile());
        }
        else
        {
            text = tr("%1 %2").arg(i+1)
                    .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = ui->menu_Window->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == activeMdichild());
        connect(action,SIGNAL(triggered()),windowMapper,SLOT(map()));
        windowMapper->setMapping(action,windows.at(i));
    }
}

void MainWindow::on_action_Save_triggered()
{
    if (activeMdichild() && activeMdichild()->save())
        ui->statusBar->showMessage(tr("文件保存成功"),2000);
}

void MainWindow::on_action_SaveAs_triggered()
{
    if (activeMdichild() && activeMdichild()->saveAs())
        ui->statusBar->showMessage(tr("文件保存成功"),2000);
}

void MainWindow::on_action_Undo_triggered()
{
    if (activeMdichild())
        activeMdichild()->undo();
}

void MainWindow::on_action_Redo_triggered()
{
    if (activeMdichild())
        activeMdichild()->redo();
}

void MainWindow::on_action_Cut_triggered()
{
    if (activeMdichild())
        activeMdichild()->cut();
}

void MainWindow::on_action_Copy_triggered()
{
    if (activeMdichild())
        activeMdichild()->copy();
}

void MainWindow::on_action_Paste_triggered()
{
    if (activeMdichild())
        activeMdichild()->paste();
}

void MainWindow::on_action_Close_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_action_CloseAll_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_action_AboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this,tr("关于多文档编辑器"),
                       tr("本编辑器是用Qt编写,联系作者"
                          "<a href = \""
                           "http://www.blueecho.info\">blueecho.info</a>"));
}

void MainWindow::on_action_Previous_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_action_Next_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow())
    {
        event->ignore();
    }
    else
    {
        writeSetting();
        event->accept();
    }
}

void MainWindow::writeSetting()
{
    QSettings settings("bluecho.info","MdiTextEditor");
    settings.setValue("pos",pos());
    settings.setValue("size",size());
}

void MainWindow::readSettings()
{
    QSettings settings("bluecho.info","MdiTextEditor");
    QPoint pos = settings.value("pos",QPoint(200,200)).toPoint();
    QSize size = settings.value("size",QSize(400,400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::on_action_Exit_triggered()
{
    qApp->closeAllWindows();
}

void MainWindow::initWindow()
{
    setWindowTitle(tr("多文档编辑器"));
    ui->mainToolBar->setWindowTitle(tr("工具栏"));
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->statusBar->showMessage(tr("欢迎使用多文档编辑器"));
    QLabel *label = new QLabel(this);
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
    label->setText(tr("<a href = \""
                      "http://www.blueecho.info\">blueecho.info</a>"));
    label->setTextFormat(Qt::RichText);
    label->setOpenExternalLinks(true);
    ui->statusBar->addPermanentWidget(label);

    ui->action_About->setStatusTip(tr("关于作者"));
    ui->action_AboutQt->setStatusTip(tr("关于Qt"));
    ui->action_Cascase->setStatusTip(tr("折叠窗口"));
    ui->action_Close->setStatusTip(tr("关闭当前窗口"));
    ui->action_CloseAll->setStatusTip(tr("关闭所有窗口"));
    ui->action_Copy->setStatusTip(tr("复制"));
    ui->action_Cut->setStatusTip(tr("剪切"));
    ui->action_Exit->setStatusTip(tr("退出程序"));
    ui->action_New->setStatusTip(tr("新建文件"));
    ui->action_Next->setStatusTip(tr("前一个窗口"));
    ui->action_Open->setStatusTip(tr("打开文档"));
    ui->action_Paste->setStatusTip(tr("粘贴"));
    ui->action_Previous->setStatusTip(tr("前一个窗口"));
    ui->action_Redo->setStatusTip(tr("恢复"));
    ui->action_Save->setStatusTip(tr("保存"));
    ui->action_SaveAs->setStatusTip(tr("另存为..."));
    ui->action_Tile->setStatusTip(tr("平铺窗口"));
    ui->action_Undo->setStatusTip(tr("撤销"));
}

void MainWindow::showTextRowAndCol()
{
    if (activeMdichild())
    {
        int rowNum = activeMdichild()->textCursor().blockNumber() + 1;
        int colNum = activeMdichild()->textCursor().columnNumber() + 1;
        ui->statusBar->showMessage(tr("%1行 %2列").arg(rowNum)
                                   .arg(colNum),2000);
    }
}
