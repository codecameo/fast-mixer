//
// Created by asalehin on 7/30/20.
//

#ifndef FAST_MIXER_BASESTREAM_H
#define FAST_MIXER_BASESTREAM_H

#include "oboe/Definitions.h"
#include "oboe/Utilities.h"
#include "oboe/AudioStream.h"
#include "StreamConstants.h"
#include "../logging_macros.h"
#include "../recording/RecordingIO.h"

class BaseStream {
public:
    BaseStream(RecordingIO* recordingIO);

    RecordingIO* mRecordingIO;

    oboe::AudioStream *mStream = nullptr;

    void startStream();
    void stopStream();
    void closeStream();

private:
    const char* TAG = "Stream Processor:: %s";
};


#endif //FAST_MIXER_BASESTREAM_H
