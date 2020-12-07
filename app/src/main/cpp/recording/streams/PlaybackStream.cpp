//
// Created by asalehin on 7/30/20.
//

#include <cassert>
#include "PlaybackStream.h"
#include "../../streams/BaseStream.h"

PlaybackStream::PlaybackStream(RecordingIO* recordingIO): BaseStream(recordingIO) {}

oboe::Result PlaybackStream::openStream() {
    LOGD(TAG, "openLivePlaybackStream(): ");
    oboe::AudioStreamBuilder builder;
    setupPlaybackStreamParameters(&builder, StreamConstants::mAudioApi, StreamConstants::mPlaybackFormat, this,
                                  StreamConstants::mPlaybackDeviceId, StreamConstants::mPlaybackSampleRate, StreamConstants::mOutputChannelCount);
    oboe::Result result = builder.openStream(mStream);
    if (result == oboe::Result::OK && mStream) {
        assert(mStream->getChannelCount() == StreamConstants::mOutputChannelCount);
//        assert(mStream->getSampleRate() == mSampleRate);
        assert(mStream->getFormat() == StreamConstants::mPlaybackFormat);

        int32_t mFramesPerBurst = mStream->getFramesPerBurst();

        mStream->setBufferSizeInFrames(mFramesPerBurst);

    } else {
        LOGE(TAG, "openPlaybackStream(): Failed to create playback stream. Error: %s",
             oboe::convertToText(result));
    }
    return result;
}

oboe::AudioStreamBuilder *
PlaybackStream::setupPlaybackStreamParameters(oboe::AudioStreamBuilder *builder,
                                          oboe::AudioApi audioApi, oboe::AudioFormat audioFormat,
                                          oboe::AudioStreamDataCallback *audioStreamCallback,
                                          int32_t deviceId, int32_t sampleRate, int channelCount) {
    LOGD(TAG, "setupPlaybackStreamParameters()");
    builder->setAudioApi(audioApi)
            ->setFormat(audioFormat)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setDataCallback(audioStreamCallback)
            ->setErrorCallback(reinterpret_cast<AudioStreamErrorCallback *>(audioStreamCallback))
            ->setDeviceId(deviceId)
            ->setDirection(oboe::Direction::Output)
            ->setSampleRate(sampleRate)
            ->setChannelCount(channelCount);
    return builder;
}

oboe::DataCallbackResult
PlaybackStream::onAudioReady(oboe::AudioStream *audioStream, void *audioData,
                               int32_t numFrames) {
    return processPlaybackFrame(audioStream, static_cast<float_t *>(audioData), numFrames, audioStream->getChannelCount());
}

oboe::DataCallbackResult
PlaybackStream::processPlaybackFrame(oboe::AudioStream *audioStream, float *audioData,
                                       int32_t numFrames, int32_t channelCount) {
    if (audioStream && audioStream->getState() != oboe::StreamState::Closed) {
        fillArrayWithZeros(audioData, numFrames);
        mRecordingIO->read_playback(audioData, numFrames, channelCount);
        return oboe::DataCallbackResult::Continue;
    }
    return oboe::DataCallbackResult::Stop;
}

void PlaybackStream::onErrorAfterClose(oboe::AudioStream *audioStream, oboe::Result result) {
    mStream.reset();
}
