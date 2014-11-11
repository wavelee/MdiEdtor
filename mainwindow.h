#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class MdiChild;
QT_BEGIN_NAMESPACE
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_New_triggered();
    void updateMenus();
    MdiChild *createMdiChild();
    void setActiveSubWindow(QWidget *window);
    void on_action_Open_triggered();
    void updateWindowMenu();

    void on_action_Save_triggered();

    void on_action_SaveAs_triggered();

    void on_action_Undo_triggered();

    void on_action_Redo_triggered();

    void on_action_Cut_triggered();

    void on_action_Copy_triggered();

    void on_action_Paste_triggered();

    void on_action_Close_triggered();

    void on_action_CloseAll_triggered();

    void on_action_AboutQt_triggered();

    void on_action_About_triggered();

    void on_action_Previous_triggered();

    void on_action_Next_triggered();

    void on_action_Exit_triggered();

    void showTextRowAndCol();

private:
    Ui::MainWindow *ui;
    QAction *actionSeparator;
    QSignalMapper *windowMapper;    //信号映射器
    MdiChild *activeMdichild();
    QMdiSubWindow *findMdiChild(const QString &filename);
    void readSettings();
    void writeSetting();
    void initWindow();

};

#endif // MAINWINDOW_H
