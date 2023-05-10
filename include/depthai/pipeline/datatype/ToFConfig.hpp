#pragma once

#include <unordered_map>
#include <vector>

#include "depthai-shared/datatype/RawToFConfig.hpp"
#include "depthai/pipeline/datatype/Buffer.hpp"

namespace dai {

/**
 * ToFConfig message. Carries config for feature tracking algorithm
 */
class ToFConfig : public Buffer {
    Serialized serialize() const override;
    RawToFConfig& cfg;

   public:
    /**
     * Construct ToFConfig message.
     */
    ToFConfig();
    explicit ToFConfig(std::shared_ptr<RawToFConfig> ptr);
    virtual ~ToFConfig() = default;

    /**
     * Set explicit configuration.
     * @param config Explicit configuration
     */
    ToFConfig& set(dai::RawToFConfig config);

    /**
     * Retrieve configuration data for ToF.
     * @returns config for feature tracking algorithm
     */
    dai::RawToFConfig get() const;
};

}  // namespace dai