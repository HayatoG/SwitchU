#pragma once
#include <nxui/widgets/GlassWidget.hpp>
#include <nxui/widgets/Label.hpp>
#include <nxui/widgets/Box.hpp>
#include <nxui/widgets/Image.hpp>
#include <nxui/core/Animation.hpp>
#include <nxui/core/Font.hpp>
#include <nxui/core/Texture.hpp>
#include <nxui/Theme.hpp>
#include <string>
#include <vector>
#include <functional>

class GameInfoOverlay : public nxui::GlassWidget {
public:
    struct Info {
        std::string              title;
        uint64_t                 titleId      = 0;
        bool                     isGameCard   = false;
        bool                     canLaunch    = false;
        bool                     needsUpdate  = false;
        bool                     needsVerify  = false;
        bool                     hasContents  = true;
        bool                     isSuspended  = false;
        std::vector<std::string> folderNames;
        nxui::Texture*           iconTex      = nullptr; // non-owning
    };

    using ManageFoldersCb = std::function<void(uint64_t)>;
    using VoidCb          = std::function<void()>;

    void setFont(nxui::Font* f)          { m_font = f; }
    void setSmallFont(nxui::Font* f)     { m_smallFont = f; }
    void setTheme(const nxui::Theme* t)  { m_theme = t; }
    void onManageFolders(ManageFoldersCb cb) { m_manageFoldersCb = std::move(cb); }
    void onCloseSfx(VoidCb cb)           { m_closeSfxCb = std::move(cb); }

    void show(const Info& info);
    void hide();
    bool isActive() const { return m_active || m_animatingOut; }

    void updateFolderNames(const std::vector<std::string>& names);

    void update(float dt) override;
    void render(nxui::Renderer& ren) override;

private:
    void buildWidgetTree();
    void setupActions();
    std::string queryPlaytime() const;
    std::string statusText()    const;

    nxui::Font*        m_font      = nullptr;
    nxui::Font*        m_smallFont = nullptr;
    const nxui::Theme* m_theme     = nullptr;

    Info m_info;
    bool m_active       = false;
    bool m_animatingOut = false;

    std::shared_ptr<nxui::Image>  m_iconWidget;
    std::shared_ptr<nxui::Label>  m_foldersLabel;

    nxui::AnimatedFloat m_overlayAlpha;
    nxui::AnimatedFloat m_panelScale;

    ManageFoldersCb m_manageFoldersCb;
    VoidCb          m_closeSfxCb;

    static constexpr float kPanelW   = 720.f;
    static constexpr float kPadX     = 32.f;
    static constexpr float kPadY     = 28.f;
    static constexpr float kRadius   = 24.f;
    static constexpr float kIconSize = 96.f;
    static constexpr float kRowGap   = 16.f;
    static constexpr float kHintH    = 28.f;
};
