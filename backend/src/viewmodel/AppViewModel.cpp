#include "AppViewModel.h"
#include <QDebug>

AppViewModel::AppViewModel(QObject* parent)
    : QObject(parent) {}

void AppViewModel::navigateTo(const QString& screenName) {
    const QString name = screenName.trimmed().toLower();
    if (name == "introduction" || name == "01_introduction") {
        m_screen = GameScreen::Introduction;
    } else if (name == "literaturereview" || name == "02_literature_review") {
        m_screen = GameScreen::LiteratureReview;
    } else if (name == "systemarchitecture" || name == "03_system_architecture") {
        m_screen = GameScreen::SystemArchitecture;
    } else if (name == "implementation" || name == "04_implementation") {
        m_screen = GameScreen::Implementation;
    } else if (name == "conclusion" || name == "05_conclusion") {
        m_screen = GameScreen::Conclusion;
    } else {
        qWarning() << "AppViewModel::navigateTo unknown screen" << screenName;
        return;
    }
    emit currentScreenChanged(m_screen);
}

void AppViewModel::setMusicVolume(qreal value) {
    m_musicVolume = qBound(0.0, value, 1.0);
    emit musicVolumeChanged(m_musicVolume);
}

void AppViewModel::toggleMute() {
    m_muted = !m_muted;
    emit mutedChanged(m_muted);
}
