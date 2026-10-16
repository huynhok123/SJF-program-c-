#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <climits>

using namespace std;

struct Process {
    string id;
    int index;      // Thứ tự nhập ban đầu
    int at;         // Arrival Time (Thời điểm đến)
    int bt;         // Burst Time (Thời gian thực thi)
    int ct = 0;     // Completion Time (Thời điểm hoàn thành)
    int tat = 0;    // Turnaround Time (TAT = CT - AT)
    int wt = 0;     // Waiting Time (WT = TAT - BT)
    bool completed = false;
};

struct GanttBlock {
    string label;
    int start_time;
    int end_time;
};

int main() {
    int n;
    cout << "Nhap so luong tien trinh: ";
    if (!(cin >> n) || n <= 0) return 0;

    vector<Process> p(n);
    for (int i = 0; i < n; ++i) {
        p[i].id = "P" + to_string(i + 1);
        p[i].index = i;
        cout << "Nhap AT va BT cua " << p[i].id << ": ";
        cin >> p[i].at >> p[i].bt;
    }

    int completed_count = 0;
    int current_time = 0;
    double total_tat = 0;
    double total_wt = 0;
    vector<GanttBlock> gantt;

    while (completed_count < n) {
        int selected_idx = -1;

        // Tìm tiến trình đã đến (AT <= current_time) có BT nhỏ nhất
        for (int i = 0; i < n; ++i) {
            if (!p[i].completed && p[i].at <= current_time) {
                if (selected_idx == -1) {
                    selected_idx = i;
                } else {
                    // Quy tắc ưu tiên: 1. BT nhỏ hơn -> 2. AT nhỏ hơn -> 3. Thứ tự nhập nhỏ hơn
                    if (p[i].bt < p[selected_idx].bt) {
                        selected_idx = i;
                    } else if (p[i].bt == p[selected_idx].bt) {
                        if (p[i].at < p[selected_idx].at) {
                            selected_idx = i;
                        } else if (p[i].at == p[selected_idx].at && p[i].index < p[selected_idx].index) {
                            selected_idx = i;
                        }
                    }
                }
            }
        }

        // Nếu chưa có tiến trình nào đến, CPU ở trạng thái chờ (IDLE)
        if (selected_idx == -1) {
            int next_at = INT_MAX;
            for (int i = 0; i < n; ++i) {
                if (!p[i].completed && p[i].at < next_at) {
                    next_at = p[i].at;
                }
            }
            gantt.push_back({"IDLE", current_time, next_at});
            current_time = next_at;
        } else {
            // Cho tiến trình chạy đến khi hoàn thành (Non-preemptive SJF)
            int start = current_time;
            current_time += p[selected_idx].bt;
            int end = current_time;

            p[selected_idx].ct = end;
            p[selected_idx].tat = p[selected_idx].ct - p[selected_idx].at;
            p[selected_idx].wt = p[selected_idx].tat - p[selected_idx].bt;
            p[selected_idx].completed = true;

            total_tat += p[selected_idx].tat;
            total_wt += p[selected_idx].wt;
            completed_count++;

            gantt.push_back({p[selected_idx].id, start, end});
        }
    }

    // 1. In Sơ đồ Gantt
    cout << "\n================ SO DO GANTT ================\n";
    cout << "CPU: |";
    for (const auto& block : gantt) {
        cout << "----" << block.label << "----|";
    }
    cout << "\n     " << gantt[0].start_time;
    for (const auto& block : gantt) {
        cout << setw(11) << block.end_time;
    }
    cout << "\n";

    // 2. In Bảng kết quả chi tiết
    cout << "\n================ BANG KET QUA SJF ================\n";
    cout << left << setw(12) << "Tien trinh"
         << setw(8)  << "AT"
         << setw(8)  << "BT"
         << setw(8)  << "CT"
         << setw(10) << "TAT"
         << setw(10) << "WT" << "\n";
    cout << string(56, '-') << "\n";

    for (int i = 0; i < n; ++i) {
        cout << left << setw(12) << p[i].id
             << setw(8)  << p[i].at
             << setw(8)  << p[i].bt
             << setw(8)  << p[i].ct
             << setw(10) << p[i].tat
             << setw(10) << p[i].wt << "\n";
    }
    cout << string(56, '-') << "\n";

    // 3. In Giá trị trung bình
    cout << fixed << setprecision(2);
    cout << "Thoi gian luu lai he thong trung binh (Average TAT): " << (total_tat / n) << " ms\n";
    cout << "Thoi gian cho trung binh (Average WT)              : " << (total_wt / n) << " ms\n";

    return 0;
}