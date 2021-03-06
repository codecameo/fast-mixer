/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "../logging_macros.h"
#include <oboe/Oboe.h>
#include <regex>
#include <string>
#include <sys/stat.h>
#include "FileDataSource.h"
#include "FFMpegExtractor.h"
#include <regex>

constexpr int kMaxCompressionRatio { 12 };

FileDataSource::FileDataSource (
        unique_ptr<float[]> data,
        size_t size,
        const AudioProperties properties) :
        mBuffer(move(data)), mBufferSize(size), mProperties(properties) {

    for (int i = 0; i < mBufferSize; i++) {
        if (abs(mBuffer[i]) > mMaxSampleValue) {
            mMaxSampleValue = abs(mBuffer[i]);
        }
    }
}

FileDataSource* FileDataSource::newFromCompressedFile(
        const char *filename,
        const AudioProperties targetProperties) {
    string filenameStr(filename);

    FILE* fl = fopen(filenameStr.c_str(), "r");
    if (!fl) {
        LOGE("Failed to open asset %s", filenameStr.c_str());
        fclose(fl);
        return nullptr;
    }
    fclose(fl);

    off_t assetSize = getSizeOfFile(filenameStr.c_str());

    // Allocate memory to store the decompressed audio. We don't know the exact
    // size of the decoded data until after decoding so we make an assumption about the
    // maximum compression ratio and the decoded sample format (float for FFmpeg, int16 for NDK).

    auto ffmpegExtractor = FFMpegExtractor(filenameStr, targetProperties);

    long maximumDataSizeInBytes = kMaxCompressionRatio * assetSize * sizeof(float);

    auto decodedData = new uint8_t [maximumDataSizeInBytes];

    int64_t bytesDecoded = ffmpegExtractor.decode(decodedData);

    if (bytesDecoded <= 0) {
        return nullptr;
    }

    auto numSamples = bytesDecoded / sizeof(float);

    // Now we know the exact number of samples we can create a float array to hold the audio data
    auto outputBuffer = make_unique<float[]>(numSamples);
    memcpy(outputBuffer.get(), decodedData, (size_t)bytesDecoded);

    delete [] decodedData;

    return new FileDataSource(move(outputBuffer),
                              numSamples,
                              move(targetProperties));
}

unique_ptr<buffer_data> FileDataSource::readData(size_t countPoints) {
    int channelCount = mProperties.channelCount;

    size_t samplesToHandle;

    if (countPoints * channelCount > mBufferSize) {
        samplesToHandle = (int) (mBufferSize / channelCount);
    } else {
        samplesToHandle = countPoints;
    }

    int ptsDistance = (int) (mBufferSize / (samplesToHandle * channelCount));

    auto selectedSamples = new float [samplesToHandle];
    for (int i = 0; i < samplesToHandle; i++) {
        float maxValue = 0;
        for (int j = i * ptsDistance; j < (i + 1) * ptsDistance; j += channelCount) {
            if (abs(mBuffer[j]) > maxValue) {
                maxValue = abs(mBuffer[j]);
            }
        }
        selectedSamples[i] = maxValue;
    }

    buffer_data buff = {
            .ptr = selectedSamples,
            .countPoints = samplesToHandle
    };
    return make_unique<buffer_data>(buff);
}

void FileDataSource::setPlayHead(int64_t playHead) {
    mPlayHead = playHead;
}

int64_t FileDataSource::getPlayHead() {
    return mPlayHead;
}
