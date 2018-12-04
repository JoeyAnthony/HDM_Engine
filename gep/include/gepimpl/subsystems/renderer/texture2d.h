#pragma once

#include "gep/interfaces/resourceManager.h"
#include "gepimpl/subsystems/renderer/imageData2d.h"
#include "gepimpl/subsystems/renderer/renderer.h"

namespace gep
{
    // forward references
    class Texture2D;
    class Renderer;

    /// \brief interface for loading a 2d texture
    class ITexture2DLoader : public IResourceLoader
    {
    public:
		ITexture2DLoader(const char* resourceId);
        virtual Texture2D* loadResource(Texture2D* pInPlace) = 0;
        virtual IResource* loadResource(IResource* pInPlace) override;
        virtual void deleteResource(IResource* pResource) override;
    };

    class DummyTexture2DLoader : public ITexture2DLoader
    {
    public:
		DummyTexture2DLoader();
        virtual Texture2D* loadResource(Texture2D* pInPlace) override;
        virtual void postLoad(ResourcePtr<IResource> pResource) override;

		GEP_RESOURCELOADER_DEFAULT_FUNCTIONS(DummyTexture2DLoader, "Texture2D");
    };

    /// \brief loads a 2d texture from a file
    class Texture2DFileLoader : public ITexture2DLoader
    {
    private:
        Renderer* m_pRenderer;
        bool m_isRegistered;

    public:
        Texture2DFileLoader(const char* filename);
        ~Texture2DFileLoader();
        virtual Texture2D* loadResource(Texture2D* pInPlace) override;
        virtual void postLoad(ResourcePtr<IResource> pResource) override;

		GEP_RESOURCELOADER_DEFAULT_FUNCTIONS(Texture2DFileLoader, "Texture2D");
    };

    class GeneratorTextureLoader : public ITexture2DLoader
    {
    private:
        std::function<void(ArrayPtr<uint8>)> m_generatorFunction;
        Renderer* m_pRenderer;
        uint32 m_width, m_height;

    public:
        GeneratorTextureLoader(uint32 width, uint32 height, std::function<void(ArrayPtr<uint8>)>& generatorFunction, const char* resourceId);
        virtual Texture2D* loadResource(Texture2D* pInPlace) override;
        virtual void postLoad(ResourcePtr<IResource> pResource) override;

		GEP_RESOURCELOADER_DEFAULT_FUNCTIONS(GeneratorTextureLoader, "Texture2D");
    };

    /// \brief a 2d texture
    class Texture2D : public IResource
    {
    private:
        ImageData2D m_data;
        ITexture2DLoader* m_pLoader;
        bool m_hasData;
        std::string m_name;
        ID3D11Device* m_pDevice;
        ID3D11DeviceContext* m_pDeviceContext;
        ID3D11Texture2D* m_pTexture;
        ID3D11ShaderResourceView* m_pResourceView;
        TextureMode m_mode;
        size_t m_gpuWidth, m_gpuHeight;
        ImageFormat m_gpuFormat;

    public:

        Texture2D(const char* name, ITexture2DLoader* pLoader, ID3D11Device* pDevice, ID3D11DeviceContext*, TextureMode mode);
        ~Texture2D();

        void createEmpty(uint32 width, uint32 height, ImageFormat format);

        inline ImageData2D& getImageData() { return m_data; }
        inline void setHasData(bool value) { m_hasData = value; }
        inline ID3D11ShaderResourceView* getResourceView() { return m_pResourceView; }

        //IResource interface
        virtual IResource* getSuperResource() override;
        virtual ITexture2DLoader* getLoader() override;
        virtual void setLoader(IResourceLoader* loader) override;
        virtual bool isLoaded() override;
        virtual void unload() override;
        virtual void finalize() override;
        virtual uint32 getFinalizeOptions() override;
    };
}
