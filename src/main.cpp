
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <algorithm>
#include <string>

using namespace geode::prelude;

class $modify(PracticeTools, PlayLayer) {
    struct Fields {
        CCLabelBMFont* hud = nullptr;
        int practiceAttempts = 0;
        float bestProgress = 0.0f;
        bool wasPractice = false;
        float updateTimer = 0.0f;
    };

    void createPracticeHUD() {
        auto size =
            CCDirector::sharedDirector()->getWinSize();

        auto label = CCLabelBMFont::create(
            "",
            "bigFont.fnt"
        );

        if (!label) return;

        label->setScale(0.35f);
        label->setAnchorPoint({0.0f, 1.0f});
        label->setPosition({
            8.0f,
            size.height - 35.0f
        });

        label->setAlignment(kCCTextAlignmentLeft);
        label->setColor({100, 255, 150});
        label->setOpacity(220);
        label->setVisible(false);
        label->setID("practice-tools-hud");

        this->addChild(label, 1000);

        m_fields->hud = label;

        log::info("Practice Tools initialized!");

    }

    void resetLevel() {
        PlayLayer::resetLevel();

        if (m_isPracticeMode) {
            m_fields->practiceAttempts++;
        }
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto fields = m_fields.self();

        if (!fields->hud && m_isPracticeMode) {
            createPracticeHUD();
        }

        if (!fields->hud) {
            return;
        }

        bool practice = m_isPracticeMode;

        if (practice && !fields->wasPractice) {
            fields->practiceAttempts = 1;
            fields->bestProgress = 0.0f;
        }

        fields->wasPractice = practice;

        if (!practice) {
            fields->hud->setVisible(false);
            return;
        }

        float progress = getCurrentPercent();

        fields->bestProgress = std::max(
            fields->bestProgress,
            progress
        );

        bool showHUD =
            Mod::get()->getSettingValue<bool>(
                "show-hud"
            );

        fields->hud->setVisible(showHUD);

        if (!showHUD) {
            return;
        }

        fields->updateTimer += dt;

        if (fields->updateTimer < 0.1f) {
            return;
        }

        fields->updateTimer = 0.0f;

        std::string text = "PRACTICE TOOLS\n";

        text += fmt::format(
            "Attempts: {}",
            fields->practiceAttempts
        );

        if (Mod::get()->getSettingValue<bool>(
            "show-checkpoints"
        )) {
            int checkpoints = m_checkpointArray
                ? m_checkpointArray->count()
                : 0;

            text += fmt::format(
                "\nCheckpoints: {}",
                checkpoints
            );
        }

        if (Mod::get()->getSettingValue<bool>(
            "show-progress"
        )) {
            text += fmt::format(
                "\nProgress: {:.1f}%"
                "\nBest: {:.1f}%",
                progress,
                fields->bestProgress
            );
        }

        fields->hud->setString(text.c_str());
    }
};
