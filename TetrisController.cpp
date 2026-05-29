#include "TetrisController.h"
#include <QTime>
#include <QSettings>
#include <cmath>
#include <algorithm>

static constexpr int    Y_OFFSET          = 8;
static constexpr double START_Y           = -8.0;
static constexpr double DROP_SPEED_DEFAULT = 0.8;

// ── Color themes ──────────────────────────────────────────────────────────────
// Each theme has 9 colors (indices 0-8) matching original tetrisColors[] order.
// Index 8 (black) is kept near-invisible to match "LED off".
static const QColor THEMES[TetrisController::NUM_THEMES][9] = {
    // 0: Classic  (original Tetris colors)
    {{240,0,0},{0,200,0},{0,80,240},{200,200,200},{240,220,0},{0,220,220},{220,0,220},{240,140,0},{20,20,20}},
    // 1: Monochrome
    {{220,220,220},{160,160,160},{200,200,200},{255,255,255},{140,140,140},{180,180,180},{200,200,200},{170,170,170},{20,20,20}},
    // 2: Pastel
    {{255,150,150},{150,230,150},{150,180,255},{240,240,255},{255,240,150},{150,240,240},{240,150,240},{255,200,150},{20,20,20}},
    // 3: Neon
    {{255,0,80},{0,255,100},{0,150,255},{255,255,255},{255,230,0},{0,255,240},{180,0,255},{255,100,0},{10,10,10}},
    // 4: Retro  (amber / phosphor-green alternating)
    {{255,160,0},{0,220,60},{220,130,0},{0,200,50},{255,190,30},{50,240,80},{180,100,0},{0,180,40},{20,20,20}},
};

// Preset grid-dot colors bundled per theme (used as default when switching theme)
static const char* THEME_GRID_DOTS[TetrisController::NUM_THEMES] = {
    "#252525", "#1a1a1a", "#252030", "#08080f", "#1a1200",
};

// ── Rotation-during-fall logic (matches original library) ─────────────────────
static int effectiveRotation(const fall_instr& instr, double currentY)
{
    int fi  = static_cast<int>(currentY + Y_OFFSET);
    int ys  = instr.y_stop;
    int rot = instr.num_rot;
    if (rot == 1) {
        if (fi < ys / 2)           rot = 0;
    } else if (rot == 2) {
        if      (fi < ys / 3)      rot = 0;
        else if (fi < ys * 2 / 3)  rot = 1;
    } else if (rot == 3) {
        if      (fi < ys / 4)      rot = 0;
        else if (fi < ys / 2)      rot = 1;
        else if (fi < ys * 3 / 4)  rot = 2;
    }
    return rot;
}

// ── 12h/24h conversion ────────────────────────────────────────────────────────
static int displayHour(int rawHour, bool use24Hour)
{
    if (use24Hour) return rawHour;
    int h = rawHour % 12;
    return (h == 0) ? 12 : h;
}

// ── Color lookup (theme-aware) ────────────────────────────────────────────────
QColor TetrisController::colorForIndex(int idx) const
{
    if (idx < 0 || idx > 8) return QColor(120, 120, 120);
    return THEMES[qBound(0, m_colorTheme, NUM_THEMES - 1)][idx];
}

// ── Constructor ───────────────────────────────────────────────────────────────
TetrisController::TetrisController(QObject* parent) : QObject(parent)
{
    clearAllState();

    QSettings s("TetrisClock", "TetrisClock");
    m_use24Hour    = s.value("use24Hour",    true).toBool();
    m_dropSpeed    = s.value("dropSpeed",    DROP_SPEED_DEFAULT).toDouble();
    m_brightness   = s.value("brightness",   1.0).toDouble();
    m_colorTheme   = s.value("colorTheme",   0).toInt();
    m_gridDotColor = s.value("gridDotColor", QString(THEME_GRID_DOTS[0])).toString();

    connect(&m_animTimer, &QTimer::timeout, this, &TetrisController::onAnimTick);
    m_animTimer.start(50);
    connect(&m_secTimer,  &QTimer::timeout, this, &TetrisController::onSecondTick);
    m_secTimer.start(1000);

    QTime now = QTime::currentTime();
    m_currentHour   = now.hour();
    m_currentMinute = now.minute();
    buildQueueForTime(displayHour(m_currentHour, m_use24Hour), m_currentMinute);
}

void TetrisController::clearAllState()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            m_board[r][c] = -1;   // -1 = empty

    for (int i = 0; i < 4; i++) {
        m_channels[i].queue.clear();
        m_channels[i].hasPiece = false;
    }
}

// ── board() property: int indices → hex strings ───────────────────────────────
QVariantList TetrisController::board() const
{
    int temp[ROWS][COLS];
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            temp[r][c] = m_board[r][c];

    // Overlay falling pieces
    for (int i = 0; i < 4; i++) {
        const DigitChannel& ch = m_channels[i];
        if (!ch.hasPiece) continue;
        const ActivePiece& ap  = ch.piece;
        int boardRow = static_cast<int>(std::floor(ap.currentY));
        int rot      = effectiveRotation(ap.instr, ap.currentY);
        fall_instr tmp = ap.instr;
        tmp.num_rot = rot;
        int xBase = DIGIT_X_OFFSETS[i] + tmp.x_pos;
        for (auto [dx, dy] : getBlockOffsets(tmp.blocktype, tmp.num_rot)) {
            int row = boardRow + dy, col = xBase + dx;
            if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
                temp[row][col] = tmp.color;
        }
    }

    // Colon dots: index -2 (rendered as fixed light gray)
    if (m_colonVisible) {
        for (int c : {14, 15}) {
            if (2 < ROWS) temp[2][c] = -2;
            if (5 < ROWS) temp[5][c] = -2;
        }
    }

    static const QString COLON_COLOR = QColor(210, 210, 210).name();

    QVariantList result;
    result.reserve(ROWS * COLS);
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
            int ci = temp[r][c];
            if      (ci == -1) result.append(QString());
            else if (ci == -2) result.append(COLON_COLOR);
            else               result.append(colorForIndex(ci).name());
        }
    return result;
}

// ── Place piece: store color index (not QColor) ───────────────────────────────
void TetrisController::placePiece(int dst[][COLS], const fall_instr& instr,
                                   int digitIndex, int boardRow) const
{
    int xBase = DIGIT_X_OFFSETS[digitIndex] + instr.x_pos;
    for (auto [dx, dy] : getBlockOffsets(instr.blocktype, instr.num_rot)) {
        int row = boardRow + dy, col = xBase + dx;
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
            dst[row][col] = instr.color;
    }
}

// ── Animation tick ────────────────────────────────────────────────────────────
void TetrisController::onAnimTick()
{
    bool changed = false;
    for (int i = 0; i < 4; i++) {
        DigitChannel& ch = m_channels[i];
        if (ch.hasPiece) {
            ch.piece.currentY += m_dropSpeed;
            double target = static_cast<double>(ch.piece.instr.y_stop - Y_OFFSET);
            if (ch.piece.currentY >= target) {
                placePiece(m_board, ch.piece.instr, i,
                           static_cast<int>(std::round(target)));
                ch.hasPiece = false;
            }
            changed = true;
        }
        if (!ch.hasPiece && !ch.queue.empty()) {
            ActivePiece ap;
            ap.instr      = ch.queue.front(); ch.queue.pop_front();
            ap.digitIndex = i;
            ap.currentY   = START_Y;
            ch.piece = ap; ch.hasPiece = true;
            changed = true;
        }
    }
    if (changed) emit boardChanged();
}

// ── Second tick ───────────────────────────────────────────────────────────────
void TetrisController::onSecondTick()
{
    if (m_showingDate) {
        // 日付表示中はコロン点滅・時刻更新をスキップ
        emit boardChanged();
        return;
    }

    m_colonVisible = !m_colonVisible;
    QTime now = QTime::currentTime();
    int rawH = now.hour(), m = now.minute();
    if (rawH != m_currentHour || m != m_currentMinute) {
        m_currentHour = rawH; m_currentMinute = m;
        clearAllState();
        buildQueueForTime(displayHour(rawH, m_use24Hour), m);
    }
    emit boardChanged();
}

void TetrisController::buildQueueForTime(int hour, int minute)
{
    int digits[4] = {hour / 10, hour % 10, minute / 10, minute % 10};
    for (int di = 0; di < 4; di++)
        for (int k = 0; k < DIGIT_SIZES[digits[di]]; k++)
            m_channels[di].queue.push_back(ALL_DIGITS[digits[di]][k]);
}

// ── Setters ───────────────────────────────────────────────────────────────────
void TetrisController::setUse24Hour(bool v)
{
    if (m_use24Hour == v) return;
    m_use24Hour = v;
    QSettings("TetrisClock", "TetrisClock").setValue("use24Hour", v);
    emit use24HourChanged();
    QTime now = QTime::currentTime();
    clearAllState();
    m_currentHour = now.hour(); m_currentMinute = now.minute();
    buildQueueForTime(displayHour(m_currentHour, m_use24Hour), m_currentMinute);
}

void TetrisController::setDropSpeed(double v)
{
    v = qBound(0.3, v, 2.0);
    if (qFuzzyCompare(m_dropSpeed, v)) return;
    m_dropSpeed = v;
    QSettings("TetrisClock", "TetrisClock").setValue("dropSpeed", v);
    emit dropSpeedChanged();
}

void TetrisController::setBrightness(double v)
{
    v = qBound(0.2, v, 1.0);
    if (qFuzzyCompare(m_brightness, v)) return;
    m_brightness = v;
    QSettings("TetrisClock", "TetrisClock").setValue("brightness", v);
    emit brightnessChanged();
}

void TetrisController::setColorTheme(int v)
{
    v = qBound(0, v, NUM_THEMES - 1);
    if (m_colorTheme == v) return;
    m_colorTheme = v;
    // Also update grid dot to match theme default
    m_gridDotColor = THEME_GRID_DOTS[v];
    QSettings s("TetrisClock", "TetrisClock");
    s.setValue("colorTheme",   v);
    s.setValue("gridDotColor", m_gridDotColor);
    emit appearanceChanged();
    emit boardChanged();
}

void TetrisController::replayAnimation()
{
    clearAllState();
    buildQueueForTime(displayHour(m_currentHour, m_use24Hour), m_currentMinute);
    emit boardChanged();
}

void TetrisController::showDate()
{
    QDate today = QDate::currentDate();
    clearAllState();
    m_showingDate  = true;
    m_colonVisible = true;  // コロンを常時点灯して時刻と区別
    // MM/DD を HH:MM と同じ4桁フォーマットで表示
    buildQueueForTime(today.month(), today.day());
    emit boardChanged();
}

void TetrisController::showTime()
{
    m_showingDate = false;
    clearAllState();
    // 強制的に現在時刻で再構築
    QTime now = QTime::currentTime();
    m_currentHour   = now.hour();
    m_currentMinute = now.minute();
    buildQueueForTime(displayHour(m_currentHour, m_use24Hour), m_currentMinute);
    emit boardChanged();
}

void TetrisController::setGridDotColor(const QString& v)
{
    if (m_gridDotColor == v) return;
    m_gridDotColor = v;
    QSettings("TetrisClock", "TetrisClock").setValue("gridDotColor", v);
    emit appearanceChanged();
}
