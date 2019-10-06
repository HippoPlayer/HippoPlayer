#pragma once

#include <QtWidgets/QAbstractScrollArea>
#include <QtGui/QFont>
#include <QtGui/QPen>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class QPaintEvent;
class QPainter;
class QResizeEvent;
class QRegion;
struct HippoTrackerChannel;

struct TrackerDisplaySettings {
    short line_spacing;
    short track_text_pad;  // padding in this area | <-> C-4 .. F03 <-> |
    int margin_spacing;

    uint32_t note_color;
    uint32_t instrument_color;
    uint32_t effect_color;
    uint32_t volume_color;
};

struct PatternData {
    int row_count;
};

class TrackerDisplay : public QAbstractScrollArea {
    Q_OBJECT
public:
	TrackerDisplay(QWidget* parent);
    void event(const unsigned char* data, int len);

private:
    struct Range {
        int start;
        int end;
    };

    QFont m_mono_font;
	void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

	void render_track(QPainter& painter, const QRegion& region, int track, const HippoTrackerChannel* channel, bool last_track);
	void render_tracks(QPainter& painter, const QRegion& region);
    void render_numbers(QPainter& painter, const QRegion& region);
    void update_font_size();
    void setup_scroll_bars();
    void set_playing_row(int row);
    void invalidate_left_margin_row(int row);
    void set_scroll_pos(int new_scroll_pos_x, int new_scroll_pos_y);

    inline int div_floor(int a, int b) {
        if (a < 0) {
            return -abs(a) / b - 1;
        }
        return a / b;
    }

    inline int get_logical_x(int track) {
        return track * m_track_width;
    }

    inline int get_logical_y(int row) {
        return row * m_row_height;
    }

    inline int get_physical_x(int track) {
	    return m_left_margin_width + get_logical_x(track) - m_scroll_pos_x;
    }

    inline int get_physical_y(int row) {
	    return m_top_margin_height + get_logical_y(row) - m_scroll_pos_y;
    }

    inline int get_track_from_logical_x(int x) {
	    return div_floor(x, m_track_width);
    }

    inline int get_row_from_logical_y(int y) {
	    return div_floor(y, m_row_height);
    }

    int inline get_track_from_physical_x(int x) {
        return get_track_from_logical_x(x - m_left_margin_width + m_scroll_pos_x);
    }

    int get_row_from_physical_y(int y) {
        return get_row_from_logical_y(y - m_top_margin_height + m_scroll_pos_y);
    }

    Q_SLOT void test_change_row();

	TrackerDisplaySettings m_settings;

	unsigned char* m_tracker_data = nullptr;

	int m_scroll_pos_x = 0;
	int m_scroll_pos_y = 0;

    int m_top_margin = 0;
    int m_bottom_margin = 0;

    int m_top_margin_height = 0;
    int m_left_margin_width = 0;

    int m_row_height = 0;
    int m_row_count = 0;
    int m_track_width = 0;
    int m_current_row = 0;

    // Should come from data
    int m_track_count = 4;

    QPen m_note_color;
    QPen m_instrument_color;
    QPen m_effect_color;
    QPen m_volume_color;
};
