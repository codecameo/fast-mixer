//
// Created by asalehin on 7/9/20.
//

#ifndef FAST_MIXER_RECORDINGENGINE_H
#define FAST_MIXER_RECORDINGENGINE_H

#ifndef MODULE_NAME
#define MODULE_NAME "AudioEngine"
#endif

#include <oboe/Definitions.h>
#include <oboe/AudioStream.h>
#include "../logging_macros.h"
#include "RecordingIO.h"
#include "streams/RecordingStream.h"
#include "streams/LivePlaybackStream.h"
#include "streams/PlaybackStream.h"

class RecordingEngine {

public:

    RecordingEngine(string appDir, string recordingSessionId, bool recordingScreenViewModelPassed);
    ~RecordingEngine();

    void startRecording();
    void stopRecording();

    void startLivePlayback();
    void stopLivePlayback();

    bool startPlayback();
    void stopAndResetPlayback();
    void pausePlayback();

    void flushWriteBuffer();
    void restartPlayback();

    int getCurrentMax();

    void resetCurrentMax();

    void setStopPlayback();

    int getTotalRecordedFrames();

    int getCurrentPlaybackProgress();

    void setPlayHead(int position);

    int getDurationInSeconds();

    void resetAudioEngine();

    void closePlaybackStream();

    void stopPlaybackCallable();

    bool startPlaybackCallable();

private:

    const char* TAG = "Recording Engine:: %s";

    string mRecordingSessionId = nullptr;
    string mAppDir = nullptr;
    bool mPlayback = true;

    mutex recordingStreamMtx;
    mutex livePlaybackStreamMtx;
    mutex playbackStreamMtx;

    RecordingIO mRecordingIO;

    RecordingStream recordingStream = RecordingStream(&mRecordingIO);
    LivePlaybackStream livePlaybackStream = LivePlaybackStream(&mRecordingIO);
    PlaybackStream playbackStream = PlaybackStream(&mRecordingIO);
    bool mRecordingScreenViewModelPassed = false;

    void stopPlayback();
};


#endif //FAST_MIXER_RECORDINGENGINE_H
