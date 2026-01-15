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
    //Info
    std::cout << "OpenCV версия: " << CV_VERSION << std::endl;
	std::cout << "ONNX Runtime версия: " << OrtGetApiBase()->GetVersionString() << std::endl;
    std::cout << "Имя файла: " << VideoData << std::endl;
    std::cout << "Путь до модели: " << Model << std::endl;

    // Открытие видео
    cv::VideoCapture video(VideoData);
    if (!video.isOpened()) {
        std::cout << "Ошибка открытия видео!" << std::endl;
        return -1;
    }

    int totalFrames = static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
    int width = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = video.get(cv::CAP_PROP_FPS);

    // Вывод информации о видео
	std::cout << "\nИнформация о видео:" << std::endl;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Кадров: " << totalFrames << std::endl;
    std::cout << "Ширина: " << width << std::endl;
    std::cout << "Высота: " << height << std::endl;

	
    // Инициализация ONNX Runtime
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "YOLOv8");
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(4);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

	if (!std::filesystem::exists(Model)) {
		std::cerr << "Модель не найдена: " << Model << std::endl;
		return -1;
	}

    Ort::Session session = Ort::Session(env, Model.c_str(), session_options);

    // Получаем имена входов/выходов
    Ort::AllocatorWithDefaultOptions allocator;

    const auto inputNameAllocated = session.GetInputNameAllocated(0, allocator);
    const auto outputNameAllocated = session.GetOutputNameAllocated(0, allocator);

    const char* inputName = inputNameAllocated.get();
    const char* outputName = outputNameAllocated.get();

    cv::namedWindow("Result", cv::WINDOW_NORMAL);
    cv::resizeWindow("Result", 1280, 720);

	// Классы
	std::vector<std::string> classNames = {"airplanes", "birds", "kites"};

	// Цвета для отрисовки классов
	std::vector<cv::Scalar> colors = {
		cv::Scalar(255, 0, 0),    // Синий
		cv::Scalar(0, 255, 0),    // Зелёный
		cv::Scalar(0, 0, 255)     // Красный
	};

    cv::Mat frame;
    int frameCount = 0;
	bool firstFrame = true;
	
	std::vector<int64_t> inputShape = {1, 3, IMAGE_SIZE_FOR_ONNX, IMAGE_SIZE_FOR_ONNX};
    size_t inputTensorSize = 1 * 3 * IMAGE_SIZE_FOR_ONNX * IMAGE_SIZE_FOR_ONNX;
	
	std::cout << "\nОбработка... (ESC - выход, Пробел - пауза)\n" << std::endl;
	auto startTime = std::chrono::high_resolution_clock::now();

    while (video.read(frame)) {
		if (frame.empty()) {
			break;
		}
		
        frameCount++;
		
		try {
           // Предобработка кадров
		   // Усреднение по кадрам
			AverageFrames(frame);
			// Скользящая медиана по пикселям в оригинальном разрешении
			FilterFrame();

            // Подготовка для YOLO
            cv::Mat resizedFilteredFrame;
            cv::resize(filteredFrame, resizedFilteredFrame, cv::Size(IMAGE_SIZE_FOR_ONNX, IMAGE_SIZE_FOR_ONNX));
            
            cv::Mat rgbFrame;
            cv::cvtColor(resizedFilteredFrame, rgbFrame, cv::COLOR_BGR2RGB);
            
            cv::Mat floatRgbFrame;
            rgbFrame.convertTo(floatRgbFrame, CV_32F, 1.0 / 255.0);

            // Преобразование в NCHW
            std::vector<float> inputTensorValues(inputTensorSize);
            std::vector<cv::Mat> channels(3);
            cv::split(floatRgbFrame, channels);
            
            for (int c = 0; c < 3; ++c) {
                memcpy(
                    inputTensorValues.data() + c * IMAGE_SIZE_FOR_ONNX * IMAGE_SIZE_FOR_ONNX,
                    channels[c].data,
                    IMAGE_SIZE_FOR_ONNX * IMAGE_SIZE_FOR_ONNX * sizeof(float)
                );
            }

            // Создаём тензор
            auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
            Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
                memoryInfo,
                inputTensorValues.data(),
                inputTensorSize,
                inputShape.data(),
                inputShape.size()
            );

            // Инференс
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
			
            // Получаем результаты [1, 300, 6]
            float* outputData = outputTensors[0].GetTensorMutableData<float>();
            auto outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
            
            if (firstFrame) {
                std::cout << "Output format: [";
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

            float xScale = static_cast<float>(frame.cols) / IMAGE_SIZE_FOR_ONNX;
            float yScale = static_cast<float>(frame.rows) / IMAGE_SIZE_FOR_ONNX;

            cv::Mat resultFrame = frame.clone();
            int validDetections = 0;

			// Обработка детекций: [xLeft, yTop, xRight, yBottom, confidence, classId]
			for (int i = 0; i < numDetections; ++i) {
				int baseIdx = i * detectionSize;
				
				float xLeft = outputData[baseIdx + 0];
				float yTop = outputData[baseIdx + 1];
				float xRight    = outputData[baseIdx + 2];
				float yBottom   = outputData[baseIdx + 3];
				float confidence = outputData[baseIdx + 4];
				int classId = static_cast<int>(outputData[baseIdx + 5]);
				
				// Фильтрация
				if (confidence < CONF_THRESHOLD || classId < 0 || classId >= NUM_CLASSES) {
					continue;
				}
				
				// Масштабируем под исходное разрешение кадра
				int left   = static_cast<int>(xLeft * xScale);
				int top    = static_cast<int>(yTop * yScale);
				int right  = static_cast<int>(xRight * xScale);
				int bottom = static_cast<int>(yBottom * yScale);
				
				// Ограничиваем границами кадра
		        if ((std::min({left, right, top, bottom}) < 0) || (std::max(left, right) >= frame.cols) || (std::max(top, bottom) >= frame.rows)) {
				    std::cout << "The borders of the bbox are outside the frame: left=" << left << ", top=" << top << ", right=" << right << " bottom=" << bottom << std::endl;
				}
				left   = std::max(0, std::min(left,   frame.cols - 1));
				top    = std::max(0, std::min(top,    frame.rows - 1));
				right  = std::max(0, std::min(right,  frame.cols));
				bottom = std::max(0, std::min(bottom, frame.rows));
				
				if (right <= left || bottom <= top) {
				    std::cout << "Incorrect bbox borders: left=" << left << ", top=" << top << ", right=" << right << " bottom=" << bottom << std::endl;
					continue;
				}
				
				cv::Rect box(left, top, right - left, bottom - top);
				validDetections++;                
                
                cv::Scalar color = colors[classId];
                cv::rectangle(resultFrame, box, color, 2);
                
                std::string label = classNames[classId] + " " + 
                                   cv::format("%.2f", confidence);
                
                int baseline = 0;
                cv::Size labelSize = cv::getTextSize(
                    label, cv::FONT_HERSHEY_SIMPLEX, 0.6, 1, &baseline
                );
                
                int labelTop = std::max(top, labelSize.height + 8);
                
                cv::rectangle(
                    resultFrame,
                    cv::Point(left, labelTop - labelSize.height - 8),
                    cv::Point(left + labelSize.width + 4, labelTop + baseline),
                    color, cv::FILLED
                );
                
                cv::putText(
                    resultFrame, label,
                    cv::Point(left + 2, labelTop - 4),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(255, 255, 255), 1, cv::LINE_AA
                );
            }

            // Логирование
            if (frameCount % 30 == 0) {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
                std::cout << "Frame " << frameCount << "/" << totalFrames 
                          << " | Time: " << elapsed << "s | Detections: " << validDetections << std::endl;
            }

            // Инфо на кадре
            std::string frameInfo = cv::format("Frame: %d/%d | Detections: %d", 
                frameCount, totalFrames, validDetections);
            
            cv::putText(resultFrame, frameInfo, cv::Point(10, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.7,
                       cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

            cv::imshow("Result", resultFrame);
            
        } catch (const std::exception& e) {
            std::cerr << "Frame " << frameCount << ": " << e.what() << std::endl;
            continue;
        }
        
        int key = cv::waitKey(1);
        if (key == 27) {
            std::cout << "\nОстановлено" << std::endl;
            break;
        }
    }

    // Освобождаем ресурсы
    video.release();
    cv::destroyAllWindows();
	
	std::cout << "\nОбработано кадров: " << frameCount << std::endl;

    return 0;
};

void TTestTracker::AverageFrames(const cv::Mat& currentFrame) {
    if (currentFrame.empty()) {
        throw std::runtime_error("Empty frame in AverageFrames");
    }

    cv::Mat floatMat;
    currentFrame.convertTo(floatMat, CV_32F);

    size_t actualListSize = AveragingListSize.value_or(1);

    // One-frame window (no averaging)
    if (actualListSize <= 1) {
        averageFrame = currentFrame.clone();
        return;
    }

    // Add new frame to sum for мoving average
    if (FrameListForPreprocessing.empty()) {
        sumFrame = floatMat.clone();
    } else {
        sumFrame = sumFrame + floatMat;
    }

    FrameListForPreprocessing.push_back(floatMat.clone());

    // Remove oldest frame if window is full
    if (FrameListForPreprocessing.size() > actualListSize) {
        sumFrame -= FrameListForPreprocessing.front();
        FrameListForPreprocessing.pop_front();
    }

    // Calculate average
    cv::Mat avgMat = sumFrame / static_cast<double>(FrameListForPreprocessing.size());
    avgMat.convertTo(averageFrame, CV_8U);
   
    return;
};

void TTestTracker::FilterFrame() {
    if (averageFrame.empty()) {
        throw std::runtime_error("averageFrame is empty in FilterFrame");
    }

    size_t actualMedianWindowSize = MedianFilterWindowSize.value_or(1) | 1;

    if (actualMedianWindowSize <= 1) {
        averageFrame.copyTo(filteredFrame);
    } else {
        cv::medianBlur(averageFrame, filteredFrame, actualMedianWindowSize);
    }
	
	return;
}

}  // namespace NTestTracker

