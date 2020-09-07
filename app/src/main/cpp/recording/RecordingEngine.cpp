//
// Created by asalehin on 7/9/20.
//

#include <oboe/Oboe.h>

#include <utility>
#include "../jni_env.h"
#include "RecordingEngine.h"
#include "../logging_macros.h"

RecordingEngine::RecordingEngine(
        char* appDir,
        bool recordingScreenViewModelPassed) {
    assert(StreamConstants::mInputChannelCount == StreamConstants::mOutputChannelCount);
    mAppDir = appDir;
    mRecordingIO.setStopPlaybackCallback([&]() {
        setStopPlayback();
    });
    mRecordingScreenViewModelPassed = recordingScreenViewModelPassed;
}

RecordingEngine::~RecordingEngine() {
    stopRecording();
    stopLivePlayback();
}

void RecordingEngine::setRecordingSessionId(char* recordingSessionId) {
    mRecordingSessionId = recordingSessionId;

    char* appDir = strcat(mAppDir, "/");
    char* sessionCacheDir = strcat(appDir, mRecordingSessionId);
    char* recordingFilePath = strcat(sessionCacheDir, "/recording.wav");
    mRecordingIO.setRecordingFilePath(recordingFilePath);
}

void RecordingEngine::startLivePlayback() {
    livePlaybackStream.openLivePlaybackStream();
    if (livePlaybackStream.mStream != nullptr) {
        mRecordingIO.sync_live_playback();
        livePlaybackStream.startStream();
    } else {
        LOGE(TAG, "startLivePlayback(): Failed to create live playback (%p) stream", livePlaybackStream.mStream);
        livePlaybackStream.closeStream();
    }
}

void RecordingEngine::stopLivePlayback() {
    if (livePlaybackStream.mStream == nullptr) {
        return;
    }

    if (livePlaybackStream.mStream->getState() != oboe::StreamState::Closed) {
        livePlaybackStream.stopStream();
        livePlaybackStream.closeStream();
    }
}

void RecordingEngine::pauseLivePlayback() {
    livePlaybackStream.stopStream();
}

bool RecordingEngine::startPlayback() {
    playbackStream.openPlaybackStream();
    if (playbackStream.mStream) {
        if(mRecordingIO.setup_audio_source()) {
            playbackStream.startStream();
            return true;
        } else {
            playbackStream.closeStream();
            return false;
        }
    } else {
        LOGE(TAG, "startPlayback(): Failed to create playback (%p) stream", playbackStream.mStream);
        playbackStream.closeStream();
        return false;
    }
}

void RecordingEngine::stopAndResetPlayback() {
    if (playbackStream.mStream) {
        mRecordingIO.stop_audio_source();
        return;
    }

    closePlaybackStream();
}

void RecordingEngine::stopPlayback() {
    if (playbackStream.mStream == nullptr) {
        return;
    }
    closePlaybackStream();
}

void RecordingEngine::closePlaybackStream() {
    if (playbackStream.mStream != nullptr && playbackStream.mStream->getState() != oboe::StreamState::Closed) {
        playbackStream.stopStream();
        playbackStream.closeStream();
    }
}

void RecordingEngine::pausePlayback() {
    mRecordingIO.pause_audio_source();
    playbackStream.stopStream();
}

void RecordingEngine::startRecording() {
    recordingStream.openRecordingStream();
    if (recordingStream.mStream) {
        recordingStream.startStream();
    } else {
        LOGE(TAG, "startRecording(): Failed to create recording (%p) stream", recordingStream.mStream);
        recordingStream.closeStream();
    }
}

void RecordingEngine::stopRecording() {
    if (!recordingStream.mStream) {
        return;
    }

    if (recordingStream.mStream->getState() != oboe::StreamState::Closed) {
        recordingStream.stopStream();
        recordingStream.closeStream();
        flushWriteBuffer();
    }
}

void RecordingEngine::pauseRecording() {
    recordingStream.stopStream();
    flushWriteBuffer();
}

void RecordingEngine::restartPlayback() {
    stopPlayback();
    startPlayback();
}

void RecordingEngine::flushWriteBuffer() {
    mRecordingIO.flush_buffer();
}

int RecordingEngine::getCurrentMax() {
    return mRecordingIO.getCurrentMax();
}

void RecordingEngine::resetCurrentMax() {
    mRecordingIO.resetCurrentMax();
}

void RecordingEngine::setStopPlayback() {
    call_in_attached_thread([&](auto env) {
        if (mRecordingScreenViewModelPassed && kotlinMethodIdsPtr != nullptr) {
            env->CallStaticVoidMethod(kotlinMethodIdsPtr->recordingScreenVM, kotlinMethodIdsPtr->setStopPlay);
        }
    });
}

int RecordingEngine::getTotalRecordedFrames() {
    return mRecordingIO.getTotalRecordedFrames();
}

int RecordingEngine::getCurrentPlaybackProgress() {
    return mRecordingIO.getCurrentPlaybackProgress();
}

void RecordingEngine::setPlayHead(int position) {
    mRecordingIO.setPlayHead(position);
}

int RecordingEngine::getDurationInSeconds() {
    return mRecordingIO.getDurationInSeconds();
}

void RecordingEngine::resetRecordingEngine() {
    return mRecordingIO.resetProperties();
}


