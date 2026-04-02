#include "GameInfoOverlay.hpp"
#include <nxui/core/Renderer.hpp>
#include <nxui/core/Input.hpp>
#include <nxui/core/I18n.hpp>
#include <nxui/widgets/GlassPanel.hpp>
#include <switch.h>
#include <cstdio>
#include <algorithm>

// ── Helpers ──────────────────────────────────────────────────────────────────

static std::shared_ptr<nxui::Label> makeInfoRow(const std::string& text,
                                                 nxui::Font* font,
                                                 float w,
                                                 const nxui::Color& col) {
    auto lbl = std::make_shared<nxui::Label>(text, font);
    lbl->setTextColor(col);
    lbl->setScale(0.82f);
    lbl->setRect({0, 0, w, 24.f});
    lbl->setMultiline(false);
    return lbl;
}

// ── Internal helpers ─────────────────────────────────────────────────────────

std::string GameInfoOverlay::queryPlaytime() const {
    auto& i18n = nxui::I18n::instance();
    PdmPlayStatistics stats{};
    Result rc = pdmqryInitialize();
    if (R_FAILED(rc)) return i18n.tr("gameinfo.playtime.unknown", "Playtime: Unknown");
    rc = pdmqryQueryPlayStatisticsByApplicationId(m_info.titleId, false, &stats);
    pdmqryExit();
    if (R_FAILED(rc)) return i18n.tr("gameinfo.playtime.unknown", "Playtime: Unknown");
    if (stats.total_launches == 0) return i18n.tr("gameinfo.playtime.never", "Playtime: Never played");

    uint64_t secs = stats.playtime / 1'000'000'000ULL;
    uint32_t hrs  = (uint32_t)(secs / 3600);
    uint32_t mins = (uint32_t)((secs % 3600) / 60);
    char buf[64];
    if (hrs > 0)
        std::snprintf(buf, sizeof(buf), i18n.tr("gameinfo.playtime.hours_minutes", "Playtime: %uh %02um  \xE2\x80\xA2  %u launches").c_str(),
                      hrs, mins, stats.total_launches);
    else
        std::snprintf(buf, sizeof(buf), i18n.tr("gameinfo.playtime.minutes", "Playtime: %um  \xE2\x80\xA2  %u launches").c_str(),
                      mins, stats.total_launches);
    return buf;
}

std::string GameInfoOverlay::statusText() const {
    auto& i18n = nxui::I18n::instance();
    if (m_info.isSuspended)  return i18n.tr("gameinfo.status.suspended",    "Status: Suspended");
    if (m_info.needsVerify)  return i18n.tr("gameinfo.status.needs_verify", "Status: Needs verification");
    if (m_info.needsUpdate)  return i18n.tr("gameinfo.status.needs_update", "Status: Update available");
    if (!m_info.hasContents) return i18n.tr("gameinfo.status.no_contents",  "Status: Missing game data");
    if (!m_info.canLaunch)   return i18n.tr("gameinfo.status.cant_launch",  "Status: Cannot launch");
    return i18n.tr("gameinfo.status.ready", "Status: Ready to launch");
}

// ── Widget tree ───────────────────────────────────────────────────────────────

void GameInfoOverlay::buildWidgetTree() {
    clearChildren();
    m_iconWidget.reset();


    auto& i18n = nxui::I18n::instance();

    nxui::Font* bodyFont = m_smallFont ? m_smallFont : m_font;

    nxui::Color primary   = m_theme ? m_theme->textPrimary
                                    : nxui::Color::white();
    nxui::Color secondary = m_theme ? m_theme->textSecondary
                                    : nxui::Color(0.82f, 0.82f, 0.9f, 1.f);

    float contentW = kPanelW - kPadX * 2.f;
    float infoW    = contentW - kIconSize - 20.f;

    // infoCol height: title(36) + gap(5) + 4 rows(24 each) + gaps(5*3=15) = 152
    static constexpr float kTitleH  = 36.f;
    static constexpr float kInfoGap = 5.f;
    static constexpr float kRowH    = 24.f;
    static constexpr float kTopH    = kTitleH + kInfoGap + kRowH * 4.f + kInfoGap * 3.f; // 152

    // ── Panel settings ───────────────────────────────────────
    setAxis(nxui::Axis::COLUMN);
    setCornerRadius(kRadius);
    setPadding(kPadY, kPadX, kPadY, kPadX);
    setAlignItems(nxui::AlignItems::STRETCH);
    setGap(kRowGap);
    setBackingEnabled(true);
    setBlurEnabled(false);
    setWireframeEnabled(false);
    if (m_theme) {
        setBaseColor(m_theme->panelBase);
        setBorderColor(m_theme->panelBorder);
        setHighlightColor(m_theme->panelHighlight);
        setBackingColor(nxui::Color(m_theme->panelBase.r, m_theme->panelBase.g,
                                    m_theme->panelBase.b, 1.f));
    }

    // ── Top row: [icon] [info column] ───────────────────────
    auto topRow = std::make_shared<nxui::Box>(nxui::Axis::ROW);
    topRow->setGap(20.f);
    topRow->setAlignItems(nxui::AlignItems::FLEX_START);
    topRow->setRect({0, 0, contentW, kTopH});
    topRow->setWireframeEnabled(false);

    m_iconWidget = std::make_shared<nxui::Image>();
    m_iconWidget->setTexture(m_info.iconTex);
    m_iconWidget->setCornerRadius(12.f);
    m_iconWidget->setRect({0, 0, kIconSize, kIconSize});
    topRow->addChild(m_iconWidget);

    auto infoCol = std::make_shared<nxui::Box>(nxui::Axis::COLUMN);
    infoCol->setRect({0, 0, infoW, kTopH});
    infoCol->setGap(kInfoGap);
    infoCol->setGrow(1.f);
    infoCol->setWireframeEnabled(false);

    // Title (single line, no overflow)
    auto titleLbl = std::make_shared<nxui::Label>(m_info.title, m_font);
    titleLbl->setTextColor(primary);
    titleLbl->setScale(0.97f);
    titleLbl->setMultiline(false);
    titleLbl->setRect({0, 0, infoW, kTitleH});
    infoCol->addChild(titleLbl);

    // Title ID + type
    char tidBuf[32];
    std::snprintf(tidBuf, sizeof(tidBuf), "%016lX", m_info.titleId);
    std::string typeStr = m_info.isGameCard
        ? i18n.tr("gameinfo.type_gamecard", "Game Card")
        : i18n.tr("gameinfo.type_digital",  "Digital");
    infoCol->addChild(makeInfoRow(i18n.tr("gameinfo.id_label", "ID: ") + std::string(tidBuf) + "  \xE2\x80\xA2  " + typeStr,
                                   bodyFont, infoW, secondary));

    // Playtime
    infoCol->addChild(makeInfoRow(queryPlaytime(), bodyFont, infoW, secondary));

    // Status
    infoCol->addChild(makeInfoRow(statusText(), bodyFont, infoW, secondary));



    topRow->addChild(infoCol);
    addChild(topRow);

    // ── Hint row (B • Close) ──────────
    auto hintRow = std::make_shared<nxui::Box>(nxui::Axis::ROW,
                                               nxui::JustifyContent::FLEX_END,
                                               nxui::AlignItems::CENTER);
    hintRow->setRect({0, 0, contentW, kHintH});
    hintRow->setWireframeEnabled(false);



    auto hintRight = std::make_shared<nxui::Label>(
        i18n.tr("gameinfo.hint.close", "B  \xE2\x80\xA2  Close"), bodyFont);
    hintRight->setTextColor(secondary);
    hintRight->setScale(0.82f);
    hintRight->setHAlign(nxui::Label::HAlign::Right);
    hintRight->setVAlign(nxui::Label::VAlign::Center);
    hintRight->setRect({0, 0, contentW * 0.5f, kHintH});
    hintRow->addChild(hintRight);

    addChild(hintRow);

    // ── Panel size and position ──────────────────────────────
    float panelH = kPadY + kTopH + kRowGap + kHintH + kPadY;
    float panelX = 640.f - kPanelW * 0.5f;
    float panelY = 360.f - panelH * 0.5f;
    setRect({panelX, panelY, kPanelW, panelH});

    layout();
}

// ── Actions ───────────────────────────────────────────────────────────────────

void GameInfoOverlay::setupActions() {
    clearActions();
    addAction(static_cast<uint64_t>(nxui::Button::B), [this]() {
        if (!m_active || m_animatingOut) return;
        hide();
    });

}

// ── Public API ────────────────────────────────────────────────────────────────

void GameInfoOverlay::show(const Info& info) {
    m_info         = info;
    m_active       = true;
    m_animatingOut = false;

    buildWidgetTree();

    m_overlayAlpha.setImmediate(0.f);
    m_panelScale.setImmediate(0.94f);
    m_overlayAlpha.set(1.f, 0.22f, nxui::Easing::outCubic);
    m_panelScale.set(1.f,   0.26f, nxui::Easing::outCubic);

    setFocusable(true);
    setVisible(true);
    setupActions();
}

void GameInfoOverlay::hide() {
    if (!m_active || m_animatingOut) return;
    if (m_closeSfxCb) m_closeSfxCb();
    m_animatingOut = true;
    m_overlayAlpha.set(0.f,   0.18f, nxui::Easing::outCubic);
    m_panelScale.set(0.96f,   0.18f, nxui::Easing::outCubic);
    setFocusable(false);
    clearActions();
}

// ── Update / Render ───────────────────────────────────────────────────────────

void GameInfoOverlay::update(float dt) {
    if (!m_active && !m_animatingOut) return;

    if (m_animatingOut && m_overlayAlpha.value() < 0.01f) {
        m_active       = false;
        m_animatingOut = false;
        setVisible(false);
        return;
    }

    float alpha = m_overlayAlpha.value();
    float sc    = m_panelScale.value();
    setScale(sc);
    setPanelOpacity(alpha);

    for (auto& c : children()) {
        c->setOpacity(alpha);
        c->update(dt);
    }
}

void GameInfoOverlay::render(nxui::Renderer& ren) {
    if (!m_active && !m_animatingOut) return;
    float alpha = m_overlayAlpha.value();
    if (alpha < 0.01f) return;

    ren.drawRect({0, 0, 1280, 720}, nxui::Color(0, 0, 0, 0.5f * alpha));
    nxui::GlassPanel::onRender(ren);
    for (auto& c : children())
        c->render(ren);
}
