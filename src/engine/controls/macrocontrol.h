#pragma once

#include <rigtorp/SPSCQueue.h>

#include <QString>

#include "control/controlpushbutton.h"
#include "engine/controls/enginecontrol.h"
#include "preferences/usersettings.h"

class MacroControl : public EngineControl {
    Q_OBJECT
  public:
    MacroControl(QString group, UserSettingsPointer pConfig, int slot);

    void trackLoaded(TrackPointer pNewTrack) override;
    void process(const double dRate, const double dCurrentSample, const int iBufferSize) override;
    void notifySeek(double dNewPlaypos) override;

    bool isRecording() const;

    enum Status {
        NoTrack = -1,
        Empty = 0,
        Armed = 1,
        Recording = 2,
        Recorded = 3,
        Playing = 4,
        PlaybackStopped = 5,
    };

    Status getStatus() const;
    MacroPtr getMacro() const;

  public slots:
    void controlRecord(double value = 1);
    void controlToggle(double value = 1);
    void controlClear(double value = 1);
    void controlActivate(double value = 1);
    void gotoAndPlay();

    void slotJumpQueued();

  private:
    void updateRecording();
    void stopRecording();

    void play();
    void stop();

    void setStatus(Status);

    ConfigKey getConfigKey(QString name);
    int m_slot;
    QString m_controlPattern;

    bool m_bJumpPending;
    rigtorp::SPSCQueue<MacroAction> m_recordedActions;
    QTimer m_updateRecordingTimer;

    MacroPtr m_pMacro;
    unsigned int m_iNextAction;

    ControlObject m_COStatus;
    ControlObject m_COIndicator;

    ControlPushButton m_record;
    ControlPushButton m_toggle;
    ControlPushButton m_clear;
    ControlPushButton m_activate;
};
