#include<bits/stdc++.h>
using namespace std;
using ll = long long;

struct edge {
    int u, v;     
    int cost;
}; 
int n, m, s;
vector<edge> dscanh;

void nhap() {
    cout << "Nhap so tram dien (n), so tuyen duong (m), va tram phat chinh (s): ";
    cin >> n >> m >> s;
    
    cout << "Nhap thong tin cac tuyen duong (tram_u  tram_v  chi_phi):\n";
    for(int i = 0; i < m; i++){
        int u, v, cost; 
        cin >> u >> v >> cost;
        
        // Vì mạng điện lưới có thể truyền tải 2 chiều, ta đẩy cả 2 chiều vào danh sách cạnh
        dscanh.push_back({u, v, cost});
        dscanh.push_back({v, u, cost});
    }
}

void ThietKeMangDien(int s){
    // Mảng d lưu chi phí thấp nhất từ trạm phát s đến các trạm còn lại
    // Ban đầu khởi tạo tất cả chi phí là vô cực (1e9)
    vector<int> d(n + 1, 1e9);
    d[s] = 0;
    // Thuật toán Bellman-Ford chạy n - 1 lần để tối ưu chi phí
    for(int i = 1; i <= n - 1; i++){
        for(edge e : dscanh){
            int u = e.u, v = e.v, cost = e.cost;
            if(d[u] != 1e9){
                if(d[v] > d[u] + cost){
                    d[v] = d[u] + cost; 
                }
            }
        }
    }
    // In ra kết quả chi phí tối ưu từ trạm s đến tất cả các trạm
    cout << "\n--- KẾT QUẢ TỐI ƯU CHI PHÍ TỪ TRẠM " << s << " ---\n";
    for(int i = 1; i <= n; i++) {
        if(d[i] == 1e9)
            cout << "Trạm " << i << ": Không thể kéo điện tới!\n";
        else
            cout << "Chi phí tối thiểu đến Trạm " << i << " là: " << d[i] << " triệu VNĐ\n";
    }
}

int main(){
    nhap();
    ThietKeMangDien(s);
    return 0;
}
