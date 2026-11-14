#ifndef BELLMANFORD_H
#define BELLMANFORD_H
#include <limits>
#include <vector>

constexpr long long VO_CUC = std::numeric_limits<long long>::max() / 4;
constexpr int SO_TRAM_TOI_DA = 1000;
constexpr int SO_TUYEN_TOI_DA = 20000;

struct Canh { int u, v; long long chiPhi; };
struct KetQua {
    std::vector<long long> d;
    std::vector<int> truoc;
    int soLuot = 0;
};

KetQua bellmanFord(int n, int s, const std::vector<Canh> &cacTuyen);
std::vector<int> truyVet(int dich, const KetQua &ketQua);
#endif
