#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>


class MdiChild : public QTextEdit
{
    Q_OBJECT
public:
    explicit MdiChild(QWidget *parent = 0);
    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile()
    {
        return curFile;
    }
protected:
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

signals:

public slots:
private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setcurrentFile(const QString &fileName);
    QString curFile;
    bool isUntitled;

};

#endif // MDICHILD_H
