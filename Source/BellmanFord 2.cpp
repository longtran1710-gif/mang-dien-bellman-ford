#include <algorithm>
#include <charconv>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using ll = long long;

constexpr ll INF = numeric_limits<ll>::max() / 4;
constexpr int MAX_TRAM = 1000;
constexpr int MAX_TUYEN = 20000;

struct Canh {
    int u, v;
    ll chiPhi;
};

struct MangDien {
    int soTram;
    int nguon;
    vector<Canh> dsCanh;
};

struct KetQua {
    vector<ll> chiPhi;
    vector<int> truoc;
    int soLuot = 0;
};

ll docSoNguyen(istream& input, const string& ten) {
    string token;
    if (!(input >> token)) {
        throw runtime_error("Thiếu dữ liệu: " + ten + ".");
    }

    ll giaTri = 0;
    const char* dau = token.data();
    const char* cuoi = dau + token.size();

    const auto ketQua = from_chars(dau, cuoi, giaTri);
    if (ketQua.ec != errc{} || ketQua.ptr != cuoi) {
        throw runtime_error(
            ten + " phải là số nguyên trong phạm vi long long."
        );
    }
    return giaTri;
}

MangDien nhap(istream& input, ostream& output) {
    output << "Nhập số trạm (n), số tuyến (m), trạm nguồn (s):\n";

    const ll n = docSoNguyen(input, "số trạm n");
    const ll m = docSoNguyen(input, "số tuyến m");
    const ll s = docSoNguyen(input, "trạm nguồn s");

    if (n < 1 || n > MAX_TRAM) {
        throw runtime_error(
            "Số trạm phải từ 1 đến " + to_string(MAX_TRAM) + "."
        );
    }
    if (m < 0 || m > MAX_TUYEN) {
        throw runtime_error(
            "Số tuyến phải từ 0 đến " + to_string(MAX_TUYEN) + "."
        );
    }
    if (s < 1 || s > n) {
        throw runtime_error("Trạm nguồn phải thuộc khoảng 1..n.");
    }

    MangDien mang{static_cast<int>(n), static_cast<int>(s), {}};
    mang.dsCanh.reserve(static_cast<size_t>(2 * m));

    const ll maxChiPhi = (INF - 1) / max(1LL, n - 1);

    output << "Nhập " << m << " tuyến, mỗi tuyến: u v chi_phi\n"
           << "Chi phí là số nguyên không âm, đơn vị triệu VNĐ.\n";

    for (ll i = 1; i <= m; ++i) {
        const string ten = "tuyến " + to_string(i);
        const ll u = docSoNguyen(input, "trạm u của " + ten);
        const ll v = docSoNguyen(input, "trạm v của " + ten);
        const ll chiPhi = docSoNguyen(input, "chi phí của " + ten);

        if (u < 1 || u > n || v < 1 || v > n) {
            throw runtime_error(
                "Hai đầu " + ten + " phải thuộc khoảng 1..n."
            );
        }
        if (chiPhi < 0) {
            throw runtime_error(
                "Chi phí " + ten + " không được âm. "
                "Mạng hai chiều có cạnh âm sẽ có chu trình âm."
            );
        }
        if (chiPhi > maxChiPhi) {
            throw runtime_error(
                "Chi phí " + ten + " quá lớn. Giới hạn với n=" +
                to_string(n) + " là " + to_string(maxChiPhi) + "."
            );
        }

        mang.dsCanh.push_back({
            static_cast<int>(u), static_cast<int>(v), chiPhi
        });
        mang.dsCanh.push_back({
            static_cast<int>(v), static_cast<int>(u), chiPhi
        });
    }
    return mang;
}

KetQua bellmanFord(const MangDien& mang, ostream* nhatKy = nullptr) {
    KetQua kq;
    kq.chiPhi.assign(mang.soTram + 1, INF);
    kq.truoc.assign(mang.soTram + 1, -1);
    kq.chiPhi[mang.nguon] = 0;

    for (int luot = 1; luot <= mang.soTram - 1; ++luot) {
        bool coThayDoi = false;
        ++kq.soLuot;

        if (nhatKy) {
            *nhatKy << "\nLượt " << luot << ":\n";
        }

        for (const Canh& canh : mang.dsCanh) {
            const int u = canh.u;
            const int v = canh.v;

            if (kq.chiPhi[u] == INF) {
                continue;
            }

            const ll deXuat = kq.chiPhi[u] + canh.chiPhi;

            if (deXuat < kq.chiPhi[v]) {
                if (nhatKy) {
                    *nhatKy << "  " << u << " -> " << v << ": ";

                    if (kq.chiPhi[v] == INF) {
                        *nhatKy << "chưa có đường";
                    } else {
                        *nhatKy << kq.chiPhi[v];
                    }

                    *nhatKy << " -> " << deXuat << " triệu VNĐ\n";
                }

                kq.chiPhi[v] = deXuat;
                kq.truoc[v] = u;
                coThayDoi = true;
            }
        }

        if (!coThayDoi) {
            if (nhatKy) {
                *nhatKy << "  Không có cập nhật. Dừng sớm.\n";
            }
            break;
        }
    }
    return kq;
}

vector<int> truyVet(int dich, const KetQua& kq) {
    if (kq.chiPhi[dich] == INF) {
        return {};
    }

    vector<int> duongDi;

    for (int tram = dich; tram != -1; tram = kq.truoc[tram]) {
        duongDi.push_back(tram);
    }

    reverse(duongDi.begin(), duongDi.end());
    return duongDi;
}

void inKetQua(
    const MangDien& mang,
    const KetQua& kq,
    ostream& output
) {
    output << "\n=== CHI PHÍ ĐƯỜNG ĐI THẤP NHẤT TỪ TRẠM "
           << mang.nguon << " ===\n";

    for (int tram = 1; tram <= mang.soTram; ++tram) {
        output << "Trạm " << tram << ": ";

        if (kq.chiPhi[tram] == INF) {
            output << "Không có đường kết nối từ nguồn.\n";
            continue;
        }

        output << kq.chiPhi[tram]
               << " triệu VNĐ | Đường đi: ";

        const vector<int> duongDi = truyVet(tram, kq);
        for (size_t i = 0; i < duongDi.size(); ++i) {
            if (i != 0) {
                output << " -> ";
            }
            output << duongDi[i];
        }
        output << '\n';
    }

    output << "Số lượt duyệt cạnh: " << kq.soLuot << '\n'
           << "Lưu ý: Đây là chi phí đường đi đến từng trạm, "
              "không phải tổng chi phí xây dựng tối thiểu của toàn mạng.\n";
}

void huongDan(ostream& output) {
    output << "Ứng dụng Ford–Bellman cho mạng điện — bản terminal 0.1\n"
           << "Cách chạy: ./bellmanford [--chi-tiet]\n"
           << "           ./bellmanford --chi-tiet < du_lieu.txt\n"
           << "Dữ liệu: n m s, tiếp theo là m bộ u v chi_phi.\n"
           << "Trạm được đánh số 1..n. Tuyến nối hai chiều.\n"
           << "1 <= n <= " << MAX_TRAM
           << "; 0 <= m <= " << MAX_TUYEN << ".\n"
           << "Chi phí là số nguyên không âm, đơn vị triệu VNĐ.\n"
           << "--chi-tiet: in các cập nhật trong từng lượt duyệt cạnh.\n";
}

int main(int argc, char* argv[]) {
    bool chiTiet = false;

    for (int i = 1; i < argc; ++i) {
        const string thamSo = argv[i];

        if (thamSo == "--help" || thamSo == "-h") {
            huongDan(cout);
            return 0;
        }

        if (thamSo == "--chi-tiet") {
            chiTiet = true;
        } else {
            cerr << "Tham số không hợp lệ: " << thamSo
                 << ". Dùng --help để xem hướng dẫn.\n";
            return 1;
        }
    }

    try {
        const MangDien mang = nhap(cin, cout);
        const KetQua kq = bellmanFord(
            mang, chiTiet ? &cout : nullptr
        );
        inKetQua(mang, kq, cout);
    } catch (const exception& loi) {
        cerr << "Lỗi: " << loi.what() << '\n';
        return 1;
    }

    return 0;
}
