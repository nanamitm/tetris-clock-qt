#pragma once

#include <QObject>
#include <QTimer>
#include <QColor>
#include <QVariantList>
#include <deque>
#include "DigitPatterns.h"

struct ActivePiece {
    fall_instr instr;
    int    digitIndex = 0;
    double currentY   = 0.0;
};

struct DigitChannel {
    std::deque<fall_instr> queue;
    ActivePiece piece;
    bool hasPiece = false;
};

class TetrisController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList board      READ board       NOTIFY boardChanged)
    Q_PROPERTY(int  cols               READ cols        CONSTANT)
    Q_PROPERTY(int  rows               READ rows        CONSTANT)
    Q_PROPERTY(bool use24Hour          READ use24Hour   WRITE setUse24Hour   NOTIFY use24HourChanged)
    Q_PROPERTY(double dropSpeed        READ dropSpeed   WRITE setDropSpeed   NOTIFY dropSpeedChanged)
    Q_PROPERTY(double brightness       READ brightness  WRITE setBrightness  NOTIFY brightnessChanged)
    Q_PROPERTY(int  colorTheme         READ colorTheme  WRITE setColorTheme  NOTIFY appearanceChanged)
    Q_PROPERTY(QString gridDotColor    READ gridDotColor WRITE setGridDotColor NOTIFY appearanceChanged)

public:
    explicit TetrisController(QObject* parent = nullptr);

    static constexpr int COLS = 32;
    static constexpr int ROWS =  9;
    static constexpr int NUM_THEMES = 5;

    QVariantList board()         const;
    int    cols()                const { return COLS; }
    int    rows()                const { return ROWS; }
    bool   use24Hour()           const { return m_use24Hour; }
    double dropSpeed()           const { return m_dropSpeed; }
    double brightness()          const { return m_brightness; }
    int    colorTheme()          const { return m_colorTheme; }
    QString gridDotColor()       const { return m_gridDotColor; }

    Q_INVOKABLE void setUse24Hour    (bool v);
    Q_INVOKABLE void setDropSpeed    (double v);
    Q_INVOKABLE void setBrightness   (double v);
    Q_INVOKABLE void setColorTheme   (int v);
    Q_INVOKABLE void setGridDotColor (const QString& v);
    Q_INVOKABLE void replayAnimation ();
    Q_INVOKABLE void showDate        ();
    Q_INVOKABLE void showTime        ();

signals:
    void boardChanged();
    void use24HourChanged();
    void dropSpeedChanged();
    void brightnessChanged();
    void appearanceChanged();   // theme or grid dot color changed

private slots:
    void onAnimTick();
    void onSecondTick();

private:
    // Board stores color indices (-1=empty, -2=colon dot, 0-8=piece colors)
    int          m_board[ROWS][COLS];
    DigitChannel m_channels[4];
    bool         m_colonVisible  = true;
    bool         m_showingDate  = false;
    bool         m_use24Hour     = true;
    double       m_dropSpeed     = 0.8;
    double       m_brightness    = 1.0;
    int          m_colorTheme    = 0;
    QString      m_gridDotColor  = "#252525";
    int          m_currentHour   = -1;
    int          m_currentMinute = -1;
    QTimer       m_animTimer;
    QTimer       m_secTimer;

    void clearAllState();
    void buildQueueForTime(int hour, int minute);
    void placePiece(int dst[][COLS], const fall_instr& instr,
                    int digitIndex, int boardRow) const;
    QColor colorForIndex(int idx) const;
};
