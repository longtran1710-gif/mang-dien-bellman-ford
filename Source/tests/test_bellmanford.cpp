#include "bellmanford.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>

void kiemTra(bool dung, const char *ten) {
    if (!dung)
        throw std::runtime_error(ten);
}

void kiemTraMang(int n, int s, const std::vector<Canh> &canh) {
    KetQua kq = bellmanFord(n, s, canh);
    std::vector<std::vector<long long>> chiPhi(n + 1, std::vector<long long>(n + 1, VO_CUC));
    for (int u = 1; u <= n; ++u)
        chiPhi[u][u] = 0;
    for (Canh e : canh) {
        chiPhi[e.u][e.v] = std::min(chiPhi[e.u][e.v], e.chiPhi);
        chiPhi[e.v][e.u] = std::min(chiPhi[e.v][e.u], e.chiPhi);
    }
    auto khoangCach = chiPhi;
    for (int k = 1; k <= n; ++k)
        for (int u = 1; u <= n; ++u)
            for (int v = 1; v <= n; ++v)
                khoangCach[u][v] = std::min(khoangCach[u][v], khoangCach[u][k] + khoangCach[k][v]);
    kiemTra(kq.soLuot == n - 1, "số lượt duyệt");
    for (int v = 1; v <= n; ++v) {
        kiemTra(kq.d[v] == khoangCach[s][v], "chi phí ngắn nhất");
        auto duongDi = truyVet(v, kq);
        if (kq.d[v] == VO_CUC) {
            kiemTra(duongDi.empty(), "không có đường đi");
            continue;
        }
        kiemTra(!duongDi.empty() && duongDi.front() == s && duongDi.back() == v, "hai đầu đường đi");
        std::vector<bool> daGap(n + 1, false);
        long long tong = 0;
        for (size_t i = 0; i < duongDi.size(); ++i) {
            int u = duongDi[i];
            kiemTra(!daGap[u], "đường đi không được lặp trạm");
            daGap[u] = true;
            if (i > 0) {
                long long gia = chiPhi[duongDi[i - 1]][u];
                kiemTra(gia != VO_CUC, "cạnh của đường đi có tồn tại");
                tong += gia;
            }
        }
        kiemTra(tong == kq.d[v], "tổng chi phí đường đi");
    }
}

void phaiBaoLoi(int n, int s, const std::vector<Canh> &canh) {
    bool daBaoLoi = false;
    try {
        bellmanFord(n, s, canh);
    } catch (const std::invalid_argument &) {
        daBaoLoi = true;
    }
    kiemTra(daBaoLoi, "dữ liệu sai phải bị từ chối");
}

int main() {
    try {
        std::vector<Canh> mau = {{1, 2, 4}, {1, 3, 10}, {2, 3, 3}, {2, 4, 8}, {3, 4, 2},
                                 {3, 5, 6}, {4, 5, 3},  {4, 6, 7}, {5, 6, 2}};
        KetQua kq = bellmanFord(6, 1, mau);
        kiemTra(kq.d[6] == 14, "mẫu 6 trạm");
        kiemTra(truyVet(6, kq) == std::vector<int>({1, 2, 3, 4, 5, 6}), "đường đi mẫu");
        kiemTraMang(6, 1, mau);
        mau.push_back({6, 8, 5});
        mau.push_back({8, 9, 10});
        kq = bellmanFord(9, 1, mau);
        kiemTra(kq.d[9] == 29 && kq.d[7] == VO_CUC, "mẫu 9 trạm");
        kiemTraMang(9, 1, mau);
        kiemTraMang(1, 1, {});
        kiemTraMang(4, 3, {});
        kiemTraMang(4, 1, {{1, 2, 0}, {2, 3, 0}, {3, 1, 0}, {3, 4, 5}});
        kiemTraMang(3, 3, {{1, 1, 0}, {1, 2, 10}, {1, 2, 2}, {2, 3, 4}});
        kiemTraMang(3, 1, {{1, 2, 1000000000}, {2, 3, 2000000000}});
        kiemTraMang(5, 1, {{4, 5, 2}, {3, 4, 2}, {2, 3, 2}, {1, 2, 2}});
        long long ganGioiHan = (VO_CUC - 1) / 3;
        kiemTraMang(4, 1, {{1, 2, ganGioiHan}, {2, 3, ganGioiHan}, {3, 4, ganGioiHan}});
        phaiBaoLoi(0, 1, {});
        phaiBaoLoi(SO_TRAM_TOI_DA + 1, 1, {});
        phaiBaoLoi(3, 4, {});
        phaiBaoLoi(3, 1, {{1, 4, 2}});
        phaiBaoLoi(3, 1, {{0, 2, 2}});
        phaiBaoLoi(3, 1, {{1, 2, -1}});
        phaiBaoLoi(2, 1, {{1, 2, VO_CUC}});
        phaiBaoLoi(4, 1, {{1, 2, ganGioiHan + 1}});
        phaiBaoLoi(1, 1, std::vector<Canh>(SO_TUYEN_TOI_DA + 1, {1, 1, 0}));
        bool loiDich = false;
        try {
            truyVet(0, kq);
        } catch (const std::out_of_range &) {
            loiDich = true;
        }
        kiemTra(loiDich, "trạm đích sai");

        std::mt19937 ngauNhien(20261113);
        for (int lan = 0; lan < 100; ++lan) {
            int n = 1 + ngauNhien() % 12, s = 1 + ngauNhien() % n;
            int m = ngauNhien() % 35;
            std::vector<Canh> canh;
            for (int i = 0; i < m; ++i) {
                int u = 1 + ngauNhien() % n, v = 1 + ngauNhien() % n;
                canh.push_back({u, v, static_cast<long long>(ngauNhien() % 21)});
            }
            kiemTraMang(n, s, canh);
        }
        std::cout << "PASS: ví dụ mẫu, kiểm tra dữ liệu, kiểm tra đường đi và 100 mạng ngẫu nhiên.\n";
    } catch (const std::exception &loi) {
        std::cerr << "FAIL: " << loi.what() << '\n';
        return 1;
    }
    return 0;
}
