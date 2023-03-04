/*
** Copyright 2023 John R. Patek Sr.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#ifndef VKMEDIA_HPP
#define VKMEDIA_HPP

#include <vkmedia.h>
#include <vulkan/vulkan.hpp>

#include <memory>
#include <optional>

namespace vkm
{
    template <class Type, class Deleter>
    class handle
    {
    public:
        handle()
        {
            set(nullptr, true);
        }

        handle(Type data, bool owner)
        {
            set(data, owner);
        }

        ~handle()
        {
            if (_owner)
            {
                _data.reset();
            }
            else
            {
                _data.release();
            }
        }

        Type get() const
        {
            return _data.get();
        }

    protected:
        void set(Type data, bool owner = true)
        {
            _data = std::unique_ptr<void, Deleter>(data);
        }

    private:
        bool _owner;
        std::unique_ptr<void, Deleter> _data;
    };

    struct encoder_parameters
    {
        std::optional<int> width;
        std::optional<int> height;
    };

    class encoder : public handle<vkm_encoder, decltype(&vkm_encoder_destroy)>
    {
    public:
        encoder(const encoder_parameters &parameters);
    };

    struct swapchain_parameters
    {
        VkPhysicalDevice vulkan_physical_device;
        VkDevice vulkan_logical_device;
        CUcontext cuda_context;
        VkFormat format;
        VkExtent2D size;
        uint32_t count;
    };

    class swapchain : public handle<vkm_swapchain, decltype(&vkm_swapchain_destroy)>
    {
    public:
        swapchain(const swapchain_parameters &parameters);
        
        
    };

}

#endif