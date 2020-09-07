//
// Created by asalehin on 7/30/20.
//

#include "BaseStream.h"

BaseStream::BaseStream(RecordingIO* recordingIO) {
    mRecordingIO = recordingIO;
}

void BaseStream::startStream() {
    LOGD(TAG, "startStream(): ");
    if (mStream) {
        oboe::Result result = mStream->requestStart();
        if (result != oboe::Result::OK) {
            LOGE(TAG, "Error starting the stream: %s", oboe::convertToText(result));
        }
    }
}

void BaseStream::stopStream() {
    LOGD("stopStream(): ");
    if (mStream && mStream->getState() != oboe::StreamState::Closed) {
        oboe::Result result = mStream->stop(0L);
        if (result != oboe::Result::OK) {
            LOGE(TAG, "Error stopping the stream: %s");
            LOGE(TAG, oboe::convertToText(result));
        }
        LOGW(TAG, "stopStream(): Total samples = ");
        LOGW(TAG, std::to_string(mRecordingIO->getTotalSamples()).c_str());
    }
}

void BaseStream::closeStream() {
    LOGD("closeStream(): ");

    if (mStream) {
        oboe::Result result = mStream->close();
        if (result != oboe::Result::OK) {
            LOGE(TAG, "Error closing stream. %s", oboe::convertToText(result));
        } else {
            mStream = nullptr;
        }

        LOGW(TAG, "closeStream(): mTotalSamples = ");
        LOGW(TAG, std::to_string(mRecordingIO->getTotalSamples()).c_str());
    }
}
