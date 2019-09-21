#pragma once

#include <QtWidgets/QWidget>
#include <QtGui/QStaticText>
#include <QtGui/QFont>
#include <QtCore/QTimer>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ScrollText : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)

public:
    explicit ScrollText(QWidget *parent = 0);

    Q_SLOT QString text() const;
    Q_SLOT void setText(QString text);

    Q_SLOT QString separator() const;
    Q_SLOT void setSeparator(QString separator);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void updateText();
    //QFont m_font;
    QString _text;
    QString _separator;
    QStaticText staticText;
    int singleTextWidth;
    QSize wholeTextSize;
    int leftMargin;
    bool scrollEnabled;
    int scrollPos;
    QImage alphaChannel;
    QImage buffer;
    QTimer timer;

private slots:
    virtual void timer_timeout();
};
