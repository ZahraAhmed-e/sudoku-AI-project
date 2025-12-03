Zahra El, [02/12/2025 07:50 م]
#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

class Sudoku {
private:
    int grid[9][9]; // اللوحة

public:
    Sudoku() {
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                grid[r][c] = 0;
    }

    // ============================
    // طباعة اللوحة
    // ============================
    void printUI() const {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        cout << "\n=========== SUDOKU SOLVER ===========\n";
        cout << "         Using DFS Backtracking\n";
        cout << "======================================\n\n";

        for (int r = 0; r < 9; r++) {
            if (r % 3 == 0)
                cout << "-----------------------------------------\n";

            for (int c = 0; c < 9; c++) {

                if (c % 3 == 0)
                    cout << "| ";

                if (grid[r][c] == 0)
                    cout << ". ";
                else
                    cout << grid[r][c] << " ";
            }

            cout << "|\n";
        }

        cout << "-----------------------------------------\n\n";
    }

    // ============================
    // قراءة لوحة من المستخدم
    // ============================
    // void readFromUser() {
    //     cout << "أدخل لوحة سودوكو (0 = خانة فارغة):\n";
    //     for (int r = 0; r < 9; r++)
    //         for (int c = 0; c < 9; c++)
    //             cin >> grid[r][c];
    // }
void readFromUser() {
    cout << "أدخل لوحة سودوكو (0 = خانة فارغة، الأرقام من 1 إلى 9 فقط):\n";
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int val;
            while (true) {
                cout << "الخانة [" << r + 1 << "][" << c + 1 << "]: ";
                cin >> val;

                // تحقق من أن الرقم بين 0 و 9
                if (val < 0 || val > 9) {
                    cout << "❌ الرقم يجب أن يكون بين 0 و 9. حاول مرة أخرى.\n";
                    continue;
                }

                // إذا الرقم ليس صفر، تحقق من الصف والعمود والصندوق
                if (val != 0 && !isSafe(r, c, val)) {
                    cout << "❌ الرقم " << val << " موجود بالفعل في الصف أو العمود أو الصندوق. حاول رقم آخر.\n";
                    continue;
                }

                break; // الرقم صحيح، اكسر الحلقة
            }
            grid[r][c] = val;
        }
    }
}

    // ============================
    // التحقق من الصف / العمود / الصندوق
    // ============================
    bool checkRow(int row, int val) const {
        for (int c = 0; c < 9; c++)
            if (grid[row][c] == val) return false;
        return true;
    }

    bool checkCol(int col, int val) const {
        for (int r = 0; r < 9; r++)
            if (grid[r][col] == val) return false;
        return true;
    }

    bool checkBox(int row, int col, int val) const {
        int sr = (row / 3) * 3;
        int sc = (col / 3) * 3;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (grid[sr + r][sc + c] == val) return false;
        return true;
    }

    bool isSafe(int row, int col, int val) const {
        return checkRow(row, val) && checkCol(col, val) && checkBox(row, col, val);
    }

    // ============================
    // البحث عن أول خانة فارغة
    // ============================
    bool findEmpty(int &row, int &col) const {
        for (row = 0; row < 9; row++)
            for (col = 0; col < 9; col++)
                if (grid[row][col] == 0) return true;
        return false;
    }

    // ============================
    // حل السودوكو Backtracking
    // ============================
    bool solve() {
        int row, col;
        if (!findEmpty(row, col)) return true; // انتهى

        for (int num = 1; num <= 9; num++) {
            if (isSafe(row, col, num)) {
                grid[row][col] = num;
                if (solve()) return true;
                grid[row][col] = 0; // backtrack
            }
        }
        return false;
    }

    // ============================
    // توليد لوحة كاملة عشوائية
    // ============================
    void fillComplete() {
        int numbers[9];
        for (int i = 0; i < 9; i++) numbers[i] = i + 1;
        srand(time(0));
        fillRecursive(0, 0, numbers);
    }

    bool fillRecursive(int row, int col, int numbers[9]) {
        if (row == 9) return true;

        int nextRow = (col == 8) ? row + 1 : row;
        int nextCol = (col + 1) % 9;

        // shuffle
        for (int i = 0; i < 9; i++) {
            int r = rand() % 9;
            swap(numbers[i], numbers[r]);
        }

        for (int i = 0; i < 9; i++) {
            int num = numbers[i];
            if (isSafe(row, col, num)) {
                grid[row][col] = num;
                if (fillRecursive(nextRow, nextCol, numbers)) return true;
                grid[row][col] = 0;
            }
        }
        return false;
    }

    // ============================

Zahra El, [02/12/2025 07:50 م]
// حذف بعض الأرقام لإنتاج فراغات
    // ============================
    void removeCells(int count) {
        srand(time(0));
        while (count > 0) {
            int r = rand() % 9;
            int c = rand() % 9;
            if (grid[r][c] != 0) {
                grid[r][c] = 0;
                count--;
            }
        }
    }

    // ============================
    // توليد لوحة عشوائية جاهزة للحل
    // ============================
    void generateRandom() {
        fillComplete();
        removeCells(40); // يمكنك تعديل العدد حسب الصعوبة
    }
};

int main() {
    Sudoku s;
    int choice;

    while (true) {
        s.printUI();
        cout << "1) إدخال لوحة سودوكو\n";
        cout << "2) حل السودوكو (DFS)\n";
        cout << "3) مسح اللوحة\n";
        cout << "4) خروج\n";
        cout << "5) توليد لوحة عشوائية\n\n";

        cout << "اختر: ";
        cin >> choice;

        if (choice == 1) {
            s.readFromUser();
        }
        else if (choice == 2) {
            cout << "\nجاري الحل...\n\n";
            if (s.solve()) {
                cout << " تم إيجاد الحل بنجاح!\n\n";
                s.printUI();
            }
            else {
                cout << " لا يوجد حل لهذه اللوحة!\n";
            }

#ifdef _WIN32
            system("pause");
#else
            cout << "اضغط ENTER للاستمرار...";
            cin.ignore();
            cin.get();
#endif
        }
        else if (choice == 3) {
            s = Sudoku(); // Reset
        }
        else if (choice == 4) {
            break;
        }
        else if (choice == 5) {
            s.generateRandom();
            cout << "تم توليد لوحة عشوائية قابلة للحل.\n";
#ifdef _WIN32
            system("pause");
#else
            cout << "اضغط ENTER للاستمرار...";
            cin.ignore();
            cin.get();
#endif
        }
    }

    return 0;

}
