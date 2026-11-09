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
        dscanh.push_back({u, v, cost});
        dscanh.push_back({v, u, cost});
    }
}

void BellmanFord(int s){
    vector<int> d(n + 1, 1e9);
    d[s] = 0;
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
    BellmanFord(s);
    return 0;
}
