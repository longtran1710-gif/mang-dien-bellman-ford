#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bellmanford.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGraphicsScene;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    std::vector<Canh> cacTuyen;
    KetQua ketQua;

    bool dangNap = false;

    void napMau();
    void xoaKetQua();
    void themTuyen();
    void xoaTuyen();

    bool docDuLieu();
    void tinhKetQua();

    void veMang(int dich);
    void thongBao(const QString &text, bool loi = false);
};

#endif