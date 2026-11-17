#pragma once

#include <QObject>
#include <QString>

class AppViewModel : public QObject {
    Q_OBJECT
public:
    enum class GameScreen { Introduction, LiteratureReview, SystemArchitecture, Implementation, Conclusion };
    Q_ENUM(GameScreen)

    explicit AppViewModel(QObject* parent = nullptr);
    ~AppViewModel() override = default;

    // Properties
    Q_PROPERTY(GameScreen currentScreen READ currentScreen NOTIFY currentScreenChanged)
    Q_PROPERTY(qreal musicVolume READ musicVolume WRITE setMusicVolume NOTIFY musicVolumeChanged)
    Q_PROPERTY(bool muted READ isMuted NOTIFY mutedChanged)

    GameScreen currentScreen() const { return m_screen; }
    qreal musicVolume() const { return m_musicVolume; }
    bool isMuted() const { return m_muted; }

public slots:
    void navigateTo(const QString& screenName);
    void setMusicVolume(qreal value);
    void toggleMute();

signals:
    void currentScreenChanged(GameScreen newScreen);
    void musicVolumeChanged(qreal newVolume);
    void mutedChanged(bool muted);

private:
    GameScreen m_screen = GameScreen::Introduction;
    qreal m_musicVolume = 0.5;
    bool m_muted = false;
};
