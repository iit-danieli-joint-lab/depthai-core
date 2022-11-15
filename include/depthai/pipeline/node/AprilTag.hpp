#pragma once

#include <depthai/pipeline/DeviceNode.hpp>

// shared
#include <depthai-shared/properties/AprilTagProperties.hpp>

#include "depthai/pipeline/datatype/AprilTagConfig.hpp"

namespace dai {
namespace node {

/**
 * @brief AprilTag node.
 */
class AprilTag : public NodeCRTP<DeviceNode, AprilTag, AprilTagProperties> {
   public:
    constexpr static const char* NAME = "AprilTag";

   protected:
    Properties& getProperties();

   private:
    std::shared_ptr<RawAprilTagConfig> rawConfig;

   public:
    AprilTag();
    AprilTag(std::unique_ptr<Properties> props);

    /**
     * Initial config to use when calculating spatial location data.
     */
    AprilTagConfig initialConfig;

    /**
     * Input AprilTagConfig message with ability to modify parameters in runtime.
     * Default queue is non-blocking with size 4.
     */
    Input inputConfig{true, *this, "inputConfig", Input::Type::SReceiver, false, 4, {{DatatypeEnum::AprilTagConfig, false}}};
    /**
     * Input message with depth data used to retrieve spatial information about detected object.
     * Default queue is non-blocking with size 4.
     */
    Input inputImage{true, *this, "inputImage", Input::Type::SReceiver, false, 4, {{DatatypeEnum::ImgFrame, false}}};

    /**
     * Outputs AprilTags message that carries spatial location results.
     */
    Output out{true, *this, "out", Output::Type::MSender, {{DatatypeEnum::AprilTags, false}}};

    /**
     * Outputs AprilTagConfig message that contains current configuration.
     */
    Output outConfig{true, *this, "outConfig", Output::Type::MSender, {{DatatypeEnum::AprilTagConfig, false}}};

    /**
     * Passthrough message on which the calculation was performed.
     * Suitable for when input queue is set to non-blocking behavior.
     */
    Output passthroughInputImage{true, *this, "passthroughInputImage", Output::Type::MSender, {{DatatypeEnum::ImgFrame, false}}};

    // Functions to set properties
    /**
     * Specify whether or not wait until configuration message arrives to inputConfig Input.
     * @param wait True to wait for configuration message, false otherwise.
     */
    void setWaitForConfigInput(bool wait);
};

}  // namespace node
}  // namespace dai
