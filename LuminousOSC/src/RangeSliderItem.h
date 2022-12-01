// オリジナルのコード（ofxHPVPlayerのサンプルコードより）
// https://github.com/vjacob/ofxHPVPlayer/blob/master/example-controls/src/RangeSliderItem.h
// 
// 変更点：
// MIN, MAX を使っている箇所をstd::min(), std::max() に置き換え。
// 範囲を示すオレンジ色の帯が、シークバーの背面に来るように修正。
// setRangeInOut()を追加。入力から自動的に開始位置または終了位置を設定。
// setSelectedRange()を追加。レンジを0～1.0で直接設定。


#ifndef RangeSliderItem_h
#define RangeSliderItem_h

#pragma once

#include "ofMain.h"

class RangeSliderItem
{
public:
    RangeSliderItem(float _x, float _y, float _width, float _height, std::string _name)
    {
        name = _name;
        
        draw_pos.x = _x;
        draw_pos.y = _y;
        float rect_x = draw_pos.x - (_width / 2);
        float rect_y = draw_pos.y - (_height / 2);
        hit_zone = ofRectangle(rect_x, rect_y, _width, _height);
        
        b_selected = false;
        b_hovered = false;
        
        range_in = 0.f;
        range_out = 1.f;
    }
    
    RangeSliderItem()
    {
        
    }
    
    void draw()
    {
        ofPushStyle();

        float line_x = hit_zone.x;
        float line_y = hit_zone.y + (hit_zone.height / 2);

        /* Loop range */
        if (range_in > 0.0f || range_out < 1.0f)
        {
            ofSetColor(255, 174, 0, 100);
            float range_start = line_x + (hit_zone.width * range_in);
            float range_end = hit_zone.width * (range_out - range_in);
            ofDrawRectangle(range_start, line_y - 10, range_end, 20);
        }

        if (b_hovered) ofSetColor(ofColor::lightGrey);
        else ofSetColor(ofColor::white);
        
        /* Main transport line */
        ofDrawLine(line_x, line_y, line_x+hit_zone.width, line_y);
        
        /* Cursor */
        ofSetLineWidth(3.0f);
        ofDrawLine(line_x+cur_pos, line_y-10, line_x+cur_pos, line_y+10);
        
        ofPopStyle();
    }
    
    bool inside(const ofVec2f& hit)
    {
        if (hit_zone.inside(hit))
        {
            if (b_selected) return true;
            else
            {
                b_hovered = true;
                return true;
            }
        }
        else
        {
            b_hovered = false;
            return false;
        }
    }
    
    void select()
    {

    }
    
    void unselect()
    {

    }
    
    std::string& getName()
    {
        
    }
    
    void setPlayhead(float _cur_pos)
    {
        cur_pos = _cur_pos*hit_zone.width;
    }
    
    void mouseDown(const ofVec2f& _cursor)
    {
        cur_pos = (_cursor.x - (hit_zone.x + hit_zone.width * range_in)) / ((range_out - range_in) * hit_zone.width);
    }

    void setRangeInOut(float _input) {
        // 範囲のうち、中央より前方をクリックすると、開始位置を設定。後方なら終了位置を設定。
        float pos = (_input - hit_zone.x) / hit_zone.width;
        if (pos < (range_in/2 + range_out/2) ) {
            setRangeIn(_input);
        }
        else {
            setRangeOut(_input);
        }
    }


    void setRangeIn(float _in)
    {
        range_in = (_in - hit_zone.x) / hit_zone.width;
        
        if (range_in > range_out)
        {
            //range_in = MAX(0.0f, range_out - 0.1f);
            range_in = std::max(0.0f, range_out - 0.1f);
        }
    }
   
    void setRangeOut(float _out)
    {
        range_out = (_out - hit_zone.x) / hit_zone.width;
        
        if (range_out < range_in)
        {
            //range_out = MIN(1.0f, range_in + 0.1f);
            range_out = std::min(1.0f, range_in + 0.1f);
        }
    }
    
    void clearRange()
    {
        range_in = 0.f;
        range_out = 1.f;
    }
    
    float getValue()
    {
        return cur_pos;
    }
    
    ofVec2f getSelectedRange()
    {
        return ofVec2f(range_in, range_out);
    }

    // 追加したメソッド
    // ピクセル座標ではなく、0～1.0の範囲で値を指定
    void setSelectedRange(float in, float out) {
        range_in = in;
        range_out = out;
    }
    
    ofRectangle& getRect()
    {
        return hit_zone;
    }
    
private:
    ofVec2f draw_pos;
    ofRectangle hit_zone;
    float cur_pos;
    float range_in;
    float range_out;
    std::string name;
    bool b_hovered;
    bool b_selected;
};


#endif /* RangeSliderItem_h */
