#include "Deleter.h"

namespace gll
{
    void Deleter::addToDelete(Uniform &p_uniform, size_t framebuffers)
    {
        toDelete.push_back({p_uniform, framebuffers});
    }
    void Deleter::addToDelete(VertexBuffer &p_vertex, size_t framebuffers)
    {
        toDelete.push_back({p_vertex, framebuffers});
    }
    void Deleter::addToDelete(Images* image, size_t framebuffers)
    {
        toDelete.push_back({image, framebuffers});
    }
    void Deleter::addToDelete(FrameBuffer* framebuffer, size_t framebuffers)
    {
        toDelete.push_back({framebuffer, framebuffers});
    }
    void Deleter::addToDelete(RenderPass* red, size_t framebuffers)
    {
        toDelete.push_back({red, framebuffers});
    }
    void Deleter::update(Devices &devices, int framebufferUsed)
    {
        for(int i=toDelete.size()-1; i>=0; i--)
        {
            if(findAndDeleteIntFromVector(toDelete[i].framebuffersPassed, framebufferUsed))
            {
                if(toDelete[i].uniform == 1) deleteUniform(i, devices);
                else if(toDelete[i].uniform == 0)deleteVertexBuffer(i, devices);
                else if(toDelete[i].uniform == 2)deleteImage(i, devices);
                else if(toDelete[i].uniform == 3)deleteFrameBuffer(i, devices);
                else deleteRenderPass(i, devices);
                toDelete.erase(toDelete.begin() + i);
            }
        }
    }
    void Deleter::deleteDeleter(Devices &devices)
    {
        for(uint32_t i=0;i<toDelete.size();i++)
        {
            if(toDelete[i].uniform == 1) deleteUniform(i, devices);
            else if(toDelete[i].uniform == 0)deleteVertexBuffer(i, devices);
            else if(toDelete[i].uniform == 2)deleteImage(i, devices);
            else if(toDelete[i].uniform == 3)deleteFrameBuffer(i, devices);
            else deleteRenderPass(i, devices);
        } toDelete.clear();
    }
    bool Deleter::findAndDeleteIntFromVector(std::vector<int> &vc, int in)
    {
        for(uint32_t i=0;i<vc.size();i++)
        {
            if(vc[i] == in)
            {
                vc.erase(vc.begin() + i);
                if(vc.size() == 0) return true;
                return false;
            }
        }
        return false;
    }

    void Deleter::deleteUniform(int i, Devices &devices)
    {
        vkDestroyDescriptorPool(devices.LDev, toDelete[i].descriptorPool[0], nullptr);
        delete [] toDelete[i].descriptorPool;
        for(uint32_t j=0;j<toDelete[i].handel.size();j++) {
            vkDestroyBuffer(devices.LDev, toDelete[i].handel[j][0], nullptr);
            vkFreeMemory(devices.LDev, toDelete[i].memory[j][0], nullptr);
            delete [] toDelete[i].handel[j];
            delete [] toDelete[i].memory[j];
        }
    }
    void Deleter::deleteVertexBuffer(int i, Devices &devices)
    {
        vkDestroyBuffer(devices.LDev, toDelete[i].handel[0][0], nullptr);
        vkFreeMemory(devices.LDev, toDelete[i].memory[0][0], nullptr);
        delete [] toDelete[i].handel[0];
        delete [] toDelete[i].memory[0];
    }
    void Deleter::deleteFrameBuffer(int i, Devices &devices)
    {
        vkDestroyFramebuffer(devices.LDev, toDelete[i].framebuffer[0], nullptr);
    }
    void Deleter::deleteImage(int i, Devices &devices)
    {
        toDelete[i].image->deleteImage(devices);
    }
    void Deleter::deleteRenderPass(int i, Devices &devices)
    {
        toDelete[i].renderpass->deleteRenderPass(devices);
    }
}
