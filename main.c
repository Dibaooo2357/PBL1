#include "raylib.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

// --- BIẾN TOÀN CỤC ---
int m_goc = 3, n_goc = 3;
int m = 3, n = 3;

int ChiPhiGoc[MAX][MAX] = {
    {4, 4, 6},
    {1, 4, 6},
    {3, 5, 2}
};
int ChiPhi[MAX][MAX]; 

int CungGoc[MAX] = {70, 60, 50};
int CauGoc[MAX] = {50, 50, 50};
int Cung[MAX], Cau[MAX];

int KhongCanBang = 0; 
int PhanPhoi[MAX][MAX];
int ODaChon[MAX][MAX];

int Cha[MAX * 2];
int sz[MAX * 2];

bool DaTinhToan = false;
bool DaToiUuModi = false;
char TenPhuongPhapGoc[128] = ""; 
char TenPhuongPhapHienTai[128] = "";
int TongChiPhiHienTai = 0;
char ChuoiCongThucChiTiet[2048] = ""; 

// --- ĐỊNH NGHĨA CON CHUỘT ĐANG FOCUS VÀO ĐÂU ---
// 0: Chi phí gốc, 1: Cung gốc, 2: Cầu gốc
// 3: Nhập Số lượng Cung (m_goc), 4: Nhập Số lượng Cầu (n_goc)
int FocusLoai = 0; 
int FocusR = 0, FocusC = 0;

// --- LOGIC THUẬT TOÁN ---
void DocDuLieuTuFile() {
    FILE *f = fopen("input.txt", "r");
    if (f == NULL) return;
    if (fscanf(f, "%d %d", &m_goc, &n_goc) != 2) { fclose(f); return; }
    if (m_goc > MAX) m_goc = MAX; if (n_goc > MAX) n_goc = MAX;
    if (m_goc < 1) m_goc = 1;     if (n_goc < 1) n_goc = 1;
    for (int i = 0; i < m_goc; i++) {
        for (int j = 0; j < n_goc; j++) { if (fscanf(f, "%d", &ChiPhiGoc[i][j]) != 1) ChiPhiGoc[i][j] = 0; }
    }
    for (int i = 0; i < m_goc; i++) { if (fscanf(f, "%d", &CungGoc[i]) != 1) CungGoc[i] = 0; }
    for (int j = 0; j < n_goc; j++) { if (fscanf(f, "%d", &CauGoc[j]) != 1) CauGoc[j] = 0; }
    fclose(f); DaTinhToan = false; DaToiUuModi = false;
}

void ResetPhanPhoi() {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) PhanPhoi[i][j] = 0;
    }
    ChuoiCongThucChiTiet[0] = '\0';
}

void DatLaiBangVeZero() {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            ChiPhiGoc[i][j] = 0;
        }
        CungGoc[i] = 0;
        CauGoc[i] = 0;
    }
    ResetPhanPhoi();
    DaTinhToan = false;
    DaToiUuModi = false;
    FocusLoai = 0; FocusR = 0; FocusC = 0;
}

void XuLyCanBang() {
    if (m_goc < 1) m_goc = 1;
    if (n_goc < 1) n_goc = 1;
    
    m = m_goc; n = n_goc;
    for (int i = 0; i < m; i++) Cung[i] = CungGoc[i];
    for (int j = 0; j < n; j++) Cau[j] = CauGoc[j];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) ChiPhi[i][j] = ChiPhiGoc[i][j];
    }
    int TongCung = 0, TongCau = 0;
    for (int i = 0; i < m; i++) TongCung += Cung[i];
    for (int j = 0; j < n; j++) TongCau += Cau[j];
    if (TongCung == TongCau) { KhongCanBang = 0; return; }
    if (TongCung > TongCau) {
        if (n < MAX) {
            Cau[n] = TongCung - TongCau; for (int i = 0; i < m; i++) ChiPhi[i][n] = 0; n++; KhongCanBang = 1; 
        }
    } else {
        if (m < MAX) {
            Cung[m] = TongCau - TongCung; for (int j = 0; j < n; j++) ChiPhi[m][j] = 0; m++; KhongCanBang = 2; 
        }
    }
}

void TaoChuoiCongThuc() {
    char Temp[128];
    strcpy(ChuoiCongThucChiTiet, "CHI PHI: ");
    bool CoPhanTuDau = false; int Tong = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (PhanPhoi[i][j] > 0) {
                int cp = ChiPhi[i][j]; int sl = PhanPhoi[i][j]; Tong += sl * cp;
                if (CoPhanTuDau) strcat(ChuoiCongThucChiTiet, " + ");
                sprintf(Temp, "%dx%d", sl, cp); strcat(ChuoiCongThucChiTiet, Temp); CoPhanTuDau = true;
            }
        }
    }
    sprintf(Temp, " = %d", Tong); strcat(ChuoiCongThucChiTiet, Temp); TongChiPhiHienTai = Tong;
}

void PhuongPhapGocTayBac() {
    XuLyCanBang(); ResetPhanPhoi();
    int CungConLai[MAX], CauConLai[MAX];
    for (int i = 0; i < m; i++) CungConLai[i] = Cung[i];
    for (int j = 0; j < n; j++) CauConLai[j] = Cau[j];
    int i = 0, j = 0;
    while (i < m && j < n) {
        int SoLuong = (CungConLai[i] < CauConLai[j]) ? CungConLai[i] : CauConLai[j];
        PhanPhoi[i][j] = SoLuong; CungConLai[i] -= SoLuong; CauConLai[j] -= SoLuong;
        if (CungConLai[i] == 0) i++; else j++;
    }
    strcpy(TenPhuongPhapGoc, "Phuong Phap Goc Tay Bac");
    strcpy(TenPhuongPhapHienTai, TenPhuongPhapGoc); 
    TaoChuoiCongThuc(); DaTinhToan = true; DaToiUuModi = false;
}

void PhuongPhapChiPhiNhoNhat() {
    XuLyCanBang(); ResetPhanPhoi();
    int CungConLai[MAX], CauConLai[MAX], DaDongHang[MAX] = {0}, DaDongCot[MAX] = {0};
    for (int i = 0; i < m; i++) CungConLai[i] = Cung[i];
    for (int j = 0; j < n; j++) CauConLai[j] = Cau[j];
    while (1) {
        int MinChiPhi = INT_MAX, Hang = -1, Cot = -1;
        for (int i = 0; i < m; i++) {
            if (DaDongHang[i]) continue;
            for (int j = 0; j < n; j++) {
                if (DaDongCot[j]) continue;
                if (ChiPhi[i][j] < MinChiPhi) { MinChiPhi = ChiPhi[i][j]; Hang = i; Cot = j; }
            }
        }
        if (Hang == -1 || Cot == -1) break;
        int SoLuong = (CungConLai[Hang] < CauConLai[Cot]) ? CungConLai[Hang] : CauConLai[Cot];
        PhanPhoi[Hang][Cot] = SoLuong; CungConLai[Hang] -= SoLuong; CauConLai[Cot] -= SoLuong;
        if (CungConLai[Hang] == 0) DaDongHang[Hang] = 1; else DaDongCot[Cot] = 1;
    }
    strcpy(TenPhuongPhapGoc, "Phuong Phap Chi Phi Nho Nhat");
    strcpy(TenPhuongPhapHienTai, TenPhuongPhapGoc);
    TaoChuoiCongThuc(); DaTinhToan = true; DaToiUuModi = false;
}

void PhuongPhapVogel() {
    XuLyCanBang(); ResetPhanPhoi();
    int CungConLai[MAX], CauConLai[MAX], DaDongHang[MAX] = {0}, DaDongCot[MAX] = {0};
    for (int i = 0; i < m; i++) CungConLai[i] = Cung[i];
    for (int j = 0; j < n; j++) CauConLai[j] = Cau[j];
    while (1) {
        int HangConLai = 0, CotConLai = 0;
        for (int i = 0; i < m; i++) if (!DaDongHang[i]) HangConLai++;
        for (int j = 0; j < n; j++) if (!DaDongCot[j]) CotConLai++;
        if (HangConLai == 0 || CotConLai == 0) break;
        int MaxPenalty = -1, HangMax = -1, CotMax = -1; char Loai = 'H';
        for (int i = 0; i < m; i++) {
            if (DaDongHang[i]) continue;
            int Min1 = INT_MAX, Min2 = INT_MAX;
            for (int j = 0; j < n; j++) {
                if (DaDongCot[j]) continue;
                if (ChiPhi[i][j] < Min1) { Min2 = Min1; Min1 = ChiPhi[i][j]; }
                else if (ChiPhi[i][j] < Min2) { Min2 = ChiPhi[i][j]; }
            }
            int Penalty = (Min2 == INT_MAX) ? Min1 : (Min2 - Min1);
            if (Penalty > MaxPenalty) { MaxPenalty = Penalty; HangMax = i; Loai = 'H'; }
        }
        for (int j = 0; j < n; j++) {
            if (DaDongCot[j]) continue;
            int Min1 = INT_MAX, Min2 = INT_MAX;
            for (int i = 0; i < m; i++) {
                if (DaDongHang[i]) continue;
                if (ChiPhi[i][j] < Min1) { Min2 = Min1; Min1 = ChiPhi[i][j]; }
                else if (ChiPhi[i][j] < Min2) { Min2 = ChiPhi[i][j]; }
            }
            int Penalty = (Min2 == INT_MAX) ? Min1 : (Min2 - Min1);
            if (Penalty > MaxPenalty) { MaxPenalty = Penalty; CotMax = j; Loai = 'C'; }
        }
        int Hang = -1, Cot = -1, MinChiPhi = INT_MAX;
        if (Loai == 'H') {
            Hang = HangMax;
            for (int j = 0; j < n; j++) {
                if (DaDongCot[j]) continue;
                if (ChiPhi[Hang][j] < MinChiPhi) { MinChiPhi = ChiPhi[Hang][j]; Cot = j; }
            }
        } else {
            Cot = CotMax;
            for (int i = 0; i < m; i++) {
                if (DaDongHang[i]) continue;
                if (ChiPhi[i][Cot] < MinChiPhi) { MinChiPhi = ChiPhi[i][Cot]; Hang = i; }
            }
        }
        if (Hang == -1 || Cot == -1) break;
        int SoLuong = (CungConLai[Hang] < CauConLai[Cot]) ? CungConLai[Hang] : CauConLai[Cot];
        PhanPhoi[Hang][Cot] = SoLuong; CungConLai[Hang] -= SoLuong; CauConLai[Cot] -= SoLuong;
        if (CungConLai[Hang] == 0) DaDongHang[Hang] = 1; else DaDongCot[Cot] = 1;
    }
    strcpy(TenPhuongPhapGoc, "Thuat Toan Vogel");
    strcpy(TenPhuongPhapHienTai, TenPhuongPhapGoc);
    TaoChuoiCongThuc(); DaTinhToan = true; DaToiUuModi = false;
}

void KhoiTaoDSU() { for (int i = 0; i < n + m; i++) { Cha[i] = i; sz[i] = 1; } }
int TimDSU(int i) { if (i == Cha[i]) return i; return Cha[i] = TimDSU(Cha[i]); }
int KtraNoiDSU(int x, int y) {
    x = TimDSU(x); y = TimDSU(y); if (x == y) return 0;
    if (sz[x] > sz[y]) { sz[x] += sz[y]; Cha[y] = x; } else { sz[y] += sz[x]; Cha[x] = y; } return 1;
}
void XuLySuyBien() {
    int SoOBasic = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) { if (PhanPhoi[i][j] != 0) SoOBasic++; }
    }
    if (SoOBasic >= m + n - 1) return;
    KhoiTaoDSU();
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) { if (PhanPhoi[i][j] != 0) KtraNoiDSU(i, j + m); }
    }
    for (int i = 0; i < m && SoOBasic < m + n - 1; i++) {
        for (int j = 0; j < n && SoOBasic < m + n - 1; j++) {
            if (PhanPhoi[i][j] == 0) { if (KtraNoiDSU(i, j + m)) { PhanPhoi[i][j] = -1; SoOBasic++; } }
        }
    }
}
void TimDuongDi(int start, int end, int MangKq[], int SoLuong, int DaTham[], int *SoLuongThat, int *DaTimThay) {
    if (*DaTimThay) return; MangKq[SoLuong] = start;    
    if (start == end) { *DaTimThay = 1; SoLuong++; MangKq[SoLuong] = MangKq[0]; *SoLuongThat = SoLuong + 1; return; }
    DaTham[start] = 1;
    if (start < m) {
        for (int j = 0; j < n; j++) {
            if (m + j == end && SoLuong < 2) continue;
            if (ODaChon[start][j] && !DaTham[m + j]) TimDuongDi(m + j, end, MangKq, SoLuong + 1, DaTham, SoLuongThat, DaTimThay);
        }
    } else {
        for (int i = 0; i < m; i++) {
            if (i == end && SoLuong < 1) continue;
            if (ODaChon[i][start - m] && !DaTham[i]) TimDuongDi(i, end, MangKq, SoLuong + 1, DaTham, SoLuongThat, DaTimThay);
        }
    }
    DaTham[start] = 0;
}

void BienDoi(int MangKq[], int SoLuong) {
    int MIN = INT_MAX; int x, y, xGia = -1, yGia = -1;
    for (int i = 0; i < SoLuong - 1; i++) {
        if (MangKq[i] < MangKq[i + 1]) { x = MangKq[i]; y = MangKq[i + 1] - m; } 
        else { x = MangKq[i + 1]; y = MangKq[i] - m; }
        if (i % 2 == 0) { 
            int val = (PhanPhoi[x][y] == -1) ? 0 : PhanPhoi[x][y]; 
            if (val < MIN) { MIN = val; xGia = x; yGia = y; } 
        }
    }
    for (int i = 0; i < SoLuong - 1; i++) {
        if (MangKq[i] < MangKq[i + 1]) { x = MangKq[i]; y = MangKq[i + 1] - m; } 
        else { x = MangKq[i + 1]; y = MangKq[i] - m; }
        int val = (PhanPhoi[x][y] == -1) ? 0 : PhanPhoi[x][y];
        if (i % 2 == 1) val += MIN; 
        else val -= MIN;            
        if (val == 0) { 
            if (x == xGia && y == yGia) PhanPhoi[x][y] = 0; 
            else PhanPhoi[x][y] = -1; 
        } else { 
            PhanPhoi[x][y] = val; 
        }
    }
}

void PhuongPhapTheVi() {
    if (!DaTinhToan) return; 
    int LapToiDa = 200;
    while (LapToiDa--) {
        for (int i = 0; i < m; i++) { for (int j = 0; j < n; j++) ODaChon[i][j] = (PhanPhoi[i][j] != 0) ? 1 : 0; }
        XuLySuyBien();
        for (int i = 0; i < m; i++) { for (int j = 0; j < n; j++) ODaChon[i][j] = (PhanPhoi[i][j] != 0) ? 1 : 0; }
        
        int u[MAX], v[MAX]; for (int i = 0; i < MAX; i++) { u[i] = INT_MAX; v[i] = INT_MAX; }
        int DaKhoiTaoU = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) { if (ODaChon[i][j]) { u[i] = 0; DaKhoiTaoU = 1; break; } }
            if (DaKhoiTaoU) break;
        }
        int ThayDoi = 1;
        while (ThayDoi) {
            ThayDoi = 0;
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    if (ODaChon[i][j]) {
                        if (u[i] != INT_MAX && v[j] == INT_MAX) { v[j] = ChiPhi[i][j] - u[i]; ThayDoi = 1; } 
                        else if (v[j] != INT_MAX && u[i] == INT_MAX) { u[i] = ChiPhi[i][j] - v[j]; ThayDoi = 1; }
                    }
                }
            }
        }
        int DeltaMin = 0, Hang = -1, Cot = -1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (PhanPhoi[i][j] == 0) {
                    if (u[i] == INT_MAX || v[j] == INT_MAX) continue;
                    int Delta = ChiPhi[i][j] - u[i] - v[j]; 
                    if (Delta < DeltaMin) { DeltaMin = Delta; Hang = i; Cot = j; }
                }
            }
        }
        if (DeltaMin >= 0) break; 
        ODaChon[Hang][Cot] = 1; 
        int MangKq[MAX], DaTham[MAX] = {0}; int SoLuongThat = 0, DaTimThay = 0;
        TimDuongDi(Hang, m + Cot, MangKq, 0, DaTham, &SoLuongThat, &DaTimThay);
        if (DaTimThay) BienDoi(MangKq, SoLuongThat); else break;
    }
    sprintf(TenPhuongPhapHienTai, "%s + Toi uu MODI", TenPhuongPhapGoc);
    TaoChuoiCongThuc(); 
    DaToiUuModi = true;
}

int main(void) {
    const int screenWidth = 1250;
    const int screenHeight = 900; 
    InitWindow(screenWidth, screenHeight, "BAI TOAN VAN TAI");

    const char* ThongBaoMacDinh = "Vui long chon phuong phap tinh toan de ket xuat bang ma tran.";
    const char* ThongBaoNhacNhoModi = "Vui long chon 1 phuong phap co ban truoc khi toi uu MODI!";

    Font winFont = LoadFontEx("arial.ttf", 32, 0, 250); 
    SetTextureFilter(winFont.texture, TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(60);
    bool HienThiCanhBaoModi = false; 

    // Khung giới hạn hiển thị của bảng ma trận (Viewport)
    // Bắt đầu từ x=340 (tránh Sidebar), y=150 (tránh các ô nhập m,n)
    Rectangle viewPort = { 340, 150, (float)(screenWidth - 360), 680 }; 

    // --- CẤU HÌNH CAMERA ĐỂ CUỘN HAI CHIỀU ---
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    // Đặt offset trùng góc trái trên viewport để mốc tọa độ vẽ (0,0) nằm chuẩn xác tại (340, 150)
    camera.offset = (Vector2){ viewPort.x, viewPort.y };
    camera.target = (Vector2){ 0, 0 };

    // Các biến trạng thái để xử lý việc nhấn giữ kéo thanh cuộn bằng chuột
    bool draggingV = false;
    bool draggingH = false;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        
        int hienThiM = (m_goc < 1) ? 1 : m_goc;
        int hienThiN = (n_goc < 1) ? 1 : n_goc;

        // Tọa độ lề tương đối xuất phát từ (0,0) bên trong Camera
        int startX = 10, startY = 15; 
        int cellW = 140, cellH = 55; 
        
        // --- TÍNH TOÁN KÍCH THƯỚC TOÀN BỘ VÙNG DỮ LIỆU THỰC TẾ ---
        float bangA_Height = (hienThiM + 2) * cellH;
        float bangB_Height = DaTinhToan ? ((m + 2) * cellH + 150) : 120;
        
        float totalVirtualWidth = ((((hienThiN > n) ? hienThiN : n) + 3) * cellW) + 50; 
        float totalVirtualHeight = 40 + bangA_Height + 60 + bangB_Height;

        // --- 1. XỬ LÝ LĂN CHUỘT (MOUSE WHEEL) ---
        if (CheckCollisionPointRec(mousePos, viewPort)) {
            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                camera.target.x -= GetMouseWheelMove() * 45;
            } else {
                camera.target.y -= GetMouseWheelMove() * 45;
            }
        }

        // --- 2. TÍNH TOÁN VỊ TRÍ VÀ KÍCH THƯỚC THANH CUỘN (SCROLLBAR) ---
        // Thanh cuộn dọc
        float scrollV_H = (viewPort.height / totalVirtualHeight) * viewPort.height;
        if (scrollV_H < 30) scrollV_H = 30; 
        float scrollV_MaxY = viewPort.y + viewPort.height - scrollV_H;
        float scrollV_Y = viewPort.y + (camera.target.y / (totalVirtualHeight - viewPort.height)) * (viewPort.height - scrollV_H);
        Rectangle scrollVRec = { viewPort.x + viewPort.width + 5, scrollV_Y, 10, scrollV_H };

        // Thanh cuộn ngang
        float scrollH_W = (viewPort.width / totalVirtualWidth) * viewPort.width;
        if (scrollH_W < 30) scrollH_W = 30;
        float scrollH_MaxX = viewPort.x + viewPort.width - scrollH_W;
        float scrollH_X = viewPort.x + (camera.target.x / (totalVirtualWidth - viewPort.width)) * (viewPort.width - scrollH_W);
        Rectangle scrollHRec = { scrollH_X, viewPort.y + viewPort.height + 5, scrollH_W, 10 };

        // --- 3. XỬ LÝ KÉO THÀNH CUỘN BẰNG CHUỘT (DRAG SCROLLBAR) ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, scrollVRec) && totalVirtualHeight > viewPort.height) draggingV = true;
            if (CheckCollisionPointRec(mousePos, scrollHRec) && totalVirtualWidth > viewPort.width) draggingH = true;
        }
        if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
            draggingV = false;
            draggingH = false;
        }

        if (draggingV) {
            float newY = mousePos.y - scrollV_H / 2.0f;
            if (newY < viewPort.y) newY = viewPort.y;
            if (newY > scrollV_MaxY) newY = scrollV_MaxY;
            camera.target.y = ((newY - viewPort.y) / (viewPort.height - scrollV_H)) * (totalVirtualHeight - viewPort.height);
        }
        if (draggingH) {
            float newX = mousePos.x - scrollH_W / 2.0f;
            if (newX < viewPort.x) newX = viewPort.x;
            if (newX > scrollH_MaxX) newX = scrollH_MaxX;
            camera.target.x = ((newX - viewPort.x) / (viewPort.width - scrollH_W)) * (totalVirtualWidth - viewPort.width);
        }

        // Giới hạn Camera không trượt ra ngoài vùng dữ liệu ảo
        if (camera.target.y < 0) camera.target.y = 0;
        if (totalVirtualHeight > viewPort.height) {
            if (camera.target.y > totalVirtualHeight - viewPort.height) camera.target.y = totalVirtualHeight - viewPort.height;
        } else { camera.target.y = 0; }

        if (camera.target.x < 0) camera.target.x = 0;
        if (totalVirtualWidth > viewPort.width) {
            if (camera.target.x > totalVirtualWidth - viewPort.width) camera.target.x = totalVirtualWidth - viewPort.width;
        } else { camera.target.x = 0; }

        // Chuyển đổi tọa độ chuột sang không gian World của Camera (được tính tương đối từ mốc offset mới)
        Vector2 mouseWorldPos = GetScreenToWorld2D(mousePos, camera);

        // Định vị lại các ô cấu hình m, n cố định nằm phía trên bảng ma trận (Không bị cuộn)
        Rectangle rectM = { 340 + 180, 95, 80, 35 };
        Rectangle rectN = { 340 + 460, 95, 80, 35 };

        // --- XỬ LÝ CLICK CHUỘT CHỌN Ô FOCUS ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !draggingV && !draggingH) {
            // Click Sidebar Menu cố định
            if (CheckCollisionPointRec(mousePos, (Rectangle){25, 90, 260, 50})) { DocDuLieuTuFile(); HienThiCanhBaoModi = false; }
            if (CheckCollisionPointRec(mousePos, (Rectangle){25, 180, 260, 46})) { PhuongPhapGocTayBac(); HienThiCanhBaoModi = false; }
            if (CheckCollisionPointRec(mousePos, (Rectangle){25, 240, 260, 46})) { PhuongPhapChiPhiNhoNhat(); HienThiCanhBaoModi = false; }
            if (CheckCollisionPointRec(mousePos, (Rectangle){25, 300, 260, 46})) { PhuongPhapVogel(); HienThiCanhBaoModi = false; }
            if (CheckCollisionPointRec(mousePos, (Rectangle){25, 360, 260, 46})) {
                if (DaTinhToan) { PhuongPhapTheVi(); HienThiCanhBaoModi = false; } 
                else { HienThiCanhBaoModi = true; }
            }
            if (CheckCollisionPointRec(mousePos, (Rectangle){25, 420, 260, 46})) { DatLaiBangVeZero(); HienThiCanhBaoModi = false; }
            
            // Click Nhập m, n cố định
            if (CheckCollisionPointRec(mousePos, rectM)) { FocusLoai = 3; }
            else if (CheckCollisionPointRec(mousePos, rectN)) { FocusLoai = 4; }
            // Click chọn ô trong Bảng (Nhận diện chính xác dựa trên tọa độ camera mới)
            else if (CheckCollisionPointRec(mousePos, viewPort)) {
                for (int i = 0; i < hienThiM; i++) {
                    for (int j = 0; j < hienThiN; j++) {
                        if (CheckCollisionPointRec(mouseWorldPos, (Rectangle){ startX + (j + 1) * cellW, startY + 45 + (i + 1) * cellH, cellW, cellH })) { FocusLoai = 0; FocusR = i; FocusC = j; }
                    }
                }
                for (int i = 0; i < hienThiM; i++) {
                    if (CheckCollisionPointRec(mouseWorldPos, (Rectangle){ startX + (hienThiN + 1) * cellW, startY + 45 + (i + 1) * cellH, cellW, cellH })) { FocusLoai = 1; FocusR = i; FocusC = 0; }
                }
                for (int j = 0; j < hienThiN; j++) {
                    if (CheckCollisionPointRec(mouseWorldPos, (Rectangle){ startX + (j + 1) * cellW, startY + 45 + (hienThiM + 1) * cellH, cellW, cellH })) { FocusLoai = 2; FocusR = 0; FocusC = j; }
                }
            }
        }

        // --- ĐIỀU HƯỚNG MŨI TÊN & PHÍM BẤM ---
        if (IsKeyPressed(KEY_UP)) {
            DaTinhToan = false; DaToiUuModi = false; HienThiCanhBaoModi = false;
            if (FocusLoai == 0 && FocusR > 0) FocusR--;
            else if (FocusLoai == 2) { FocusLoai = 0; FocusR = hienThiM - 1; }
            else if (FocusLoai == 1 && FocusR > 0) FocusR--;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            DaTinhToan = false; DaToiUuModi = false; HienThiCanhBaoModi = false;
            if (FocusLoai == 0) { if (FocusR < hienThiM - 1) FocusR++; else { FocusLoai = 2; FocusR = 0; } }
            else if (FocusLoai == 1 && FocusR < hienThiM - 1) FocusR++;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            DaTinhToan = false; DaToiUuModi = false; HienThiCanhBaoModi = false;
            if (FocusLoai == 0 && FocusC > 0) FocusC--;
            else if (FocusLoai == 1) { FocusLoai = 0; FocusC = hienThiN - 1; }
            else if (FocusLoai == 2 && FocusC > 0) FocusC--;
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            DaTinhToan = false; DaToiUuModi = false; HienThiCanhBaoModi = false;
            if (FocusLoai == 0) { if (FocusC < hienThiN - 1) FocusC++; else { FocusLoai = 1; FocusC = 0; } }
            else if (FocusLoai == 2 && FocusC < n_goc - 1) FocusC++;
        }

        int key = GetCharPressed();
        if (key >= '0' && key <= '9') {
            DaTinhToan = false; DaToiUuModi = false; HienThiCanhBaoModi = false;
            if (FocusLoai == 0) { 
                if (ChiPhiGoc[FocusR][FocusC] == 0) ChiPhiGoc[FocusR][FocusC] = (key - '0');
                else ChiPhiGoc[FocusR][FocusC] = ChiPhiGoc[FocusR][FocusC] * 10 + (key - '0'); 
                if (ChiPhiGoc[FocusR][FocusC] > 999) ChiPhiGoc[FocusR][FocusC] %= 1000; 
            } 
            else if (FocusLoai == 1) { 
                if (CungGoc[FocusR] == 0) CungGoc[FocusR] = (key - '0');
                else CungGoc[FocusR] = CungGoc[FocusR] * 10 + (key - '0'); 
                if (CungGoc[FocusR] > 999) CungGoc[FocusR] %= 1000; 
            } 
            else if (FocusLoai == 2) { 
                if (CauGoc[FocusC] == 0) CauGoc[FocusC] = (key - '0');
                else CauGoc[FocusC] = CauGoc[FocusC] * 10 + (key - '0'); 
                if (CauGoc[FocusC] > 999) CauGoc[FocusC] %= 1000; 
            }
            else if (FocusLoai == 3) {
                if (m_goc == 0) m_goc = (key - '0');
                else m_goc = m_goc * 10 + (key - '0');
                if (m_goc > MAX) m_goc = MAX;
            }
            else if (FocusLoai == 4) {
                if (n_goc == 0) n_goc = (key - '0');
                else n_goc = n_goc * 10 + (key - '0');
                if (n_goc > MAX) n_goc = MAX;
            }
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            DaTinhToan = false; DaToiUuModi = false; HienThiCanhBaoModi = false;
            if (FocusLoai == 0) ChiPhiGoc[FocusR][FocusC] /= 10;
            else if (FocusLoai == 1) CungGoc[FocusR] /= 10;
            else if (FocusLoai == 2) CauGoc[FocusC] /= 10;
            else if (FocusLoai == 3) m_goc /= 10;
            else if (FocusLoai == 4) n_goc /= 10;
        }

        // ==========================================================
        //                         VẼ GIAO DIỆN
        // ==========================================================
        BeginDrawing();
        ClearBackground((Color){ 241, 245, 249, 255 });

        // 1. THANH HEADER TRÊN CÙNG CỐ ĐỊNH
        DrawRectangle(0, 0, screenWidth, 65, (Color){ 15, 23, 42, 255 });
        const char* TieuDeChinh = "BAI TOAN VAN TAI";
        Vector2 textBounds = MeasureTextEx(winFont, TieuDeChinh, 28, 1);
        DrawTextEx(winFont, TieuDeChinh, (Vector2){(screenWidth - textBounds.x) / 2.0f, 18}, 28, 1, WHITE);

        // 2. MENU SIDEBAR TRÁI CỐ ĐỊNH
        DrawRectangle(0, 65, 310, screenHeight - 65, (Color){ 248, 250, 252, 255 });
        DrawLine(310, 65, 310, screenHeight, (Color){ 203, 213, 225, 255 }); 

        DrawRectangleRounded((Rectangle){25, 90, 260, 50}, 0.15f, 4, (Color){ 5, 150, 105, 255 });
        DrawTextEx(winFont, "[Doc du lieu tu File]", (Vector2){35, 105}, 20, 1, WHITE);
        DrawTextEx(winFont, "CHON PHUONG PHAP TINH", (Vector2){30, 160}, 18, 1, (Color){ 100, 116, 139, 255 });

        Color btnColor = (Color){ 30, 64, 175, 255 };
        DrawRectangleRounded((Rectangle){25, 185, 260, 46}, 0.15f, 4, btnColor);
        DrawTextEx(winFont, "1. Goc Tay Bac", (Vector2){38, 196}, 22, 1, WHITE);
        DrawRectangleRounded((Rectangle){25, 245, 260, 46}, 0.15f, 4, btnColor);
        DrawTextEx(winFont, "2. Chi Phi Nho Nhat", (Vector2){38, 256}, 22, 1, WHITE);
        DrawRectangleRounded((Rectangle){25, 305, 260, 46}, 0.15f, 4, btnColor);
        DrawTextEx(winFont, "3. Thuat Toan Vogel", (Vector2){38, 316}, 22, 1, WHITE);

        Color modiBtnColor = DaTinhToan ? (Color){ 180, 83, 9, 255 } : (Color){ 156, 163, 175, 255 };
        DrawRectangleRounded((Rectangle){25, 365, 260, 46}, 0.15f, 4, modiBtnColor);
        DrawTextEx(winFont, "4. Toi Uu Phuong An", (Vector2){38, 376}, 22, 1, WHITE);

        Color resetBtnColor = (Color){ 220, 38, 38, 255 };
        DrawRectangleRounded((Rectangle){25, 425, 260, 46}, 0.15f, 4, resetBtnColor);
        DrawTextEx(winFont, "x Dat lai bang ve 0", (Vector2){45, 436}, 22, 1, WHITE);

        // Ô NHẬP m, n CỐ ĐỊNH PHÍA TRÊN (KHÔNG CUỘN)
        DrawTextEx(winFont, "So luong Cung (m):", (Vector2){ 340, 102 }, 18, 1, BLACK);
        if (FocusLoai == 3) {
            DrawRectangleRec(rectM, (Color){ 254, 240, 138, 255 }); DrawRectangleLinesEx(rectM, 2, (Color){ 217, 119, 6, 255 });
        } else {
            DrawRectangleRec(rectM, WHITE); DrawRectangleLinesEx(rectM, 1, GRAY);
        }
        char strM[10]; sprintf(strM, "%d", m_goc);
        DrawTextEx(winFont, strM, (Vector2){ rectM.x + 25, rectM.y + 8 }, 18, 1, BLACK);

        DrawTextEx(winFont, "So luong Cau (n):", (Vector2){ 340 + 290, 102 }, 18, 1, BLACK);
        if (FocusLoai == 4) {
            DrawRectangleRec(rectN, (Color){ 254, 240, 138, 255 }); DrawRectangleLinesEx(rectN, 2, (Color){ 217, 119, 6, 255 });
        } else {
            DrawRectangleRec(rectN, WHITE); DrawRectangleLinesEx(rectN, 1, GRAY);
        }
        char strN[10]; sprintf(strN, "%d", n_goc);
        DrawTextEx(winFont, strN, (Vector2){ rectN.x + 25, rectN.y + 8 }, 18, 1, BLACK);


        // ==========================================================
        // KHỞI ĐỘNG CHẾ ĐỘ CẮT KHUNG VÀ VẼ CAMERA MẠ TRẬN (A & B)
        // ==========================================================
        BeginScissorMode((int)viewPort.x, (int)viewPort.y, (int)viewPort.width, (int)viewPort.height);
        BeginMode2D(camera);

        char BuffTxt[256];
        // --- VẼ BẢNG MA TRẬN NHẬP LIỆU GỐC (BẢNG A) ---
        DrawTextEx(winFont, "A. MA TRAN BANG NHAP DU LIEU GOC", (Vector2){startX, startY}, 20, 1, (Color){ 15, 23, 42, 255 });

        // Vẽ Header Cột Cầu
        for (int j = 0; j < hienThiN; j++) {
            DrawRectangle(startX + (j + 1) * cellW, startY + 45, cellW, cellH, (Color){ 59, 130, 246, 255 });
            DrawRectangleLinesEx((Rectangle){startX + (j + 1) * cellW, startY + 45, cellW, cellH}, 1, WHITE);
            sprintf(BuffTxt, "Cau %d", j + 1);
            DrawTextEx(winFont, BuffTxt, (Vector2){startX + (j + 1) * cellW + 42, startY + 45 + 18}, 18, 1, WHITE);
        }
        DrawRectangle(startX + (hienThiN + 1) * cellW, startY + 45, cellW, cellH, (Color){ 71, 85, 105, 255 });
        DrawRectangleLinesEx((Rectangle){startX + (hienThiN + 1) * cellW, startY + 45, cellW, cellH}, 1, WHITE);
        DrawTextEx(winFont, "Cung", (Vector2){startX + (hienThiN + 1) * cellW + 48, startY + 45 + 18}, 18, 1, WHITE);

        // Vẽ Nội Dung Hàng Bảng A
        for (int i = 0; i < hienThiM; i++) {
            DrawRectangle(startX, startY + 45 + (i + 1) * cellH, cellW, cellH, (Color){ 59, 130, 246, 255 });
            DrawRectangleLinesEx((Rectangle){startX, startY + 45 + (i + 1) * cellH, cellW, cellH}, 1, WHITE);
            sprintf(BuffTxt, "Cung %d", i + 1);
            DrawTextEx(winFont, BuffTxt, (Vector2){startX + 38, startY + 45 + (i + 1) * cellH + 18}, 18, 1, WHITE);

            for (int j = 0; j < hienThiN; j++) {
                Rectangle cellRec = { startX + (j + 1) * cellW, startY + 45 + (i + 1) * cellH, cellW, cellH };
                if (FocusLoai == 0 && FocusR == i && FocusC == j) {
                    DrawRectangleRec(cellRec, (Color){ 254, 240, 138, 255 }); DrawRectangleLinesEx(cellRec, 2.5f, (Color){ 217, 119, 6, 255 });
                } else {
                    DrawRectangleRec(cellRec, WHITE); DrawRectangleLinesEx(cellRec, 1, (Color){ 203, 213, 225, 255 });
                }
                sprintf(BuffTxt, "%d", ChiPhiGoc[i][j]);
                DrawTextEx(winFont, BuffTxt, (Vector2){startX + (j + 1) * cellW + 60, startY + 45 + (i + 1) * cellH + 18}, 20, 1, (Color){ 15, 23, 42, 255 });
            }
            
            Rectangle cungRec = { startX + (hienThiN + 1) * cellW, startY + 45 + (i + 1) * cellH, cellW, cellH };
            if (FocusLoai == 1 && FocusR == i) {
                DrawRectangleRec(cungRec, (Color){ 254, 240, 138, 255 }); DrawRectangleLinesEx(cungRec, 2.5f, (Color){ 217, 119, 6, 255 });
            } else {
                DrawRectangleRec(cungRec, WHITE); DrawRectangleLinesEx(cungRec, 1, (Color){ 203, 213, 225, 255 });
            }
            sprintf(BuffTxt, "%d", CungGoc[i]);
            DrawTextEx(winFont, BuffTxt, (Vector2){startX + (hienThiN + 1) * cellW + 62, startY + 45 + (i + 1) * cellH + 18}, 20, 1, (Color){ 180, 83, 9, 255 });
        }

        // Hàng Cầu Bảng A
        DrawRectangle(startX, startY + 45 + (hienThiM + 1) * cellH, cellW, cellH, (Color){ 71, 85, 105, 255 });
        DrawRectangleLinesEx((Rectangle){startX, startY + 45 + (hienThiM + 1) * cellH, cellW, cellH}, 1, WHITE);
        DrawTextEx(winFont, "Cau", (Vector2){startX + 50, startY + 45 + (hienThiM + 1) * cellH + 18}, 18, 1, WHITE);
        for (int j = 0; j < hienThiN; j++) {
            Rectangle cauRec = { startX + (j + 1) * cellW, startY + 45 + (hienThiM + 1) * cellH, cellW, cellH };
            if (FocusLoai == 2 && FocusC == j) {
                DrawRectangleRec(cauRec, (Color){ 254, 240, 138, 255 }); DrawRectangleLinesEx(cauRec, 2.5f, (Color){ 217, 119, 6, 255 });
            } else {
                DrawRectangleRec(cauRec, WHITE); DrawRectangleLinesEx(cauRec, 1, (Color){ 203, 213, 225, 255 });
            }
            sprintf(BuffTxt, "%d", CauGoc[j]);
            DrawTextEx(winFont, BuffTxt, (Vector2){startX + (j + 1) * cellW + 62, startY + 45 + (hienThiM + 1) * cellH + 18}, 20, 1, (Color){ 180, 83, 9, 255 });
        }


        // --- VẼ BẢNG KẾT QUẢ PHÂN PHỐI VẬN TẢI (BẢNG B) ---
        int outX = startX, outY = startY + 45 + (hienThiM + 2) * cellH + 40; 
        DrawTextEx(winFont, "B. PHUONG AN PHAN PHOI VA KET QUA", (Vector2){outX, outY}, 20, 1, (Color){ 15, 23, 42, 255 });

        if (DaTinhToan) {
            for (int j = 0; j < n; j++) {
                bool laCotGia = (KhongCanBang == 1 && j == n - 1);
                Color cHeader = laCotGia ? (Color){ 100, 116, 139, 255 } : (Color){ 29, 78, 216, 255 };
                DrawRectangle(outX + (j + 1) * cellW, outY + 30, cellW, cellH, cHeader);
                DrawRectangleLinesEx((Rectangle){outX + (j + 1) * cellW, outY + 30, cellW, cellH}, 1, WHITE);
                if (laCotGia) DrawTextEx(winFont, "Cau Gia", (Vector2){outX + (j + 1) * cellW + 38, outY + 30 + 18}, 18, 1, WHITE);
                else {
                    sprintf(BuffTxt, "Cau %d", j + 1);
                    DrawTextEx(winFont, BuffTxt, (Vector2){outX + (j + 1) * cellW + 45, outY + 30 + 18}, 18, 1, WHITE);
                }
            }
            DrawRectangle(outX + (n + 1) * cellW, outY + 30, cellW, cellH, (Color){ 30, 41, 59, 255 });
            DrawRectangleLinesEx((Rectangle){outX + (n + 1) * cellW, outY + 30, cellW, cellH}, 1, WHITE);
            DrawTextEx(winFont, "Cung", (Vector2){outX + (n + 1) * cellW + 48, outY + 30 + 18}, 18, 1, WHITE);

            for (int i = 0; i < m; i++) {
                bool laHangGia = (KhongCanBang == 2 && i == m - 1);
                Color rHeader = laHangGia ? (Color){ 100, 116, 139, 255 } : (Color){ 30, 41, 59, 255 };
                DrawRectangle(outX, outY + 30 + (i + 1) * cellH, cellW, cellH, rHeader);
                DrawRectangleLinesEx((Rectangle){outX, outY + 30 + (i + 1) * cellH, cellW, cellH}, 1, WHITE);
                if (laHangGia) DrawTextEx(winFont, "Cung Gia", (Vector2){outX + 34, outY + 30 + (i + 1) * cellH + 18}, 18, 1, WHITE);
                else {
                    sprintf(BuffTxt, "Cung %d", i + 1);
                    DrawTextEx(winFont, BuffTxt, (Vector2){outX + 38, outY + 30 + (i + 1) * cellH + 18}, 18, 1, WHITE);
                }

                for (int j = 0; j < n; j++) {
                    Rectangle outCellRec = { outX + (j + 1) * cellW, outY + 30 + (i + 1) * cellH, cellW, cellH };
                    DrawRectangleRec(outCellRec, WHITE); DrawRectangleLinesEx(outCellRec, 1, (Color){ 218, 226, 235, 255 });
                    
                    sprintf(BuffTxt, "%d", ChiPhi[i][j]);
                    DrawTextEx(winFont, BuffTxt, (Vector2){outX + (j + 1) * cellW + 12, outY + 30 + (i + 1) * cellH + 8}, 18, 1, (Color){ 148, 163, 184, 255 });

                    if (PhanPhoi[i][j] == -1) {
                        DrawTextEx(winFont, "e", (Vector2){outX + (j + 1) * cellW + 65, outY + 30 + (i + 1) * cellH + 16}, 20, 1, RED);
                    } else if (PhanPhoi[i][j] > 0) {
                        sprintf(BuffTxt, "%d", PhanPhoi[i][j]);
                        DrawTextEx(winFont, BuffTxt, (Vector2){outX + (j + 1) * cellW + 60, outY + 30 + (i + 1) * cellH + 16}, 20, 1, (Color){ 5, 150, 105, 255 });
                    } else {
                        DrawTextEx(winFont, "-", (Vector2){outX + (j + 1) * cellW + 65, outY + 30 + (i + 1) * cellH + 18}, 20, 1, LIGHTGRAY);
                    }
                }
                Rectangle outCungRec = { outX + (n + 1) * cellW, outY + 30 + (i + 1) * cellH, cellW, cellH };
                DrawRectangleRec(outCungRec, (Color){ 248, 250, 252, 255 }); DrawRectangleLinesEx(outCungRec, 1, (Color){ 71, 85, 105, 255 });
                sprintf(BuffTxt, "%d", Cung[i]);
                DrawTextEx(winFont, BuffTxt, (Vector2){outX + (n + 1) * cellW + 62, outY + 30 + (i + 1) * cellH + 18}, 20, 1, BLACK);
            }

            DrawRectangle(outX, outY + 30 + (m + 1) * cellH, cellW, cellH, (Color){ 71, 85, 105, 255 });
            DrawRectangleLinesEx((Rectangle){outX, outY + 30 + (m + 1) * cellH, cellW, cellH}, 1, WHITE);
            DrawTextEx(winFont, "Cau", (Vector2){outX + 50, outY + 30 + (m + 1) * cellH + 18}, 18, 1, WHITE);
            for (int j = 0; j < n; j++) {
                Rectangle outCauRec = { outX + (j + 1) * cellW, outY + 30 + (m + 1) * cellH, cellW, cellH };
                DrawRectangleRec(outCauRec, (Color){ 248, 250, 252, 255 }); DrawRectangleLinesEx(outCauRec, 1, (Color){ 71, 85, 105, 255 });
                sprintf(BuffTxt, "%d", Cau[j]);
                DrawTextEx(winFont, BuffTxt, (Vector2){outX + (j + 1) * cellW + 62, outY + 30 + (m + 1) * cellH + 18}, 20, 1, BLACK);
            }

            // Khung chuỗi kết quả dưới đáy bảng B
            int boxX = outX; int boxY = outY + 30 + (m + 2.5f) * cellH;
            DrawRectangleRounded((Rectangle){boxX, boxY, 870, 100}, 0.05f, 4, WHITE);
            DrawRectangleRoundedLinesEx((Rectangle){boxX, boxY, 870, 100}, 0.05f, 4, 1.5f, (Color){ 16, 185, 129, 255 });
            
            sprintf(BuffTxt, "Trang thai: Hoan thanh tinh toan [%s]", TenPhuongPhapHienTai);
            DrawTextEx(winFont, BuffTxt, (Vector2){boxX + 25, boxY + 18}, 20, 1, (Color){ 30, 41, 59, 255 });
            DrawTextEx(winFont, ChuoiCongThucChiTiet, (Vector2){boxX + 25, boxY + 52}, 22, 1, (Color){ 220, 38, 38, 255 });

        } else {
            Rectangle emptyRec = { outX, outY + 30, 700, 90 };
            DrawRectangleRec(emptyRec, WHITE); DrawRectangleLinesEx(emptyRec, 1, (Color){ 203, 213, 225, 255 });
            if (HienThiCanhBaoModi) {
                DrawTextEx(winFont, ThongBaoNhacNhoModi, (Vector2){outX + 25, outY + 30 + 35}, 18, 1, (Color){ 217, 119, 6, 255 });
            } else {
                DrawTextEx(winFont, ThongBaoMacDinh, (Vector2){outX + 25, outY + 30 + 35}, 18, 1, (Color){ 100, 116, 139, 255 });
            }
        }

        EndMode2D();
        EndScissorMode(); // --- KẾT THÚC KHUNG CẮT ---

        // 4. VẼ THANH TRƯỢT TRỰC QUAN (SCROLLBAR) BÊN NGOÀI KHUNG NHÌN CHÍNH
        if (totalVirtualHeight > viewPort.height) {
            DrawRectangleRec((Rectangle){ viewPort.x + viewPort.width + 5, viewPort.y, 10, viewPort.height }, (Color){ 226, 232, 240, 255 }); 
            DrawRectangleRounded(scrollVRec, 0.4f, 4, (draggingV ? DARKGRAY : GRAY));
        }
        if (totalVirtualWidth > viewPort.width) {
            DrawRectangleRec((Rectangle){ viewPort.x, viewPort.y + viewPort.height + 5, viewPort.width, 10 }, (Color){ 226, 232, 240, 255 }); 
            DrawRectangleRounded(scrollHRec, 0.4f, 4, (draggingH ? DARKGRAY : GRAY));
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}