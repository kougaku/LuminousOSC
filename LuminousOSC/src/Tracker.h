#pragma once

#include "TrackingPoint.h"
#include <iostream>
#include <vector>

using namespace std;

class Tracker {

private:
    vector<TrackingPoint> points;
    int max_points = 5;
    int max_life = 10;
    int near_distance = 50;
    int recovery_amount = 5;
    int reducing_amount = 1;
    bool is_enabled = true;

    //--------------------------------------------------------------
    int getNearPointIndex(int x, int y, vector<TrackingPoint>& points, int near_dist) {

        if (points.size() == 0) return -1;

        int min_i = -1;
        double min_d = 0;

        for (int i = 0; i < points.size(); i++) {
            double d = points[i].distanceFrom(x, y);
            if (i == 0 || (d < min_d)) {
                min_d = d;
                min_i = i;
            }
        }

        if (min_d >= near_dist) return -1;
        return min_i;
    }

public:

    //--------------------------------------------------------------
    Tracker() {
    }

    //--------------------------------------------------------------
    bool isEnabled() {
        return is_enabled;
    }

    //--------------------------------------------------------------
    void enable() {
        is_enabled = true;
        points.clear();
    }

    //--------------------------------------------------------------
    void disable() {
        is_enabled = false;
        points.clear();
    }

    //--------------------------------------------------------------
    void setMaxPoints(int new_max_points) {
        this->max_points = new_max_points;
        points.clear();
    }

    //--------------------------------------------------------------
    void setMaxLife(int new_max_life) {
        this->max_life = new_max_life;
    }

    //--------------------------------------------------------------
    void setNearDistance(int new_near_distance) {
        this->near_distance = new_near_distance;
    }

    //--------------------------------------------------------------
    void setRecoveryAmount(int new_recovery_amount) {
        this->recovery_amount = new_recovery_amount;
    }

    //--------------------------------------------------------------
    void setReducingAmount(int new_reducing_amount) {
        this->reducing_amount = new_reducing_amount;
    }

    //--------------------------------------------------------------
    vector<TrackingPoint> get() {
        return points;
    }

    //--------------------------------------------------------------
    void set(vector<cv::Point> candidates) {
        // トラッキングしない場合 ================================================
        if (!is_enabled) {
            points.clear();
            for (int i = 0; i < candidates.size(); i++) {
                if (points.size() < this->max_points) {
                    points.push_back(TrackingPoint(candidates[i].x, candidates[i].y, i, max_life));
                }
                else {
                    return;
                }
            }
            return;
        }

        // トラッキングする場合 ===================================================
       
        // あるフレームで検出されたすべての候補点群（指定の面積に収まるもの）と、
        // 現在保有している点群（points）とのあいだで、最も距離が近い者同士をペアリングする。
        // ペアリングされなかった候補点は、個数上限に達していなければ先着順で新しい点として迎える。
        // アルゴリズムは洗練されてないので、高速化したいならここを見直す

        // 距離テーブルの初期化 dist_table[現在][候補]
        vector<vector<double>> dist_table;
        dist_table.resize(points.size());
        for (int i = 0; i < dist_table.size(); i++) {
            dist_table[i].resize(candidates.size());
        }

        // 距離計算（この時点で閾値未満のものははじく）
        for (int i = 0; i < points.size(); i++) {
            for (int j = 0; j < candidates.size(); j++) {
                double d = points[i].distanceFrom(candidates[j].x, candidates[j].y);;
                if (d < near_distance ) {
                    dist_table[i][j] = d;
                }
                else {
                    dist_table[i][j] = -1; // 無効
                }
            }
        }

        // 候補点について、ペアリングされたかどうかのフラグ
        vector<bool> candidate_paired(candidates.size(), false);

        while (true) {
            // テーブル内をチェックし、すべて-1で埋まってるようなら終了
            bool all_minus = true;
            for (int i = 0; i < dist_table.size(); i++) {
                for (double d : dist_table[i]) {
                    if (d >= 0) {
                        all_minus = false;
                        break;
                    }
                }
            }
            if (all_minus) break;

            // 距離が-1でない・かつ・最も近い距離にある組み合わせを探す
            int min_i = -1;
            int min_j = -1;
            double min_dist = 99999;
            for (int i = 0; i < points.size(); i++) {
                for (int j = 0; j < candidates.size(); j++) {
                    double d = dist_table[i][j];
                    if (d >=0 && d < min_dist) {
                        min_dist = d;
                        min_i = i;
                        min_j = j;
                    }
                }
            }

            // 見つかったペアに対する処理
            if (min_i >= 0 && min_j >= 0) {
                // 情報を更新
                points[min_i].updatePosition(candidates[min_j].x, candidates[min_j].y, recovery_amount);
                for (int i = 0; i < dist_table.size(); i++) dist_table[i][min_j] = -1;
                for (int j = 0; j < dist_table[0].size(); j++) dist_table[min_i][j] = -1;
                candidate_paired[min_j] = true;
            }

        }

        // ペアリングされてない新しい点がある場合、追加可能であれば、先着順で追加。
        for (int j = 0; j < candidates.size(); j++) {

            // 上限に達していれば終了
            if (points.size() == this->max_points) return;

            // ペアリングされてない点について
            if (!candidate_paired[j]) {
                // 使われていない最小の番号にするためにリストの空き位置を探す
                auto itr = points.begin();
                int id = 0;
                while (id < points.size()) {
                    if (points[id].getId() != id) break;
                    id++;
                    itr++;
                }
                points.insert(itr, TrackingPoint(candidates[j].x, candidates[j].y, id, max_life));
            }
        }
    }

    //--------------------------------------------------------------
    void update() {
        if (!is_enabled) return;

        auto itr = points.begin();
        for (int i = 0; i < points.size(); i++) {
            // 点のライフ更新処理
            points[i].reduceLife(reducing_amount);

            // ライフが0以下であれば、その点は削除
            if (points[i].isDead()) {
                points.erase(itr);
            }
            itr++;
        }
    }

};
