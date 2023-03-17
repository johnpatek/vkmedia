# VkMedia

Low latency RTSP server for Vulkan graphics. Based on Vulkan and GStreamer.

## Requirements

This code requires a graphics card capable of running Vulkan 1.3 or newer. For now, it will also require Vulkan beta extensions. Currently there is only support for Vulkan video processing on NVIDIA cards, but hopefully AMD and Intel will follow soon.

## Install

For NVIDIA cards, use [NVIDIA-Linux-x86_64-525.47.13](https://developer.nvidia.com/downloads/vulkan-beta-5254713-linux). It is also advisable to use the Vulkan SDK from LunarG.

One the SDK and driver are installed, use `vulkaninfo` to verify the following extensions are present:
```shell
VK_EXT_video_encode_h264                  : extension revision 9
VK_KHR_video_decode_h264                  : extension revision 8
```
