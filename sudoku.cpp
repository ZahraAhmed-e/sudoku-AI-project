#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Sudoku {
private:
    int grid[9][9];
    int original[9][9]; // لتخزين اللوحة قبل المقارنة

public:
    // إحصائيات
    int stepsDFS = 0, backtracksDFS = 0;
    int stepsImproved = 0, backtracksImproved = 0;
    int stepsMinConflicts = 0;

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
        cout << " Using DFS Backtracking (Basic & Improved) & Min-Conflicts\n";
        cout << "======================================\n\n";

        for (int r = 0; r < 9; r++) {
            if (r % 3 == 0)
                cout << "-----------------------------------------\n";
            for (int c = 0; c < 9; c++) {
                if (c % 3 == 0) cout << "| ";
                if (grid[r][c] == 0) cout << ". ";
                else cout << grid[r][c] << " ";
            }
            cout << "|\n";
        }
        cout << "-----------------------------------------\n\n";
    }

    // ============================
    // إدخال اللوحة
    // ============================
    void readFromUser() {
        cout << "أدخل لوحة سودوكو (0 = خانة فارغة):\n";
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                cin >> grid[r][c];
    }

    // ============================
    // فحص القواعد
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
    bool findEmpty(int& row, int& col) const {
        for (row = 0; row < 9; row++)
            for (col = 0; col < 9; col++)
                if (grid[row][col] == 0) return true;
        return false;
    }

    // ============================
    // DFS + Backtracking
    // ============================
    bool solveWithStats() {
        int row, col;
        if (!findEmpty(row, col)) return true;

        for (int num = 1; num <= 9; num++) {
            stepsDFS++;
            if (isSafe(row, col, num)) {
                grid[row][col] = num;
                if (solveWithStats()) return true;
                grid[row][col] = 0;
                backtracksDFS++;
            }
        }
        return false;
    }

    // ============================
    // MRV + LCV + Forward Checking
    // ============================
    vector<int> getPossibleValues(int row, int col) const {
        vector<int> vals;
        if (grid[row][col] != 0) return vals;
        for (int i = 1; i <= 9; i++)
            if (isSafe(row, col, i))
                vals.push_back(i);
        return vals;
    }

    bool findMRVCell(int& row, int& col) const {
        int minCount = 10;
        bool found = false;
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                if (grid[r][c] == 0) {
                    int size = getPossibleValues(r, c).size();
                    if (size == 0) return false;
                    if (size < minCount) {
                        minCount = size;
                        row = r;
                        col = c;
                        found = true;
                    }
                }
            }
        }
        return found;
    }

    vector<int> getLCVOrder(int row, int col) const {
        vector<int> vals = getPossibleValues(row, col);
        sort(vals.begin(), vals.end(), [&](int a, int b) {
            int ca = 0, cb = 0;
            for (int r = 0; r < 9; r++)
                for (int c = 0; c < 9; c++)
                    if (grid[r][c] == 0) {
                        if (isSafe(r, c, a)) ca++;
                        if (isSafe(r, c, b)) cb++;
                    }
            return ca > cb;
            });
        return vals;
    }

    bool forwardCheck() const {
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                if (grid[r][c] == 0 && getPossibleValues(r, c).empty())
                    return false;
        return true;
    }

    bool solveImprovedWithStats() {
        int row, col;
        if (!findMRVCell(row, col)) return true;

        for (int num : getLCVOrder(row, col)) {
            stepsImproved++;
            grid[row][col] = num;
            if (forwardCheck() && solveImprovedWithStats()) return true;
            grid[row][col] = 0;
            backtracksImproved++;
        }
        return false;
    }

    // ============================
    // Min-Conflicts
    // ============================
    bool solveMinConflicts(int maxSteps = 10000) {
        vector<pair<int, int>> emptyCells;
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                if (grid[r][c] == 0)
                    emptyCells.push_back({ r,c });

        // ملأ عشوائي
        for (auto& cell : emptyCells) {
            int r = cell.first;
            int c = cell.second;
            vector<int> vals = getPossibleValues(r, c);
            if (vals.empty()) vals = { 1,2,3,4,5,6,7,8,9 };
            grid[r][c] = vals[rand() % vals.size()];
        }

        // الخطوات
        for (int step = 0; step < maxSteps; step++) {
            stepsMinConflicts++;
            vector<pair<int, int>> conflicts;
            for (auto& cell : emptyCells) {
                int r = cell.first;
                int c = cell.second;
                int conf = 0;
                for (int i = 0; i < 9; i++) {
                    if (i != c && grid[r][i] == grid[r][c]) conf++;
                    if (i != r && grid[i][c] == grid[r][c]) conf++;
                }
                int sr = (r / 3) * 3, sc = (c / 3) * 3;
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        if ((sr + i != r || sc + j != c) && grid[sr + i][sc + j] == grid[r][c])
                            conf++;
                if (conf > 0) conflicts.push_back({ r,c });
            }

            if (conflicts.empty()) return true;

            auto cell = conflicts[rand() % conflicts.size()];
            int r = cell.first, c = cell.second;

            int bestVal = grid[r][c];
            int minConf = 100;
            for (int val = 1; val <= 9; val++) {
                grid[r][c] = val;
                int conf = 0;
                for (int i = 0; i < 9; i++) {
                    if (i != c && grid[r][i] == val) conf++;
                    if (i != r && grid[i][c] == val) conf++;
                }
                int sr = (r / 3) * 3, sc = (c / 3) * 3;
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        if ((sr + i != r || sc + j != c) && grid[sr + i][sc + j] == val)
                            conf++;
                if (conf < minConf) {
                    minConf = conf;
                    bestVal = val;
                }
            }
            grid[r][c] = bestVal;
        }
        return false;
    }

    // ============================
    // توليد لوحة عشوائية
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
        for (int i = 0; i < 9; i++) {
            int r = rand() % 9;
            swap(numbers[i], numbers[r]);
        }
        for (int i = 0; i < 9; i++) {
            if (isSafe(row, col, numbers[i])) {
                grid[row][col] = numbers[i];
                if (fillRecursive(nextRow, nextCol, numbers)) return true;
                grid[row][col] = 0;
            }
        }
        return false;
    }

    void removeCells(int count) {
        while (count > 0) {
            int r = rand() % 9;
            int c = rand() % 9;
            if (grid[r][c] != 0) {
                grid[r][c] = 0;
                count--;
            }
        }
    }

    void generateRandom() {
        fillComplete();
        removeCells(40);
    }

    // ============================
    // نسخ واسترجاع اللوحة الأصلية
    // ============================
    void copyOriginal() {
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                original[r][c] = grid[r][c];
    }

    void restoreOriginal() {
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                grid[r][c] = original[r][c];
    }

    // ============================
    // مقارنة الخوارزميات
    // ============================
    void compareSolvers() {
        copyOriginal();

        stepsDFS = backtracksDFS = 0;
        auto start = high_resolution_clock::now();
        bool solvedDFS = solveWithStats();
        auto end = high_resolution_clock::now();
        auto timeDFS = duration_cast<milliseconds>(end - start).count();

        cout << "\nDFS العادي:\n"
            << "  حل = " << (solvedDFS ? "نعم" : "لا") << "\n"
            << "  عدد العقد = " << stepsDFS << "\n"
            << "  عدد التراجعات = " << backtracksDFS << "\n"
            << "  وقت الحل = " << timeDFS << " ms\n";

        restoreOriginal();

        copyOriginal();
        stepsImproved = backtracksImproved = 0;
        start = high_resolution_clock::now();
        bool solvedImp = solveImprovedWithStats();
        end = high_resolution_clock::now();
        auto timeImp = duration_cast<milliseconds>(end - start).count();

        cout << "\nDFS محسّن (MRV + LCV + FC):\n"
            << "  حل = " << (solvedImp ? "نعم" : "لا") << "\n"
            << "  عدد العقد = " << stepsImproved << "\n"
            << "  عدد التراجعات = " << backtracksImproved << "\n"
            << "  وقت الحل = " << timeImp << " ms\n";

        restoreOriginal();

        copyOriginal();
        stepsMinConflicts = 0;
        start = high_resolution_clock::now();
        bool solvedMC = solveMinConflicts();
        end = high_resolution_clock::now();
        auto timeMC = duration_cast<milliseconds>(end - start).count();

        cout << "\nMin-Conflicts:\n"
            << "  حل = " << (solvedMC ? "نعم" : "لا") << "\n"
            << "  عدد الخطوات = " << stepsMinConflicts << "\n"
            << "  وقت الحل = " << timeMC << " ms\n";
    }
};

// ============================
// main
// ============================
int main() {
    Sudoku s;
    int choice;

    while (true) {
        s.printUI();
        cout << "1) إدخال لوحة سودوكو\n";
        cout << "2) حل DFS عادي\n";
        cout << "3) مسح اللوحة\n";
        cout << "4) خروج\n";
        cout << "5) توليد لوحة عشوائية\n";
        cout << "6) حل محسّن (MRV + LCV + FC)\n";
        cout << "7) حل Min-Conflicts\n";
        cout << "8) مقارنة الخوارزميات\n";
        cout << "اختر: ";
        cin >> choice;

        if (choice == 1) s.readFromUser();
        else if (choice == 2) s.solveWithStats();
        else if (choice == 3) s = Sudoku();
        else if (choice == 4) break;
        else if (choice == 5) s.generateRandom();
        else if (choice == 6) s.solveImprovedWithStats();
        else if (choice == 7) s.solveMinConflicts();
        else if (choice == 8) s.compareSolvers();
    }

    return 0;
}
