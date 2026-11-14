#include "bellmanford.h"
#include <algorithm>
#include <stdexcept>
#include <string>

KetQua bellmanFord(int n, int s, const std::vector<Canh> &cacTuyen) {
    if (n < 1 || n > SO_TRAM_TOI_DA || s < 1 || s > n)
        throw std::invalid_argument("Số trạm hoặc trạm nguồn không hợp lệ.");
    if (cacTuyen.size() > SO_TUYEN_TOI_DA)
        throw std::invalid_argument("Số tuyến vượt giới hạn cho phép.");

    long long maxChiPhi = (VO_CUC - 1) / std::max(1, n - 1);
    std::vector<Canh> dscanh;
    int soHang = 0;
    for (Canh e : cacTuyen) {
        ++soHang;
        std::string tenHang = "Hàng " + std::to_string(soHang) + ": ";
        if (e.u < 1 || e.u > n || e.v < 1 || e.v > n)
            throw std::invalid_argument(tenHang + "mã trạm phải thuộc 1..n.");
        if (e.chiPhi < 0 || e.chiPhi > maxChiPhi)
            throw std::invalid_argument(tenHang + "chi phí phải từ 0 đến "
                                        + std::to_string(maxChiPhi) + ".");
        dscanh.push_back(e);
        dscanh.push_back({e.v, e.u, e.chiPhi});
    }

    KetQua ketQua;
    ketQua.d.assign(n + 1, VO_CUC);
    ketQua.truoc.assign(n + 1, -1);
    ketQua.d[s] = 0;
    for (int i = 1; i <= n - 1; i++) {
        ++ketQua.soLuot;
        for (Canh e : dscanh) {
            int u = e.u, v = e.v;
            long long cost = e.chiPhi;
            if (ketQua.d[u] != VO_CUC) {
                if (ketQua.d[v] > ketQua.d[u] + cost) {
                    ketQua.d[v] = ketQua.d[u] + cost;
                    ketQua.truoc[v] = u;
                }
            }
        }
    }
    return ketQua;
}

std::vector<int> truyVet(int dich, const KetQua &ketQua) {
    if (dich < 1 || dich >= static_cast<int>(ketQua.d.size()))
        throw std::out_of_range("Trạm đích không hợp lệ.");
    std::vector<int> duongDi;
    if (ketQua.d[dich] == VO_CUC) return duongDi;
    for (int v = dich; v != -1; v = ketQua.truoc[v])
        duongDi.push_back(v);
    std::reverse(duongDi.begin(), duongDi.end());
    return duongDi;
}
