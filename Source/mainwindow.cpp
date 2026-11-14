#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QHeaderView>
#include <QMap>
#include <QPainter>
#include <QEvent>
#include <QSet>
#include <QStringList>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr double PI = 3.141592653589793;
constexpr double LE_SO_DO = 38;
constexpr double BAN_KINH_TRAM = 14;
constexpr double BAN_KINH_TOI_THIEU = 20;
constexpr double BAN_KINH_KHUYEN = 15;
constexpr double LECH_KHUYEN = 21;
constexpr double TI_LE_NHAN_CANH_KE = 0.4;
constexpr double TI_LE_NHAN_CANH_CHEO = 0.6;
constexpr int GIOI_HAN_VE_TRAM = 30;
constexpr int GIOI_HAN_VE_CANH = 80;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    scene(new QGraphicsScene(this)) {
    ui->setupUi(this);

    setStyleSheet(QStringLiteral(R"(
        QWidget { font-family: "Segoe UI"; font-size: 13px; color: #273746; }
        QMainWindow, QWidget#centralwidget { background: #f0f3f5; }
        QLabel#labelTitle { font-size: 21px; font-weight: 600; color: #244b65; }
        QLabel#labelInput, QLabel#labelGraph, QLabel#labelResult { font-weight: 600; }
        QLabel#labelSubtitle, QLabel#labelGuide, QLabel#labelLegend,
        QLabel#labelFootnote { color: #64717d; }
        QLabel#labelRoute { color: #286c54; font-weight: 600; }
        QFrame#inputCard, QFrame#graphCard, QFrame#resultCard {
            background: white; border: 1px solid #d6dfe6; border-radius: 5px;
        }
        QPushButton { background: #edf1f4; border: 1px solid #c6d0d8; outline: none;
                      border-radius: 3px; padding: 7px 10px; }
        QPushButton:hover { background: #dfe8ee; }
        QPushButton:pressed { background: #cddce5; }
        QPushButton:focus { border-color: #36765c; }
        QPushButton#btnTinh:focus { border-color: #1b4636; }
        QPushButton#btnTinh { background: #36765c; color: white; border-color: #36765c; }
        QPushButton#btnTinh:hover { background: #2a604a; }
        QSpinBox { background: white; border: 1px solid #c6d0d8; border-radius: 3px;
                   padding: 3px 6px; }
        QSpinBox:focus { border-color: #36765c; }
        QSpinBox::up-button, QSpinBox::down-button { width: 0px; border: none; }
        QTableWidget { outline:none; background: white; alternate-background-color: #f5f7f9;
            border: 1px solid #d6dfe6; gridline-color: #e3e9ee;
            selection-background-color: #deeee5; selection-color: #23372d; }
        QTableWidget::item { padding: 3px 8px; }
        QHeaderView::section { background: #eaf0f4; border: none;
            border-bottom: 1px solid #d6dfe6; padding: 7px; }
        QGraphicsView { background: white; border: none; }
    )"));
    ui->spinSoTram->setMaximum(SO_TRAM_TOI_DA);
    for (auto *spin : {ui->spinSoTram, ui->spinNguon}) {
        spin->setFixedSize(72, 30);
        spin->setAlignment(Qt::AlignCenter);

    }
    ui->fieldsLayout->addStretch();
    for (auto *nut : {ui->btnMau, ui->btnThem, ui->btnXoa, ui->btnTinh}) {
        nut->setFocusPolicy(Qt::StrongFocus);
    }
    ui->spinSoTram->setFocus();
    ui->splitter->setSizes({340, 260});

    ui->graphView->setScene(scene);
    ui->graphView->setRenderHint(QPainter::Antialiasing);
    ui->graphView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphView->viewport()->installEventFilter(this);

    for (auto *table : {ui->tableTuyen, ui->tableKetQua}) {
        table->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch
            );
        table->verticalHeader()->hide();
        table->verticalHeader()->setDefaultSectionSize(28);
        table->setAlternatingRowColors(true);
        table->setShowGrid(true);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
    }

    ui->tableKetQua->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableKetQua->horizontalHeaderItem(2)->setTextAlignment(
        Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableKetQua->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents
        );
    ui->tableKetQua->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents
        );

    connect(ui->btnMau, &QPushButton::clicked,
            this, &MainWindow::napMau);
    connect(ui->btnThem, &QPushButton::clicked,
            this, &MainWindow::themTuyen);
    connect(ui->btnXoa, &QPushButton::clicked,
            this, &MainWindow::xoaTuyen);
    connect(ui->btnTinh, &QPushButton::clicked,
            this, &MainWindow::tinhKetQua);

    connect(ui->spinSoTram, &QSpinBox::valueChanged,
            this, [this](int n) {
                ui->spinNguon->setMaximum(n);
                xoaKetQua();
            });

    connect(ui->spinNguon, &QSpinBox::valueChanged,
            this, [this] {
                xoaKetQua();
            });

    connect(ui->tableTuyen, &QTableWidget::itemChanged,
            this, [this] {
                xoaKetQua();
            });

    connect(ui->tableKetQua, &QTableWidget::currentCellChanged,
            this, [this](int row, int, int, int) {
                if (row >= 0 && !ketQua.d.empty()) {
                    veMang(row + 1);
                }
            });

    napMau();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::thongBao(const QString &text, bool loi) {
    ui->labelStatus->setText(text);
    ui->labelStatus->setStyleSheet(
        loi ? "color: #b43e43;" : "color: #286c54;"
        );
}

void MainWindow::xoaKetQua() {
    if (dangNap) return;

    ketQua = {};
    cacTuyen.clear();

    ui->tableKetQua->setRowCount(0);
    scene->clear();

    ui->labelRoute->setText(
        QStringLiteral("Bấm Tính đường đi để cập nhật sơ đồ.")
        );

    thongBao(
        QStringLiteral("Dữ liệu đã thay đổi — cần tính lại kết quả.")
        );
}

void MainWindow::napMau() {
    dangNap = true;

    ui->spinSoTram->setValue(6);
    ui->spinNguon->setValue(1);

    const int mau[9][3] = {
        {1, 2, 4},
        {1, 3, 10},
        {2, 3, 3},
        {2, 4, 8},
        {3, 4, 2},
        {3, 5, 6},
        {4, 5, 3},
        {4, 6, 7},
        {5, 6, 2}
    };

    ui->tableTuyen->setRowCount(9);

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 3; ++c) {
            auto *item = new QTableWidgetItem(QString::number(mau[r][c]));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableTuyen->setItem(r, c, item);
        }
    }

    dangNap = false;
    tinhKetQua();
}

void MainWindow::themTuyen() {
    const int r = ui->tableTuyen->rowCount();

    if (r >= SO_TUYEN_TOI_DA) {
        thongBao(
            QStringLiteral("Tối đa %1 tuyến.").arg(SO_TUYEN_TOI_DA), true
            );
        return;
    }

    ui->tableTuyen->insertRow(r);

    for (int c = 0; c < 3; ++c) {
        auto *item = new QTableWidgetItem;
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableTuyen->setItem(r, c, item);
    }

    ui->tableTuyen->setCurrentCell(r, 0);
    ui->tableTuyen->editItem(ui->tableTuyen->item(r, 0));

    xoaKetQua();
}

void MainWindow::xoaTuyen() {
    const int r = ui->tableTuyen->currentRow();

    if (r < 0) {
        thongBao(
            QStringLiteral("Chọn một hàng trong bảng tuyến để xóa."),
            true
            );
        return;
    }

    ui->tableTuyen->removeRow(r);
    xoaKetQua();
}

bool MainWindow::docDuLieu() {
    const int n = ui->spinSoTram->value();

    for (int r = 0; r < ui->tableTuyen->rowCount(); ++r) {
        long long x[3] = {};

        for (int c = 0; c < 3; ++c) {
            const auto *item = ui->tableTuyen->item(r, c);
            bool ok = false;

            if (item) {
                x[c] = item->text().trimmed().toLongLong(&ok);
            }

            if (!ok) {
                thongBao(
                    QStringLiteral(
                        "Hàng %1, cột %2: cần nhập số nguyên hợp lệ."
                        ).arg(r + 1).arg(c + 1),
                    true
                    );
                ui->tableTuyen->setCurrentCell(r, c);
                return false;
            }
        }

        if (x[0] < 1 || x[0] > n || x[1] < 1 || x[1] > n) {
            thongBao(
                QStringLiteral(
                    "Hàng %1: mã trạm phải từ 1 đến %2."
                    ).arg(r + 1).arg(n),
                true
                );
            return false;
        }

        cacTuyen.push_back({
            static_cast<int>(x[0]),
            static_cast<int>(x[1]),
            x[2]
        });
    }

    return true;
}

void MainWindow::tinhKetQua() {
    xoaKetQua();

    if (!docDuLieu()) return;

    const int n = ui->spinSoTram->value();
    try {
        ketQua = bellmanFord(n, ui->spinNguon->value(), cacTuyen);
    } catch (const std::exception &loi) {
        thongBao(QString::fromUtf8(loi.what()), true);
        return;
    }

    int soTramDenDuoc = 0;
    ui->tableKetQua->setRowCount(n);

    for (int v = 1; v <= n; ++v) {
        QStringList path;

        for (int u : truyVet(v, ketQua)) {
            path << QString::number(u);
        }

        const bool denDuoc = ketQua.d[v] != VO_CUC;
        if (denDuoc) ++soTramDenDuoc;

        const QStringList row = {
            QStringLiteral("Trạm %1").arg(v),
            denDuoc
                ? QString::number(ketQua.d[v])
                : QStringLiteral("Không kết nối"),
            denDuoc
                ? path.join(QStringLiteral(" → "))
                : QStringLiteral("—")
        };

        for (int c = 0; c < 3; ++c) {
            auto *item = new QTableWidgetItem(row[c]);
            item->setTextAlignment(
                c == 2 ? Qt::AlignLeft | Qt::AlignVCenter : Qt::AlignCenter);
            ui->tableKetQua->setItem(v - 1, c, item);
        }
    }

    thongBao(
        QStringLiteral(
            "%1/%2 trạm kết nối được từ nguồn"
            ).arg(soTramDenDuoc).arg(n)
        );

    ui->tableKetQua->setCurrentCell(n - 1, 0);
    ui->tableKetQua->scrollToTop();
}

void MainWindow::veMang(int dich) {
    scene->clear();

    const double rong = ui->graphView->viewport()->width() - 4;
    const double cao = ui->graphView->viewport()->height() - 4;
    scene->setSceneRect(0, 0, rong, cao);

    const int n = ui->spinSoTram->value();
    const int nguon = ui->spinNguon->value();

    ui->labelRoute->setText(
        ketQua.d[dich] == VO_CUC
            ? QStringLiteral("Không có đường từ trạm %1 đến trạm %2.")
                  .arg(nguon).arg(dich)
            : QStringLiteral("Trạm %1 → Trạm %2: %3 triệu VNĐ")
                  .arg(nguon).arg(dich).arg(ketQua.d[dich]));

    QMap<QPair<int, int>, long long> canhVe;
    for (const Canh &e : cacTuyen) {
        const QPair<int, int> cap{std::min(e.u, e.v), std::max(e.u, e.v)};
        if (!canhVe.contains(cap) || e.chiPhi < canhVe[cap]) {
            canhVe[cap] = e.chiPhi;
        }
    }

    if (n > GIOI_HAN_VE_TRAM || canhVe.size() > GIOI_HAN_VE_CANH) {
        auto *thongTin = scene->addText(
            QStringLiteral("Mạng lớn: xem đường đi trong bảng kết quả."));
        thongTin->setTextWidth(rong - 20);
        thongTin->setPos(10, 10);
        return;
    }

    const QPointF tam(rong / 2, cao / 2);
    const double banKinhX = std::max(BAN_KINH_TOI_THIEU, rong / 2 - LE_SO_DO);
    const double banKinhY = std::max(BAN_KINH_TOI_THIEU, cao / 2 - LE_SO_DO);
    std::vector<QPointF> viTri(n + 1);
    for (int v = 1; v <= n; ++v) {
        const double goc = -PI / 2 + 2 * PI * (v - 1) / n;
        viTri[v] = n == 1
            ? tam
            : tam + QPointF(banKinhX * std::cos(goc), banKinhY * std::sin(goc));
    }

    QSet<QPair<int, int>> canhChon;
    const std::vector<int> duongDi = truyVet(dich, ketQua);
    for (size_t i = 1; i < duongDi.size(); ++i) {
        canhChon.insert({std::min(duongDi[i - 1], duongDi[i]),
                         std::max(duongDi[i - 1], duongDi[i])});
    }

    for (auto it = canhVe.cbegin(); it != canhVe.cend(); ++it) {
        const int u = it.key().first;
        const int v = it.key().second;
        const bool chon = canhChon.contains(it.key());
        const QPen net(QColor(chon ? "#2f8664" : "#b9c6d1"), chon ? 3 : 1.5);

        QPointF viTriNhan;
        if (u == v) {
            viTriNhan = viTri[u] - QPointF(LECH_KHUYEN, LECH_KHUYEN);
            scene->addEllipse(viTriNhan.x() - BAN_KINH_KHUYEN,
                              viTriNhan.y() - BAN_KINH_KHUYEN,
                              2 * BAN_KINH_KHUYEN, 2 * BAN_KINH_KHUYEN, net);
        } else {
            const QLineF tuyen(viTri[u], viTri[v]);
            scene->addLine(tuyen, net);
            viTriNhan = tuyen.pointAt(
                v - u == 1 ? TI_LE_NHAN_CANH_KE : TI_LE_NHAN_CANH_CHEO);
        }

        QFont phong("Segoe UI", 10);
        phong.setBold(chon);
        auto *nhan = scene->addSimpleText(QString::number(it.value()), phong);
        nhan->setBrush(QColor(chon ? "#25694f" : "#344b5d"));
        nhan->setPos(viTriNhan - nhan->boundingRect().center());
        nhan->setZValue(2);
        scene->addRect(nhan->sceneBoundingRect().adjusted(-2, -1, 2, 1),
                       QPen(Qt::NoPen), QBrush(Qt::white))->setZValue(1);
    }

    for (int v = 1; v <= n; ++v) {
        const QColor mau(v == nguon ? "#326b9d" : v == dich ? "#b47a21" : "#344c60");
        scene->addEllipse(viTri[v].x() - BAN_KINH_TRAM, viTri[v].y() - BAN_KINH_TRAM,
                          2 * BAN_KINH_TRAM, 2 * BAN_KINH_TRAM,
                          QPen(Qt::white, 2), QBrush(mau))->setZValue(3);
        auto *so = scene->addSimpleText(QString::number(v),
                                        QFont("Segoe UI", 10, QFont::Bold));
        so->setBrush(Qt::white);
        so->setPos(viTri[v] - so->boundingRect().center());
        so->setZValue(4);
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    if (object == ui->graphView->viewport()
        && event->type() == QEvent::Resize
        && !ketQua.d.empty() && ui->tableKetQua->currentRow() >= 0) {
        veMang(ui->tableKetQua->currentRow() + 1);
    }

    return QMainWindow::eventFilter(object, event);
}
