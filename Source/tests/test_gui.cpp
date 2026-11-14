
#include "mainwindow.h"
#include <QApplication>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTableWidget>
#include <array>
#include <limits>
#include <random>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    app.processEvents();
    auto *input = w.findChild<QTableWidget *>("tableTuyen");
    auto *out = w.findChild<QTableWidget *>("tableKetQua");
    auto *n = w.findChild<QSpinBox *>("spinSoTram");
    auto *source = w.findChild<QSpinBox *>("spinNguon");
    auto *calc = w.findChild<QPushButton *>("btnTinh");
    auto *status = w.findChild<QLabel *>("labelStatus");
    auto *view = w.findChild<QGraphicsView *>("graphView");
    auto check = [](bool ok, const char *name) {
        if (!ok)
            qFatal("FAIL: %s", name);
    };
    check(out->rowCount() == 6 && out->item(5, 1)->text() == "14", "chi phí mặc định");
    check(out->item(5, 2)->text() == QStringLiteral("1 → 2 → 3 → 4 → 5 → 6"), "đường đi mặc định");
    int highlighted = 0;
    for (auto *item : view->scene()->items())
        if (auto *line = dynamic_cast<QGraphicsLineItem *>(item))
            if (line->pen().color() == QColor("#2f8664"))
                ++highlighted;
    check(highlighted == 5, "số cạnh được tô sáng");
    w.grab().save("preview-refined.png");
    w.resize(940, 620);
    app.processEvents();
    w.grab().save("preview-refined-small.png");
    w.resize(1180, 740);
    app.processEvents();
    input->item(0, 2)->setText("-1");
    check(out->rowCount() == 0 && view->scene()->items().isEmpty(), "xóa kết quả khi dữ liệu đổi");
    calc->click();
    check(out->rowCount() == 0 && status->text().contains(QStringLiteral("chi phí phải")),
          "chi phí âm");
    input->item(0, 2)->setText("2.5");
    calc->click();
    check(status->text().contains(QStringLiteral("số nguyên")), "số thập phân");
    input->item(0, 2)->setText("4");
    input->item(0, 0)->setText("4294967297");
    calc->click();
    check(out->rowCount() == 0 && status->text().contains(QStringLiteral("mã trạm")),
          "mã trạm vượt giới hạn int");
    w.findChild<QPushButton *>("btnMau")->click();
    w.findChild<QPushButton *>("btnThem")->click();
    check(input->rowCount() == 10, "thêm hàng");
    input->setFocus();
    calc->click();
    check(out->rowCount() == 0 && status->text().contains(QStringLiteral("Hàng 10")), "hàng trống");
    input->setCurrentCell(9, 0);
    w.findChild<QPushButton *>("btnXoa")->click();
    calc->click();
    check(input->rowCount() == 9 && out->item(5, 1)->text() == "14", "xóa hàng");
    auto setGraph = [&](int size, int s, const std::vector<std::array<long long, 3>> &edges) {
        QSignalBlocker a(n), b(source), c(input);
        n->setValue(size);
        source->setMaximum(size);
        source->setValue(s);
        input->setRowCount(int(edges.size()));
        for (int r = 0; r < int(edges.size()); ++r)
            for (int k = 0; k < 3; ++k)
                input->setItem(r, k, new QTableWidgetItem(QString::number(edges[r][k])));
        calc->click();
    };
    setGraph(9, 1,
             {{1, 2, 4},
              {1, 3, 10},
              {2, 3, 3},
              {2, 4, 8},
              {3, 4, 2},
              {3, 5, 6},
              {4, 5, 3},
              {4, 6, 7},
              {5, 6, 2},
              {6, 8, 5},
              {8, 9, 10}});
    check(out->item(8, 1)->text() == "29", "mẫu 9 trạm");
    check(out->item(6, 1)->text() == QStringLiteral("Không kết nối"), "trạm cô lập");
    w.grab().save("preview-nine.png");
    w.resize(940, 620);
    app.processEvents();
    w.grab().save("preview-nine-small.png");
    w.resize(1180, 740);
    app.processEvents();
    setGraph(3, 1, {{1, 2, 1000000000}, {2, 3, 2000000000}});
    check(out->item(2, 1)->text() == "3000000000", "chi phí lớn");
    setGraph(3, 1, {{1, 1, 0}, {1, 2, 8}, {1, 2, 2}, {2, 3, 0}, {3, 1, 2}});
    check(out->item(2, 1)->text() == "2", "tuyến song song và chu trình chi phí 0");
    setGraph(1, 1, {});
    check(out->item(0, 1)->text() == "0", "một trạm");
    setGraph(31, 1, {});
    check(out->rowCount() == 31, "mạng lớn vẫn có bảng kết quả");
    source->setValue(31);
    n->setValue(2);
    check(source->value() == 2, "nguồn bị kẹp lại");
    std::mt19937 rng(12345);
    const long long inf = std::numeric_limits<long long>::max() / 4;
    for (int test = 0; test < 60; ++test) {
        int size = 1 + rng() % 10, s = 1 + rng() % size, m = rng() % 25;
        std::vector<std::array<long long, 3>> edges;
        std::vector<std::vector<long long>> dist(size + 1, std::vector<long long>(size + 1, inf));
        for (int i = 1; i <= size; ++i)
            dist[i][i] = 0;
        for (int j = 0; j < m; ++j) {
            int u = 1 + rng() % size, v = 1 + rng() % size;
            long long cost = rng() % 21;
            edges.push_back({u, v, cost});
            dist[u][v] = dist[v][u] = std::min(dist[u][v], cost);
        }
        auto costs = dist;
        for (int k = 1; k <= size; ++k)
            for (int i = 1; i <= size; ++i)
                for (int j = 1; j <= size; ++j)
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
        setGraph(size, s, edges);
        check(out->rowCount() == size, "số hàng ngẫu nhiên");
        for (int v = 1; v <= size; ++v) {
            if (dist[s][v] == inf) {
                check(out->item(v - 1, 1)->text() == QStringLiteral("Không kết nối"),
                      "không kết nối");
                continue;
            }
            check(out->item(v - 1, 1)->text().toLongLong() == dist[s][v], "chi phí ngắn nhất ngẫu nhiên");
            auto path = out->item(v - 1, 2)->text().split(QStringLiteral(" → "));
            check(path.first().toInt() == s && path.last().toInt() == v, "hai đầu đường đi");
            long long sum = 0;
            for (int j = 1; j < path.size(); ++j)
                sum += costs[path[j - 1].toInt()][path[j].toInt()];
            check(sum == dist[s][v], "tổng chi phí đường đi");
        }
    }
    qInfo("PASS: thao tác giao diện, tô sáng đường đi, kiểm tra dữ liệu và 60 mạng ngẫu nhiên "
          "đối chiếu Floyd-Warshall.");
}
