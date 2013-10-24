// script generated file. DO NOT edit.

#include "pch.h"
#include "hooks.h"

void HookedClassFactory::registerAll()
{
    registerFactory<ID3D11Buffer>(D3D11BufferHook::sNewInstance, D3D11BufferHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIOutput1>(DXGIOutput1Hook::sNewInstance, DXGIOutput1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11DepthStencilView>(D3D11DepthStencilViewHook::sNewInstance, D3D11DepthStencilViewHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIAdapter2>(DXGIAdapter2Hook::sNewInstance, DXGIAdapter2Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11RefTrackingOptions>(D3D11RefTrackingOptionsHook::sNewInstance, D3D11RefTrackingOptionsHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoProcessorOutputView>(D3D11VideoProcessorOutputViewHook::sNewInstance, D3D11VideoProcessorOutputViewHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11RenderTargetView>(D3D11RenderTargetViewHook::sNewInstance, D3D11RenderTargetViewHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoProcessorInputView>(D3D11VideoProcessorInputViewHook::sNewInstance, D3D11VideoProcessorInputViewHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11AuthenticatedChannel>(D3D11AuthenticatedChannelHook::sNewInstance, D3D11AuthenticatedChannelHook::sDeleteInstance, nullptr);
    registerFactory<IDXGISurface1>(DXGISurface1Hook::sNewInstance, DXGISurface1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VertexShader>(D3D11VertexShaderHook::sNewInstance, D3D11VertexShaderHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11ClassInstance>(D3D11ClassInstanceHook::sNewInstance, D3D11ClassInstanceHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Texture3D>(D3D11Texture3DHook::sNewInstance, D3D11Texture3DHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIDevice1>(DXGIDevice1Hook::sNewInstance, DXGIDevice1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Texture1D>(D3D11Texture1DHook::sNewInstance, D3D11Texture1DHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Predicate>(D3D11PredicateHook::sNewInstance, D3D11PredicateHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11ComputeShader>(D3D11ComputeShaderHook::sNewInstance, D3D11ComputeShaderHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11UnorderedAccessView>(D3D11UnorderedAccessViewHook::sNewInstance, D3D11UnorderedAccessViewHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11RefDefaultTrackingOptions>(D3D11RefDefaultTrackingOptionsHook::sNewInstance, D3D11RefDefaultTrackingOptionsHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoContext>(D3D11VideoContextHook::sNewInstance, D3D11VideoContextHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11DomainShader>(D3D11DomainShaderHook::sNewInstance, D3D11DomainShaderHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11RasterizerState>(D3D11RasterizerStateHook::sNewInstance, D3D11RasterizerStateHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11DeviceContext1>(D3D11DeviceContext1Hook::sNewInstance, D3D11DeviceContext1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Counter>(D3D11CounterHook::sNewInstance, D3D11CounterHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Resource>(D3D11ResourceHook::sNewInstance, D3D11ResourceHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoDecoderOutputView>(D3D11VideoDecoderOutputViewHook::sNewInstance, D3D11VideoDecoderOutputViewHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11TracingDevice>(D3D11TracingDeviceHook::sNewInstance, D3D11TracingDeviceHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIAdapter1>(DXGIAdapter1Hook::sNewInstance, DXGIAdapter1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Device>(D3D11DeviceHook::sNewInstance, D3D11DeviceHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11DeviceChild>(D3D11DeviceChildHook::sNewInstance, D3D11DeviceChildHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIDevice>(DXGIDeviceHook::sNewInstance, DXGIDeviceHook::sDeleteInstance, nullptr);
    registerFactory<IDXGISurface2>(DXGISurface2Hook::sNewInstance, DXGISurface2Hook::sDeleteInstance, nullptr);
    registerFactory<IDXGIAdapter>(DXGIAdapterHook::sNewInstance, DXGIAdapterHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11InputLayout>(D3D11InputLayoutHook::sNewInstance, D3D11InputLayoutHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIResource1>(DXGIResource1Hook::sNewInstance, DXGIResource1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11View>(D3D11ViewHook::sNewInstance, D3D11ViewHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoDevice>(D3D11VideoDeviceHook::sNewInstance, D3D11VideoDeviceHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11ClassLinkage>(D3D11ClassLinkageHook::sNewInstance, D3D11ClassLinkageHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIOutputDuplication>(DXGIOutputDuplicationHook::sNewInstance, DXGIOutputDuplicationHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11HullShader>(D3D11HullShaderHook::sNewInstance, D3D11HullShaderHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11RasterizerState1>(D3D11RasterizerState1Hook::sNewInstance, D3D11RasterizerState1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11DeviceContext>(D3D11DeviceContextHook::sNewInstance, D3D11DeviceContextHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11InfoQueue>(D3D11InfoQueueHook::sNewInstance, D3D11InfoQueueHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11GeometryShader>(D3D11GeometryShaderHook::sNewInstance, D3D11GeometryShaderHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIDevice2>(DXGIDevice2Hook::sNewInstance, DXGIDevice2Hook::sDeleteInstance, nullptr);
    registerFactory<IDXGIObject>(DXGIObjectHook::sNewInstance, DXGIObjectHook::sDeleteInstance, nullptr);
    registerFactory<IDXGISwapChain1>(DXGISwapChain1Hook::sNewInstance, DXGISwapChain1Hook::sDeleteInstance, nullptr);
    registerFactory<IDXGIResource>(DXGIResourceHook::sNewInstance, DXGIResourceHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11CommandList>(D3D11CommandListHook::sNewInstance, D3D11CommandListHook::sDeleteInstance, nullptr);
    registerFactory<ID3DUserDefinedAnnotation>(D3DUserDefinedAnnotationHook::sNewInstance, D3DUserDefinedAnnotationHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11BlendState1>(D3D11BlendState1Hook::sNewInstance, D3D11BlendState1Hook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Asynchronous>(D3D11AsynchronousHook::sNewInstance, D3D11AsynchronousHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoProcessorEnumerator>(D3D11VideoProcessorEnumeratorHook::sNewInstance, D3D11VideoProcessorEnumeratorHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Texture2D>(D3D11Texture2DHook::sNewInstance, D3D11Texture2DHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoProcessor>(D3D11VideoProcessorHook::sNewInstance, D3D11VideoProcessorHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11SamplerState>(D3D11SamplerStateHook::sNewInstance, D3D11SamplerStateHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIDeviceSubObject>(DXGIDeviceSubObjectHook::sNewInstance, DXGIDeviceSubObjectHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Query>(D3D11QueryHook::sNewInstance, D3D11QueryHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11DepthStencilState>(D3D11DepthStencilStateHook::sNewInstance, D3D11DepthStencilStateHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11ShaderResourceView>(D3D11ShaderResourceViewHook::sNewInstance, D3D11ShaderResourceViewHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIFactory1>(DXGIFactory1Hook::sNewInstance, DXGIFactory1Hook::sDeleteInstance, nullptr);
    registerFactory<IDXGIFactory2>(DXGIFactory2Hook::sNewInstance, DXGIFactory2Hook::sDeleteInstance, nullptr);
    registerFactory<IDXGIOutput>(DXGIOutputHook::sNewInstance, DXGIOutputHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11PixelShader>(D3D11PixelShaderHook::sNewInstance, D3D11PixelShaderHook::sDeleteInstance, nullptr);
    registerFactory<IDXGISurface>(DXGISurfaceHook::sNewInstance, DXGISurfaceHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Device1>(D3D11Device1Hook::sNewInstance, D3D11Device1Hook::sDeleteInstance, nullptr);
    registerFactory<IDXGIDisplayControl>(DXGIDisplayControlHook::sNewInstance, DXGIDisplayControlHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11CryptoSession>(D3D11CryptoSessionHook::sNewInstance, D3D11CryptoSessionHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11Debug>(D3D11DebugHook::sNewInstance, D3D11DebugHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11BlendState>(D3D11BlendStateHook::sNewInstance, D3D11BlendStateHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11VideoDecoder>(D3D11VideoDecoderHook::sNewInstance, D3D11VideoDecoderHook::sDeleteInstance, nullptr);
    registerFactory<ID3DDeviceContextState>(D3DDeviceContextStateHook::sNewInstance, D3DDeviceContextStateHook::sDeleteInstance, nullptr);
    registerFactory<ID3D11SwitchToRef>(D3D11SwitchToRefHook::sNewInstance, D3D11SwitchToRefHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIFactory>(DXGIFactoryHook::sNewInstance, DXGIFactoryHook::sDeleteInstance, nullptr);
    registerFactory<IDXGISwapChain>(DXGISwapChainHook::sNewInstance, DXGISwapChainHook::sDeleteInstance, nullptr);
    registerFactory<IDXGIKeyedMutex>(DXGIKeyedMutexHook::sNewInstance, DXGIKeyedMutexHook::sDeleteInstance, nullptr);
}
