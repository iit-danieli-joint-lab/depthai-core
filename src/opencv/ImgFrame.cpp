#include "depthai/pipeline/datatype/ImgFrame.hpp"

#include <cmath>
#include <opencv2/imgproc.hpp>

// #include "spdlog/spdlog.h"

namespace dai {

ImgFrame& ImgFrame::setFrame(cv::Mat frame) {
    std::vector<uint8_t> dataVec;
    assert(frame.isContinuous());
    dataVec.insert(dataVec.begin(), frame.datastart, frame.dataend);
    setData(dataVec);
    return *this;
}

cv::Mat ImgFrame::getFrame(bool deepCopy) {
    // Convert to cv::Mat. If deepCopy enabled, then copy pixel data, otherwise reference only
    cv::Mat mat;
    cv::Size size = {0, 0};
    int type = 0;

    switch(getType()) {
        case Type::RGB888i:
        case Type::BGR888i:
            size = cv::Size(getWidth(), getHeight());
            type = CV_8UC3;
            break;
        case Type::BGR888p:
        case Type::RGB888p:
            size = cv::Size(getWidth(), getHeight() * 3);
            type = CV_8UC1;
            break;
        case Type::YUV420p:
        case Type::NV12:
        case Type::NV21:
            size = cv::Size(getWidth(), getPlaneHeight() * 3 / 2);
            type = CV_8UC1;
            break;

        case Type::RAW8:
        case Type::GRAY8:
            size = cv::Size(getWidth(), getHeight());
            type = CV_8UC1;
            break;

        case Type::GRAYF16:
            size = cv::Size(getWidth(), getHeight());
            type = CV_16FC1;
            break;

        case Type::RAW16:
        case Type::RAW14:
        case Type::RAW12:
        case Type::RAW10:
            size = cv::Size(getWidth(), getHeight());
            type = CV_16UC1;
            break;

        case Type::RGBF16F16F16i:
        case Type::BGRF16F16F16i:
        case Type::RGBF16F16F16p:
        case Type::BGRF16F16F16p:
            size = cv::Size(getWidth(), getHeight());
            type = CV_16FC3;
            break;

        case Type::BITSTREAM:
        default:
            size = cv::Size(static_cast<int>(getData().size()), 1);
            type = CV_8UC1;
            break;
    }

    // Check if enough data
    long requiredSize = CV_ELEM_SIZE(type) * size.area();

    // TMP TMP
    // long actualSize = static_cast<long>(img.data.size());
    long actualSize = static_cast<long>(data->getSize());

    if(actualSize < requiredSize) {
        throw std::runtime_error("ImgFrame doesn't have enough data to encode specified frame, required " + std::to_string(requiredSize) + ", actual "
                                 + std::to_string(actualSize) + ". Maybe metadataOnly transfer was made?");
    } else if(actualSize > requiredSize) {
        // FIXME doesn't build on Windows (multiple definitions during link)
        // logger::warn("ImgFrame has excess data: actual {}, expected {}", actualSize, requiredSize);
    }
    if(getWidth() <= 0 || getHeight() <= 0) {
        throw std::runtime_error("ImgFrame metadata not valid (width or height = 0)");
    }

    // Copy or reference to existing data
    if(deepCopy) {
        // Create new image data
        mat.create(size, type);

        // TMPTMP
        // Copy number of bytes that are available by Mat space or by img data size
        // std::memcpy(mat.data, img.data.data(), std::min((long)(img.data.size()), (long)(mat.dataend - mat.datastart)));
        std::memcpy(mat.data, data->getData().data(), std::min((long)(data->getSize()), (long)(mat.dataend - mat.datastart)));
        // TODO stride handling
    } else {
        // TMP TMP
        if(fb.stride != 0) {
            mat = cv::Mat(size, type, data->getData().data());
        } else {
            mat = cv::Mat(size, type, data->getData().data(), getStride());
        }
    }

    return mat;
}

cv::Mat ImgFrame::getCvFrame() {
    cv::Mat frame = getFrame();
    cv::Mat output;

    switch(getType()) {
        case Type::RGB888i:
            cv::cvtColor(frame, output, cv::ColorConversionCodes::COLOR_RGB2BGR);
            break;

        case Type::BGR888i:
            output = frame.clone();
            break;

        case Type::RGB888p: {
            cv::Size s(getWidth(), getHeight());
            std::vector<cv::Mat> channels;
            // RGB
            channels.push_back(cv::Mat(s, CV_8UC1, (uint8_t*)getData().data() + s.area() * 2));
            channels.push_back(cv::Mat(s, CV_8UC1, (uint8_t*)getData().data() + s.area() * 1));
            channels.push_back(cv::Mat(s, CV_8UC1, (uint8_t*)getData().data() + s.area() * 0));
            cv::merge(channels, output);
        } break;

        case Type::BGR888p: {
            cv::Size s(getWidth(), getHeight());
            std::vector<cv::Mat> channels;
            // BGR
            channels.push_back(cv::Mat(s, CV_8UC1, (uint8_t*)getData().data() + s.area() * 0));
            channels.push_back(cv::Mat(s, CV_8UC1, (uint8_t*)getData().data() + s.area() * 1));
            channels.push_back(cv::Mat(s, CV_8UC1, (uint8_t*)getData().data() + s.area() * 2));
            cv::merge(channels, output);
        } break;

        case Type::YUV420p:
            cv::cvtColor(frame, output, cv::ColorConversionCodes::COLOR_YUV2BGR_IYUV);
            break;

        case Type::NV12:
        case Type::NV21: {
            int code = (getType() == Type::NV12) ? cv::ColorConversionCodes::COLOR_YUV2BGR_NV12 : cv::ColorConversionCodes::COLOR_YUV2BGR_NV21;
            if(getPlaneHeight() <= getHeight() && getStride() <= getWidth()) {
                cv::cvtColor(frame, output, code);
            } else {
                cv::Size s(getWidth(), getHeight());
                int type = CV_8UC1;
                int step = getStride();
                cv::Mat frameY(s, type, getData().data(), step);
                cv::Mat frameUV(s / 2, type, getData().data() + getPlaneStride(), step);
                cv::cvtColorTwoPlane(frameY, frameUV, output, code);
            }
        } break;

        case Type::RAW8:
        case Type::RAW16:
        case Type::RAW14:
        case Type::RAW12:
        case Type::RAW10:
        case Type::GRAY8:
        case Type::GRAYF16:
            output = frame.clone();
            break;

        default:
            output = frame.clone();
            break;
    }

    return output;
}

ImgFrame& ImgFrame::setCvFrame(cv::Mat mat, Type type) {
    cv::Mat output;
    setType(type);
    setSize(mat.cols, mat.rows);
    unsigned int size = mat.cols * mat.rows;
    switch(type) {
        case Type::RGB888i:
            fb.width = mat.cols;
            fb.height = mat.rows;
            fb.stride = mat.cols * 3;
            cv::cvtColor(mat, output, cv::ColorConversionCodes::COLOR_BGR2RGB);
            setFrame(output);
            break;

        case Type::BGR888i:
            fb.width = mat.cols;
            fb.height = mat.rows;
            fb.stride = mat.cols * 3;
            setFrame(mat);
            break;

        case Type::RGB888p: {
            fb.width = mat.cols;
            fb.height = mat.rows;
            fb.stride = mat.cols;
            fb.p1Offset = 0;
            fb.p2Offset = size;
            fb.p3Offset = size * 2;
            std::vector<uint8_t> dataVec;
            std::vector<cv::Mat> channels;
            cv::split(mat, channels);
            dataVec.insert(dataVec.end(), channels[2].datastart, channels[2].dataend);
            dataVec.insert(dataVec.end(), channels[1].datastart, channels[1].dataend);
            dataVec.insert(dataVec.end(), channels[0].datastart, channels[0].dataend);
            setData(dataVec);
            size_t offset = channels[0].dataend - channels[0].datastart;
            fb.p1Offset = 0;
            fb.p2Offset = offset;
            fb.p3Offset = offset * 2;
        } break;

        case Type::BGR888p: {
            fb.width = mat.cols;
            fb.height = mat.rows;
            fb.stride = mat.cols;
            fb.p1Offset = 0;
            fb.p2Offset = size;
            fb.p3Offset = size * 2;
            std::vector<uint8_t> dataVec;
            std::vector<cv::Mat> channels;
            cv::split(mat, channels);
            dataVec.insert(dataVec.end(), channels[0].datastart, channels[0].dataend);
            dataVec.insert(dataVec.end(), channels[1].datastart, channels[1].dataend);
            dataVec.insert(dataVec.end(), channels[2].datastart, channels[2].dataend);
            setData(dataVec);
            size_t offset = channels[0].dataend - channels[0].datastart;
            fb.p1Offset = 0;
            fb.p2Offset = offset;
            fb.p3Offset = offset * 2;
        } break;

        case Type::YUV420p:
            fb.width = mat.cols;
            fb.height = mat.rows;
            fb.stride = mat.cols;
            fb.p1Offset = 0;
            fb.p2Offset = size;
            fb.p3Offset = size / 4;
            cv::cvtColor(mat, output, cv::ColorConversionCodes::COLOR_BGR2YUV_I420);
            setFrame(output);
            break;

        case Type::NV12:
        case Type::NV21: {
            fb.width = mat.cols;
            fb.height = mat.rows;
            fb.stride = mat.cols;
            fb.p1Offset = 0;
            fb.p2Offset = size;
            fb.p3Offset = size;
            auto code = type == Type::NV12 ? cv::COLOR_BGR2YUV_I420 : cv::COLOR_BGR2YUV_YV12;
            cv::cvtColor(mat, output, code);
            assert(output.isContinuous());
            assert(output.total() * output.elemSize() == mat.cols * mat.rows * 3UL / 2UL);
            std::vector<uint8_t> dataVec;
            unsigned int ySize = mat.cols * mat.rows;
            assert(ySize % 4 == 0);
            unsigned int uvSize = ySize / 4;
            assert(ySize + 2 * uvSize == output.total());
            dataVec.insert(dataVec.end(), output.ptr(), output.ptr() + ySize);
            cv::Mat uVals(mat.rows / 2, mat.cols / 2, CV_8UC1, output.ptr() + ySize);
            cv::Mat vVals(mat.rows / 2, mat.cols / 2, CV_8UC1, output.ptr() + ySize + uvSize);
            cv::Mat uvVals;
            cv::merge(std::vector<cv::Mat>{uVals, vVals}, uvVals);
            assert(uvVals.total() * uvVals.elemSize() == uvSize * 2);
            assert(uvVals.isContinuous());
            dataVec.insert(dataVec.end(), uvVals.ptr(), uvVals.ptr() + uvSize * 2);
            setData(dataVec);
            break;
        }
        // case Type::RAW8:
        // case Type::RAW16:
        // case Type::RAW14:
        // case Type::RAW12:
        // case Type::RAW10:
        case Type::GRAY8:
        case Type::GRAYF16:
            fb.width = mat.cols;
            fb.height = mat.rows;
            fb.stride = type == Type::GRAY8 ? mat.cols : mat.cols * 2;
            if(mat.channels() == 3) {
                cv::cvtColor(mat, output, cv::ColorConversionCodes::COLOR_BGR2GRAY);
            } else {
                output = mat;
            }
            if(type == Type::GRAY8) {
                output.convertTo(output, CV_8UC1);
            } else {
                output.convertTo(output, CV_16FC1);
            }
            setFrame(output);
            break;

        default:
            setFrame(mat);
            break;
    }
    return *this;
}

}  // namespace dai
