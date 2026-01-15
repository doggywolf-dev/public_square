#pragma once

#include <string>
#include <deque>
#include <optional>
#include <stdexcept>
#include <opencv2/opencv.hpp>

namespace NTestTracker {

/**
 * @class TTestTracker
 * @brief Manages the entire pipeline for tracking an object in a video.
 *
 * This class is responsible for opening a video source, applying a series of
 * configurable preprocessing filters (frame averaging and median filtering),
 * and then running an object tracking algorithm using a specified model.
 */
class TTestTracker {
public:
    /**
     * Constructs a TTestTracker object with specified configurations.
     */    TTestTracker(
        const std::string& videoData,
        const std::string& model,
        const std::optional<size_t> aveListSize = std::nullopt,
        const std::optional<size_t> medianFilterWindowSize = std::nullopt)
        : VideoData(videoData)
        , Model(model)
        , AveragingListSize(aveListSize)
        , MedianFilterWindowSize(medianFilterWindowSize)
    {
        if (VideoData.empty() || Model.empty()) {
            throw std::invalid_argument("VideoData and Model cannot be empty");
        }
    }

    /**
     * Executes the main tracking loop.
     *
     * This method opens the video source, reads frames sequentially, applies the
     * configured preprocessing steps, performs tracking, and displays the results.
     * It runs until the video ends or an exit condition is met.
     */
    [[nodiscard]] int Run();

protected:
    // Frame Preprocessing Methods
    // Maintains running average of frames using circular buffer
    // This filter is effective against Gaussian noise.
    void AverageFrames(const cv::Mat& currentFrame);

    // Applies median filter to averaged frame using specified window size
    // The median blur is effective against "salt-and-pepper" noise.
    void FilterFrame();

private:
    const std::string VideoData; // Source video file path
    const std::string Model; // Model file path (e.g., for neural network)

    // Optional processing parameters
    const std::optional<size_t> AveragingListSize {std::nullopt}; // number of frames to average
    const std::optional<size_t> MedianFilterWindowSize {std::nullopt};

    // Frame processing buffers
    std::deque<cv::Mat> FrameListForPreprocessing; // list of frames to average
    cv::Mat sumFrame; // CV_32F
    cv::Mat averageFrame; // CV_8U
    cv::Mat filteredFrame; // CV_8U
};

}  // namespace NTestTracker
