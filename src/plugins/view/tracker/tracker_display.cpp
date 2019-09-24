#include "tracker_display.h"
#include <QtGui/QPaintEvent>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include "../../../plugin_api/HippoMessages.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TrackerDisplay::TrackerDisplay(QWidget* parent) :
    QAbstractScrollArea(parent),
    m_mono_font(QStringLiteral("DejaVu Sans Mono"), 14) {
    m_settings.line_spacing = 2;
    m_settings.track_text_pad = 4;
    m_settings.margin_spacing = 4;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


    /*
    QTimer* timer = new QTimer;
    QObject::connect(timer, &QTimer::timeout, this, &TrackerDisplay::test_change_row);
    timer->setInterval(200);
    timer->start();
    */

	update_font_size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::update_font_size() {
    QFontMetrics metrics(m_mono_font);

    m_top_margin = metrics.height() + 2;
    m_bottom_margin = 0;
    m_top_margin_height = metrics.height() + 2;

    m_row_height = m_settings.line_spacing + metrics.lineSpacing();

    // C-4 02 F08

    // TODO: This needs to change depending on what is being played
    m_track_width = (metrics.horizontalAdvance('0') * 12) + m_settings.track_text_pad * 2;
    m_left_margin_width = (metrics.horizontalAdvance('0') * 2) + m_settings.margin_spacing * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::render_numbers(QPainter& painter, const QRegion& region) {
    QFontMetrics metrics(m_mono_font);

	const QRect& rect = region.boundingRect();

	const int first_row = qBound(0, get_row_from_physical_y(qMax(rect.top(), m_top_margin_height)), m_row_count - 1);
	const int last_row = qBound(0, get_row_from_physical_y(qMax(rect.bottom(), m_top_margin_height)), m_row_count - 1);

    const int pad_zeros = (first_row >= 100 || last_row >= 100) ? 3 : 2;

	painter.setClipRect(QRectF(QPointF(0.0f, m_top_margin_height - 5.5f),
	                           QPointF(m_left_margin_width - 0.5f, rect.bottom() + 1.0f)));

	QRectF padding(QPointF(rect.left(), m_top_margin_height - 5.5f),
	               QPointF(m_left_margin_width - 0.5f, rect.bottom() + 1.0f));
	painter.fillRect(padding, QBrush(QColor(0, 0, 0)));

	QPen gray(QColor(180, 180, 180));
	QPen white(QColor(255, 255, 255));
	QPen selection(QColor(200, 200, 0));

	painter.setPen(QColor(180, 180, 180));

	for (int row = first_row; row <= last_row; ++row) {
		QRect left_margin(0, get_physical_y(row), m_left_margin_width, m_row_height);

		if (!region.intersects(left_margin))
			continue;

        const QString number = QString("%1").arg(row, pad_zeros, 10, QLatin1Char('0'));

        left_margin.moveLeft(m_settings.margin_spacing);

        if (row == m_current_row) {
            painter.setPen(selection);
        } else if (row % 4) {
            painter.setPen(gray);
        } else {
            painter.setPen(white);
        }

        painter.drawText(left_margin, number);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::render_track(QPainter& painter, const QRegion& region, int track, const HippoTrackerChannel* channel, bool last_track) {
	const QRect& rect = region.boundingRect();
	const int first_row = qBound(0, get_row_from_physical_y(qMax(rect.top(), m_top_margin_height)), m_row_count - 1);
	const int last_row = qBound(0, get_row_from_physical_y(qMax(rect.bottom(), m_top_margin_height)), m_row_count - 1);

	auto rows_data = channel->row_data();

	QPen grayDark(QColor(80, 80, 80));
	QPen gray(QColor(180, 180, 180));
	QPen white(QColor(255, 255, 255));
	QPen selection(QColor(200, 200, 0));

	// Draw track separation line
	const int start_y = get_physical_y(first_row);
    const int end_y = get_physical_y(last_row + 1);
    const int x_pos = get_physical_x(track);

    painter.setPen(grayDark);
    painter.drawLine(x_pos, start_y, x_pos, end_y);

    if (last_track) {
        const int x_pos = get_physical_x(track + 1);
        painter.drawLine(x_pos, start_y, x_pos, end_y);
    }

	for (int row = first_row; row <= last_row; ++row) {
		QRect pattern_data_rect(get_physical_x(track) + 2, get_physical_y(row), m_track_width, m_row_height);

		if (!region.intersects(pattern_data_rect))
			continue;

		auto row_data = rows_data->Get(row);

        if (row == m_current_row) {
            painter.setPen(selection);
        } else if (row % 4) {
            painter.setPen(gray);
        } else {
            painter.setPen(white);
        }

        //pattern_data_rect.moveLeft(2);

        const char* note = row_data->note()->c_str();
        const char* instrument = row_data->instrument()->c_str();
        const char* effect = row_data->effect()->c_str();
        const char* voleffect = row_data->volumeffect()->c_str();

        painter.drawText(pattern_data_rect, QString(QStringLiteral("%1 %2 %3%4").arg(note, instrument, effect, voleffect)));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::event(const unsigned char* data, int len) {
    const HippoMessage* message = GetHippoMessage(data);

    // Only care about added files right now
    if (message->message_type() != MessageType_tracker_data)
        return;

    // allocate storage buffer if we don't have one

    if (!m_tracker_data) {
        m_tracker_data = (unsigned char*)malloc(512 * 1024);
    }

    memcpy(m_tracker_data, data, len);

    const HippoTrackerData* tracker_data = message->message_as_tracker_data();
    set_playing_row(tracker_data->current_row());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::set_playing_row(int new_row) {
	int old_row = m_current_row;
	m_current_row = new_row;

	//printf("new row %d\n", now_row);

	m_current_row = qBound(0, m_current_row, 64 - 1);

	if (old_row != new_row) {
		invalidate_left_margin_row(old_row);
		invalidate_left_margin_row(m_current_row);
	}

	int y_scroll = (m_current_row * m_row_height) - ((viewport()->height() - m_top_margin_height) / 2) + m_row_height / 2;
	set_scroll_pos(m_scroll_pos_x, y_scroll);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::set_scroll_pos(int new_scroll_pos_x, int new_scroll_pos_y) {
	new_scroll_pos_x = qMax(new_scroll_pos_x, 0);

	if ((new_scroll_pos_x != m_scroll_pos_x) ||
	    (new_scroll_pos_y != m_scroll_pos_y)) {
		int dx = m_scroll_pos_x - new_scroll_pos_x;
		int dy = m_scroll_pos_y - new_scroll_pos_y;

		// update scrollPos
		m_scroll_pos_x = new_scroll_pos_x;
		m_scroll_pos_y = new_scroll_pos_y;

        QRect clip = viewport()->geometry();

        if (dx == 0) clip.setTop(m_top_margin_height); // don't scroll the top margin
        if (dy == 0) clip.setLeft(m_left_margin_width); // don't scroll the left margin

	    viewport()->scroll(dx, dy, clip);
	}

	//horizontalScrollBar()->setValue(new_scroll_pos_x);
	verticalScrollBar()->setValue(m_current_row);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::render_tracks(QPainter& painter, const QRegion& region) {
	const QRect& rect = region.boundingRect();

    const HippoMessage* message = GetHippoMessage(m_tracker_data);
    const HippoTrackerData* tracker_data = message->message_as_tracker_data();

	const int start_track = qBound(0, get_track_from_physical_x(qMax(rect.left(), m_left_margin_width)), m_track_count);
	const int end_track = qBound(0, get_track_from_physical_x(rect.right()) + 1, m_track_count);

	painter.setClipRect(QRectF(QPointF(m_left_margin_width - 0.5f, m_top_margin_height - 0.5f),
	                           QPointF(rect.right() + 1.0f, rect.bottom() + 1.0f)));
	painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

	auto channels = tracker_data->channels();

	for (int track = start_track; track < end_track; ++track) {
		render_track(painter, region, track, channels->Get(track), track == end_track - 1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::paintEvent(QPaintEvent* event) {
	QPainter painter(viewport());
	painter.setFont(m_mono_font);

	// render numbers but no tracks if we don't have any data

    render_numbers(painter, event->region());

	if (!m_tracker_data) {
	    return;
	}

    render_tracks(painter, event->region());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::invalidate_left_margin_row(int row) {
    QRect rect(QPoint(0, get_physical_y(row)),
               QPoint(m_left_margin_width, get_physical_y(row + 1) - 1));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::test_change_row() {
    int current_row = m_current_row;
    current_row += 1;
    current_row &= 63;

    set_playing_row(current_row);

    viewport()->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::setup_scroll_bars() {
	verticalScrollBar()->setValue(m_current_row);
	verticalScrollBar()->setMinimum(0);
	verticalScrollBar()->setMaximum(m_row_count - 1);
	verticalScrollBar()->setPageStep(1);
}

void TrackerDisplay::resizeEvent(QResizeEvent* event) {
	//m_row_count = qMin((event->size().height() - m_top_margin) / m_row_height, 64);
	m_row_count = 64; //(event->size().height() - m_top_margin) / m_row_height;
	setup_scroll_bars();
	viewport()->update();
}

