import numpy as np
import pandas as pd

SAMPLE_COUNT = 1000        # Số chu kỳ cần đo
PERIOD_MS_NOMINAL = 19.0   # Chu kỳ danh định theo đề bài (ms)
CHANNEL_INDEX = 1          # Cột 0 là microseconds, Cột 1 là kênh D0 (PA6)

def analyze_ex3_csv(csv_path):
    # Đọc file CSV
    df = pd.read_csv(csv_path)

    # 1. Chuyển đổi thời gian từ micro-giây (us) sang mili-giây (ms)
    time_ms = df.iloc[:, 0].values.astype(float) / 1000.0
    signal = df.iloc[:, CHANNEL_INDEX].values.astype(int)

    # 2. Bắt tất cả các vị trí có sườn lên (0 -> 1)
    rising_indices = np.where((signal[:-1] == 0) & (signal[1:] == 1))[0] + 1
    t_edges = time_ms[rising_indices]

    # Kiểm tra số lượng sườn xung bắt được
    # Cần bỏ qua 2 xung đầu do khởi động nên cần ít nhất SAMPLE_COUNT + 3 sườn
    if len(t_edges) < SAMPLE_COUNT + 3:
        print(f"Cảnh báo: Không đủ sườn xung! Bắt được: {len(t_edges)}, cần tối thiểu: {SAMPLE_COUNT + 3}")
        t_clean = t_edges[1:] if len(t_edges) > 1 else t_edges
        n_cycles = len(t_clean) - 1
    else:
        # Bỏ 2 sườn đầu, lấy đúng 1001 sườn liên tiếp (tạo thành 1000 chu kỳ)
        t_clean = t_edges[2 : SAMPLE_COUNT + 3]
        n_cycles = SAMPLE_COUNT

    # 3. Tính độ dài từng chu kỳ (T_i = t_{i+1} - t_i) bằng mili-giây (ms)
    periods = np.diff(t_clean)

    # 4. Thống kê số liệu
    t_min = np.min(periods)
    t_max = np.max(periods)
    t_avg = np.mean(periods)
    jitter_us = (t_max - t_min) * 1000.0  # Đổi từ ms sang us

    # Drift tổng sau n_cycles chu kỳ (ms)
    t_actual_total = t_clean[-1] - t_clean[0]
    t_nominal_total = n_cycles * PERIOD_MS_NOMINAL
    drift_total_ms = t_actual_total - t_nominal_total

    print(f"=== KẾT QUẢ CHO {csv_path} ({n_cycles} chu kỳ) ===")
    print(f"min (ms):                {t_min:.4f}")
    print(f"max (ms):                {t_max:.4f}")
    print(f"tb (ms):                 {t_avg:.4f}")
    print(f"jitter = max-min (us):   {jitter_us:.2f}")
    print(f"drift tổng (ms):         {drift_total_ms:.2f}\n")

if __name__ == "__main__":
    
    analyze_ex3_csv(r"d:\RTOS\intern-261-rtos-course-easy\week1\ex3\data4.csv")