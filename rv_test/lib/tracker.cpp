#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <chrono>

#include "tracker.h"

namespace NTestTracker {

// Model and Inference Constants
// The input image size (width and height) required by the ONNX model.
constexpr size_t IMAGE_SIZE_FOR_ONNX = 640;

// The confidence threshold for filtering detected objects. Detections below this value will be ignored.
constexpr float CONF_THRESHOLD  = 0.25f;

// The total number of classes the model is trained to detect.
constexpr int NUM_CLASSES = 3;

// Display Constants
// Default width for the application's display window.
constexpr size_t WINDOW_WIDTH_FOR_SHOW = 1280;
constexpr size_t WINDOW_HEIGHT_FOR_SHOW = 720;

int TTestTracker::Run() {
    // 1. Initialization and Information Logging
    std::cout << "System Information" << std::endl;
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    std::cout << "ONNX Runtime version: " << OrtGetApiBase()->GetVersionString() << std::endl;
    std::cout << "Video source: " << VideoData << std::endl;
    std::cout << "Model path: " << Model << std::endl;

    // 2. Video Capture Setup
    cv::VideoCapture video(VideoData);
    if (!video.isOpened()) {
        std::cout << "Error: Could not open video source!" << std::endl;
        return -1;
    }

    // Retrieve and log video properties
    int totalFrames = static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
    int width = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = video.get(cv::CAP_PROP_FPS);

    std::cout << "\nVideo Information" << std::endl;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Frame count: " << totalFrames << std::endl;
    std::cout << "Resolution: " << width << "x" << height << std::endl;

    // 3. ONNX Runtime Initialization
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "YOLOv8-Tracker");
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(4);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    // Ensure the model file exists before attempting to load it.
    if (!std::filesystem::exists(Model)) {
        std::cerr << "The model was not found: " << Model << std::endl;
        return -1;
    }

    // Create an inference session from the model file.
    Ort::Session session = Ort::Session(env, Model.c_str(), session_options);

    // Get the model's input and output layer names dynamically.
    Ort::AllocatorWithDefaultOptions allocator;

    const auto inputNameAllocated = session.GetInputNameAllocated(0, allocator);
    const auto outputNameAllocated = session.GetOutputNameAllocated(0, allocator);

    const char* inputName = inputNameAllocated.get();
    const char* outputName = outputNameAllocated.get();

    // 4. Pre-Loop Setup
    cv::namedWindow("Result", cv::WINDOW_NORMAL);
    cv::resizeWindow("Result", WINDOW_WIDTH_FOR_SHOW, WINDOW_HEIGHT_FOR_SHOW);

    const std::vector<std::string> classNames = {"airplanes", "birds", "kites"};
    const std::vector<cv::Scalar> colors = {
        cv::Scalar(255, 0, 0),    // Blue for airplanes
        cv::Scalar(0, 255, 0),    // Green for birds
        cv::Scalar(0, 0, 255)     // Red for kites
    };

    // Prepare a buffer for the input tensor.
    const std::vector<int64_t> inputShape = {1, 3, IMAGE_SIZE_FOR_ONNX, IMAGE_SIZE_FOR_ONNX};
    const size_t inputTensorSize = 1 * 3 * IMAGE_SIZE_FOR_ONNX * IMAGE_SIZE_FOR_ONNX;
    std::vector<float> inputTensorValues(inputTensorSize);

    // 5. Main Processing Loop
    std::cout << "\nProcessing... (Press ESC to exit, Space to pause)\n" << std::endl;
    const auto startTime = std::chrono::high_resolution_clock::now();

    cv::Mat frame;
    int frameCount = 0;
    bool firstFrame = true;

    while (video.read(frame)) {
        if (frame.empty()) {
            std::cout << "End of video stream." << std::endl;
            break;
        }

        frameCount++;

        try {
            // 5.1 Frame Preprocessing (Temporal and Spatial Filtering)
            AverageFrames(frame); // Apply moving average filter if enabled.
            FilterFrame(); // Apply median blur filter if enabled.
            // `filteredFrame` now contains the result of these optional steps.

            // 5.2 Inference Preprocessing (Manual Tensor Preparation)
            // The model requires a 640x640 RGB image, normalized to [0, 1], in NCHW format.
            // Resize the frame to the model's required input dimensions.
            cv::Mat resizedFilteredFrame;
            cv::resize(filteredFrame, resizedFilteredFrame, cv::Size(IMAGE_SIZE_FOR_ONNX, IMAGE_SIZE_FOR_ONNX));

            // Convert from BGR (OpenCV's default) to RGB, which the model expects.
            cv::Mat rgbFrame;
            cv::cvtColor(resizedFilteredFrame, rgbFrame, cv::COLOR_BGR2RGB);

            // Normalize pixel values from the [0, 255] integer range to the [0.0, 1.0] float range.
            cv::Mat floatRgbFrame;
            rgbFrame.convertTo(floatRgbFrame, CV_32F, 1.0 / 255.0);

            // Convert the image from HWC (Height, Width, Channels) to NCHW (Batch, Channels, Height, Width) format.
            std::vector<cv::Mat> channels(3);
            cv::split(floatRgbFrame, channels);

            for (int c = 0; c < 3; ++c) {
                if (!channels[c].isContinuous()) {
                    throw std::runtime_error(
                        "Channel " + std::to_string(c) + " is not continuous in memory"
                    );
                }

                memcpy(
                    inputTensorValues.data() + c * IMAGE_SIZE_FOR_ONNX * IMAGE_SIZE_FOR_ONNX,
                    channels[c].data,
                    IMAGE_SIZE_FOR_ONNX * IMAGE_SIZE_FOR_ONNX * sizeof(float)
                );
             }

            // 5.3 Create ONNX Tensor and Run Inference
            auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
            Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
                memoryInfo,
                inputTensorValues.data(),
                inputTensorSize,
                inputShape.data(),
                inputShape.size()
            );

            std::vector<const char*> inputNamesVec = {inputName};
            std::vector<const char*> outputNamesVec = {outputName};

            auto outputTensors = session.Run(
                Ort::RunOptions{nullptr},
                inputNamesVec.data(),
                &inputTensor,
                1,
                outputNamesVec.data(),
                1
            );

            // 5.4 Post-processing (Parse and Draw Detections)
            float* outputData = outputTensors[0].GetTensorMutableData<float>();
            auto outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();

            if (firstFrame) {
                std::cout << "Model Output Shape: [";
                for (size_t i = 0; i < outputShape.size(); ++i) {
                    std::cout << outputShape[i];
                    if (i < outputShape.size() - 1) {
                        std::cout << ", ";
                    }
                }
                std::cout << "]" << std::endl;
                firstFrame = false;
            }

            int numDetections = outputShape[1];  // 300
            int detectionSize = outputShape[2];  // 6

            // Calculate scaling factors to map detections from the model's input size (640x640) back to the original frame size.
            const float xScale = static_cast<float>(frame.cols) / IMAGE_SIZE_FOR_ONNX;
            const float yScale = static_cast<float>(frame.rows) / IMAGE_SIZE_FOR_ONNX;

            // Draw on a copy of the pristine, original frame.
            cv::Mat resultFrame = frame.clone();
            int validDetections = 0;
            int textThickness = 1;

            // The model output is expected to be already post-processed with NMS.
            // The format is [x_left, y_top, x_right, y_bottom, confidence, class_id] for each detection
            for (int i = 0; i < numDetections; ++i) {
                const int baseIdx = i * detectionSize;
                const float confidence = outputData[baseIdx + 4];
                const int classId = static_cast<int>(outputData[baseIdx + 5]);

                // Filter out low-confidence detections and invalid classes.
                if (confidence < CONF_THRESHOLD || classId < 0 || classId >= NUM_CLASSES) {
                    continue;
                }

                // Scale bounding box coordinates to the original frame's dimensions.
                int left = static_cast<int>(outputData[baseIdx + 0] * xScale);
                int top = static_cast<int>(outputData[baseIdx + 1] * yScale);
                int right = static_cast<int>(outputData[baseIdx + 2] * xScale);
                int bottom = static_cast<int>(outputData[baseIdx + 3] * yScale);

                // Clamp coordinates to be within frame boundaries to prevent drawing errors.
                left = std::max(0, std::min(left, frame.cols - 1));
                top = std::max(0, std::min(top, frame.rows - 1));
                right = std::max(0, std::min(right, frame.cols));
                bottom = std::max(0, std::min(bottom, frame.rows));

                if (right <= left || bottom <= top) {
                    continue; // Skip invalid boxes with zero or negative area.
                }

                cv::Rect box(left, top, right - left, bottom - top);
                validDetections++;

                // Draw the bounding box.
                int lineThickness = 2;
                cv::Scalar color = colors[classId];
                cv::rectangle(resultFrame, box, color / 2, lineThickness + 1); // Dark colored rectangle for the outline
                cv::rectangle(resultFrame, box, color, lineThickness);

                // Prepare the label text with class name and confidence score.
                std::string label = classNames[classId] + " " + cv::format("%.2f", confidence);

                // Define text properties
                int fontFace = cv::FONT_HERSHEY_SIMPLEX;
                double fontScale = 0.7;
                cv::Point text_origin(left, top - 10); // Position just above the bounding box

                // Draw a dark outline ("shadow") for the text first
                cv::putText(
                    resultFrame,
                    label,
                    text_origin,
                    fontFace,
                    fontScale,
                    color / 2, // Dark color for the outline
                    textThickness + 1, // Make the outline slightly thicker
                    cv::LINE_AA
                );

                // Draw the main text in color on top of the outline
                cv::putText(
                    resultFrame,
                    label,
                    text_origin,
                    fontFace,
                    fontScale,
                    color, // The actual color of the text
                    textThickness, // Main text thickness
                    cv::LINE_AA
                );
            }

            // 5.5 Logging and Display
            if (frameCount % 30 == 0) {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
                std::cout << "Frame " << frameCount << "/" << totalFrames
                          << " | Time elapsed: " << elapsed << "s | Detections in frame: " << validDetections << std::endl;
            }

            // Draw overlay information on the frame for visual feedback.
            std::string frameInfo = cv::format("Frame: %d/%d | Detections: %d",
                frameCount, totalFrames, validDetections);

            cv::putText(resultFrame, frameInfo, cv::Point(10, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.7,
                       cv::Scalar(127, 0, 127), textThickness + 1, cv::LINE_AA);
            cv::putText(resultFrame, frameInfo, cv::Point(10, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.7,
                       cv::Scalar(255, 0, 255), textThickness, cv::LINE_AA);

            cv::imshow("Result", resultFrame);

        } catch (const std::exception& e) {
            std::cerr << "Frame " << frameCount << ": " << e.what() << std::endl;
            continue;
        }

        // Handle user input for interactivity.
        int key = cv::waitKey(1); // Wait 1ms for a key press.
        if (key == 27) { // 27 is the ASCII code for the ESC key.
            std::cout << "\nProcessing stopped by user." << std::endl;
            break;
        } else if (key == 32) { // 32 is the ASCII code for the Space bar.
            std::cout << "Paused. Press any key to continue..." << std::endl;
            cv::waitKey(0); // Wait indefinitely for a key press to unpause.
        }
    }

    // 6. Cleanup
    video.release();
    cv::destroyAllWindows();

    std::cout << "\nTotal frames processed: " << frameCount << std::endl;

    return 0;
};

void TTestTracker::AverageFrames(const cv::Mat& currentFrame) {
    if (currentFrame.empty()) {
        throw std::runtime_error("Cannot average an empty frame.");
    }

    size_t actualListSize = AveragingListSize.value_or(1);

    // If averaging is disabled (window size is 1 or not set), simply clone the current frame.
    if (actualListSize <= 1) {
        averageFrame = currentFrame.clone();
        return;
    }

    // Convert the input frame to 32-bit float for accurate summation to prevent pixel value overflow.
    cv::Mat floatMat;
    currentFrame.convertTo(floatMat, CV_32F);

    if (FrameListForPreprocessing.empty()) {
        sumFrame = floatMat.clone();
    } else {
        sumFrame = sumFrame + floatMat;
    }

    FrameListForPreprocessing.push_back(floatMat.clone());

    // If the window is full, remove the oldest frame from the sum and from the deque.
    if (FrameListForPreprocessing.size() > actualListSize) {
        sumFrame -= FrameListForPreprocessing.front();
        FrameListForPreprocessing.pop_front();
    }

    // Calculate the average and convert it back to 8-bit unsigned char for display/further processing.
    cv::Mat avgMat = sumFrame / static_cast<double>(FrameListForPreprocessing.size());
    avgMat.convertTo(averageFrame, CV_8U);

    return;
};

void TTestTracker::FilterFrame() {
    if (averageFrame.empty()) {
        throw std::runtime_error("Cannot filter an empty frame. 'AverageFrames' must be called first.");
    }

    // Get the median filter window size. If not set, default to 1 (which means no filtering).
    // The median filter kernel size must be an odd number greater than 1.
    // This bitwise OR trick efficiently ensures the size is odd (e.g., 4|1=5, 5|1=5).
    size_t actualMedianWindowSize = MedianFilterWindowSize.value_or(1) | 1;

    if (actualMedianWindowSize <= 1) {
        // If window size is 1, no filtering is needed; just copy the input to the output.
        averageFrame.copyTo(filteredFrame);
    } else {
        // Apply the median blur. This is effective against "salt-and-pepper" noise.
        cv::medianBlur(averageFrame, filteredFrame, actualMedianWindowSize);
    }

    return;
}

}  // namespace NTestTracker

