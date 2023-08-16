#pragma once

#include "Conf/Conf.h"
#include "DeviceAPI.h"
#include "Utils/SingletonHolder.hpp"

MAA_TOOLKIT_DEVICE_NS_BEGIN

class DeviceMgrLinux : public SingletonHolder<DeviceMgrLinux>, public MaaToolKitDeviceMgrAPI
{
    friend class SingletonHolder<DeviceMgrLinux>;

public:
    virtual ~DeviceMgrLinux() noexcept override = default;

public: // from MaaToolKitDeviceMgrAPI
    virtual size_t find_device(std::string_view adb_path = std::string_view()) override;

    virtual std::string_view device_name(size_t index) const override;
    virtual std::string_view device_adb_path(size_t index) const override;
    virtual std::string_view device_adb_serial(size_t index) const override;
    virtual MaaAdbControllerType device_adb_controller_type(size_t index) const override;
    virtual std::string_view device_adb_config(size_t index) const override;

private:
    DeviceMgrLinux() = default;
};

MAA_TOOLKIT_DEVICE_NS_END
