// Define call ID for all D3D11 and DXGI methods.
#pragma once

enum D3D11_CALL_ID
{
    // CID for ID3D11Device
    CID_ID3D11Device_BASE,
    CID_ID3D11Device_COUNT = 40,
    CID_ID3D11Device_AddRef = CID_ID3D11Device_BASE,
    CID_ID3D11Device_Release,
    CID_ID3D11Device_QueryInterface,
    CID_ID3D11Device_CreateBuffer,
    CID_ID3D11Device_CreateTexture1D,
    CID_ID3D11Device_CreateTexture2D,
    CID_ID3D11Device_CreateTexture3D,
    CID_ID3D11Device_CreateShaderResourceView,
    CID_ID3D11Device_CreateUnorderedAccessView,
    CID_ID3D11Device_CreateRenderTargetView,
    CID_ID3D11Device_CreateDepthStencilView,
    CID_ID3D11Device_CreateInputLayout,
    CID_ID3D11Device_CreateVertexShader,
    CID_ID3D11Device_CreateGeometryShader,
    CID_ID3D11Device_CreateGeometryShaderWithStreamOutput,
    CID_ID3D11Device_CreatePixelShader,
    CID_ID3D11Device_CreateHullShader,
    CID_ID3D11Device_CreateDomainShader,
    CID_ID3D11Device_CreateComputeShader,
    CID_ID3D11Device_CreateClassLinkage,
    CID_ID3D11Device_CreateBlendState,
    CID_ID3D11Device_CreateDepthStencilState,
    CID_ID3D11Device_CreateRasterizerState,
    CID_ID3D11Device_CreateSamplerState,
    CID_ID3D11Device_CreateQuery,
    CID_ID3D11Device_CreatePredicate,
    CID_ID3D11Device_CreateCounter,
    CID_ID3D11Device_CreateDeferredContext,
    CID_ID3D11Device_OpenSharedResource,
    CID_ID3D11Device_CheckFormatSupport,
    CID_ID3D11Device_CheckMultisampleQualityLevels,
    CID_ID3D11Device_CheckCounterInfo,
    CID_ID3D11Device_CheckCounter,
    CID_ID3D11Device_CheckFeatureSupport,
    CID_ID3D11Device_GetPrivateData,
    CID_ID3D11Device_SetPrivateData,
    CID_ID3D11Device_SetPrivateDataInterface,
    CID_ID3D11Device_GetFeatureLevel,
    CID_ID3D11Device_GetCreationFlags,
    CID_ID3D11Device_GetDeviceRemovedReason,
    CID_ID3D11Device_GetImmediateContext,
    CID_ID3D11Device_SetExceptionMode,
    CID_ID3D11Device_GetExceptionMode,

    // CID for ID3D11DeviceContext
    CID_ID3D11DeviceContext_BASE,
    CID_ID3D11DeviceContext_COUNT = 108,
    CID_ID3D11DeviceContext_AddRef = CID_ID3D11DeviceContext_BASE,
    CID_ID3D11DeviceContext_Release,
    CID_ID3D11DeviceContext_QueryInterface,
    CID_ID3D11DeviceContext_VSSetConstantBuffers,
    CID_ID3D11DeviceContext_PSSetShaderResources,
    CID_ID3D11DeviceContext_PSSetShader,
    CID_ID3D11DeviceContext_PSSetSamplers,
    CID_ID3D11DeviceContext_VSSetShader,
    CID_ID3D11DeviceContext_DrawIndexed,
    CID_ID3D11DeviceContext_Draw,
    CID_ID3D11DeviceContext_Map,
    CID_ID3D11DeviceContext_Unmap,
    CID_ID3D11DeviceContext_PSSetConstantBuffers,
    CID_ID3D11DeviceContext_IASetInputLayout,
    CID_ID3D11DeviceContext_IASetVertexBuffers,
    CID_ID3D11DeviceContext_IASetIndexBuffer,
    CID_ID3D11DeviceContext_DrawIndexedInstanced,
    CID_ID3D11DeviceContext_DrawInstanced,
    CID_ID3D11DeviceContext_GSSetConstantBuffers,
    CID_ID3D11DeviceContext_GSSetShader,
    CID_ID3D11DeviceContext_IASetPrimitiveTopology,
    CID_ID3D11DeviceContext_VSSetShaderResources,
    CID_ID3D11DeviceContext_VSSetSamplers,
    CID_ID3D11DeviceContext_Begin,
    CID_ID3D11DeviceContext_End,
    CID_ID3D11DeviceContext_GetData,
    CID_ID3D11DeviceContext_SetPredication,
    CID_ID3D11DeviceContext_GSSetShaderResources,
    CID_ID3D11DeviceContext_GSSetSamplers,
    CID_ID3D11DeviceContext_OMSetRenderTargets,
    CID_ID3D11DeviceContext_OMSetRenderTargetsAndUnorderedAccessViews,
    CID_ID3D11DeviceContext_OMSetBlendState,
    CID_ID3D11DeviceContext_OMSetDepthStencilState,
    CID_ID3D11DeviceContext_SOSetTargets,
    CID_ID3D11DeviceContext_DrawAuto,
    CID_ID3D11DeviceContext_DrawIndexedInstancedIndirect,
    CID_ID3D11DeviceContext_DrawInstancedIndirect,
    CID_ID3D11DeviceContext_Dispatch,
    CID_ID3D11DeviceContext_DispatchIndirect,
    CID_ID3D11DeviceContext_RSSetState,
    CID_ID3D11DeviceContext_RSSetViewports,
    CID_ID3D11DeviceContext_RSSetScissorRects,
    CID_ID3D11DeviceContext_CopySubresourceRegion,
    CID_ID3D11DeviceContext_CopyResource,
    CID_ID3D11DeviceContext_UpdateSubresource,
    CID_ID3D11DeviceContext_CopyStructureCount,
    CID_ID3D11DeviceContext_ClearRenderTargetView,
    CID_ID3D11DeviceContext_ClearUnorderedAccessViewUint,
    CID_ID3D11DeviceContext_ClearUnorderedAccessViewFloat,
    CID_ID3D11DeviceContext_ClearDepthStencilView,
    CID_ID3D11DeviceContext_GenerateMips,
    CID_ID3D11DeviceContext_SetResourceMinLOD,
    CID_ID3D11DeviceContext_GetResourceMinLOD,
    CID_ID3D11DeviceContext_ResolveSubresource,
    CID_ID3D11DeviceContext_ExecuteCommandList,
    CID_ID3D11DeviceContext_HSSetShaderResources,
    CID_ID3D11DeviceContext_HSSetShader,
    CID_ID3D11DeviceContext_HSSetSamplers,
    CID_ID3D11DeviceContext_HSSetConstantBuffers,
    CID_ID3D11DeviceContext_DSSetShaderResources,
    CID_ID3D11DeviceContext_DSSetShader,
    CID_ID3D11DeviceContext_DSSetSamplers,
    CID_ID3D11DeviceContext_DSSetConstantBuffers,
    CID_ID3D11DeviceContext_CSSetShaderResources,
    CID_ID3D11DeviceContext_CSSetUnorderedAccessViews,
    CID_ID3D11DeviceContext_CSSetShader,
    CID_ID3D11DeviceContext_CSSetSamplers,
    CID_ID3D11DeviceContext_CSSetConstantBuffers,
    CID_ID3D11DeviceContext_VSGetConstantBuffers,
    CID_ID3D11DeviceContext_PSGetShaderResources,
    CID_ID3D11DeviceContext_PSGetShader,
    CID_ID3D11DeviceContext_PSGetSamplers,
    CID_ID3D11DeviceContext_VSGetShader,
    CID_ID3D11DeviceContext_PSGetConstantBuffers,
    CID_ID3D11DeviceContext_IAGetInputLayout,
    CID_ID3D11DeviceContext_IAGetVertexBuffers,
    CID_ID3D11DeviceContext_IAGetIndexBuffer,
    CID_ID3D11DeviceContext_GSGetConstantBuffers,
    CID_ID3D11DeviceContext_GSGetShader,
    CID_ID3D11DeviceContext_IAGetPrimitiveTopology,
    CID_ID3D11DeviceContext_VSGetShaderResources,
    CID_ID3D11DeviceContext_VSGetSamplers,
    CID_ID3D11DeviceContext_GetPredication,
    CID_ID3D11DeviceContext_GSGetShaderResources,
    CID_ID3D11DeviceContext_GSGetSamplers,
    CID_ID3D11DeviceContext_OMGetRenderTargets,
    CID_ID3D11DeviceContext_OMGetRenderTargetsAndUnorderedAccessViews,
    CID_ID3D11DeviceContext_OMGetBlendState,
    CID_ID3D11DeviceContext_OMGetDepthStencilState,
    CID_ID3D11DeviceContext_SOGetTargets,
    CID_ID3D11DeviceContext_RSGetState,
    CID_ID3D11DeviceContext_RSGetViewports,
    CID_ID3D11DeviceContext_RSGetScissorRects,
    CID_ID3D11DeviceContext_HSGetShaderResources,
    CID_ID3D11DeviceContext_HSGetShader,
    CID_ID3D11DeviceContext_HSGetSamplers,
    CID_ID3D11DeviceContext_HSGetConstantBuffers,
    CID_ID3D11DeviceContext_DSGetShaderResources,
    CID_ID3D11DeviceContext_DSGetShader,
    CID_ID3D11DeviceContext_DSGetSamplers,
    CID_ID3D11DeviceContext_DSGetConstantBuffers,
    CID_ID3D11DeviceContext_CSGetShaderResources,
    CID_ID3D11DeviceContext_CSGetUnorderedAccessViews,
    CID_ID3D11DeviceContext_CSGetShader,
    CID_ID3D11DeviceContext_CSGetSamplers,
    CID_ID3D11DeviceContext_CSGetConstantBuffers,
    CID_ID3D11DeviceContext_ClearState,
    CID_ID3D11DeviceContext_Flush,
    CID_ID3D11DeviceContext_GetType,
    CID_ID3D11DeviceContext_GetContextFlags,
    CID_ID3D11DeviceContext_FinishCommandList,

    // CID for ID3D11Debug
    CID_ID3D11Debug_BASE,
    CID_ID3D11Debug_COUNT = 9,
    CID_ID3D11Debug_AddRef = CID_ID3D11Debug_BASE,
    CID_ID3D11Debug_Release,
    CID_ID3D11Debug_QueryInterface,
    CID_ID3D11Debug_SetFeatureMask,
    CID_ID3D11Debug_GetFeatureMask,
    CID_ID3D11Debug_SetPresentPerRenderOpDelay,
    CID_ID3D11Debug_GetPresentPerRenderOpDelay,
    CID_ID3D11Debug_SetSwapChain,
    CID_ID3D11Debug_GetSwapChain,
    CID_ID3D11Debug_ValidateContext,
    CID_ID3D11Debug_ReportLiveDeviceObjects,
    CID_ID3D11Debug_ValidateContextForDispatch,

    // CID for ID3D11InfoQueue
    CID_ID3D11InfoQueue_BASE,
    CID_ID3D11InfoQueue_COUNT = 35,
    CID_ID3D11InfoQueue_AddRef = CID_ID3D11InfoQueue_BASE,
    CID_ID3D11InfoQueue_Release,
    CID_ID3D11InfoQueue_QueryInterface,
    CID_ID3D11InfoQueue_SetMessageCountLimit,
    CID_ID3D11InfoQueue_ClearStoredMessages,
    CID_ID3D11InfoQueue_GetMessage,
    CID_ID3D11InfoQueue_GetNumMessagesAllowedByStorageFilter,
    CID_ID3D11InfoQueue_GetNumMessagesDeniedByStorageFilter,
    CID_ID3D11InfoQueue_GetNumStoredMessages,
    CID_ID3D11InfoQueue_GetNumStoredMessagesAllowedByRetrievalFilter,
    CID_ID3D11InfoQueue_GetNumMessagesDiscardedByMessageCountLimit,
    CID_ID3D11InfoQueue_GetMessageCountLimit,
    CID_ID3D11InfoQueue_AddStorageFilterEntries,
    CID_ID3D11InfoQueue_GetStorageFilter,
    CID_ID3D11InfoQueue_ClearStorageFilter,
    CID_ID3D11InfoQueue_PushEmptyStorageFilter,
    CID_ID3D11InfoQueue_PushCopyOfStorageFilter,
    CID_ID3D11InfoQueue_PushStorageFilter,
    CID_ID3D11InfoQueue_PopStorageFilter,
    CID_ID3D11InfoQueue_GetStorageFilterStackSize,
    CID_ID3D11InfoQueue_AddRetrievalFilterEntries,
    CID_ID3D11InfoQueue_GetRetrievalFilter,
    CID_ID3D11InfoQueue_ClearRetrievalFilter,
    CID_ID3D11InfoQueue_PushEmptyRetrievalFilter,
    CID_ID3D11InfoQueue_PushCopyOfRetrievalFilter,
    CID_ID3D11InfoQueue_PushRetrievalFilter,
    CID_ID3D11InfoQueue_PopRetrievalFilter,
    CID_ID3D11InfoQueue_GetRetrievalFilterStackSize,
    CID_ID3D11InfoQueue_AddMessage,
    CID_ID3D11InfoQueue_AddApplicationMessage,
    CID_ID3D11InfoQueue_SetBreakOnCategory,
    CID_ID3D11InfoQueue_SetBreakOnSeverity,
    CID_ID3D11InfoQueue_SetBreakOnID,
    CID_ID3D11InfoQueue_GetBreakOnCategory,
    CID_ID3D11InfoQueue_GetBreakOnSeverity,
    CID_ID3D11InfoQueue_GetBreakOnID,
    CID_ID3D11InfoQueue_SetMuteDebugOutput,
    CID_ID3D11InfoQueue_GetMuteDebugOutput,

    // CID for IDXGIAdapter
    CID_IDXGIAdapter_BASE,
    CID_IDXGIAdapter_COUNT = 3,
    CID_IDXGIAdapter_AddRef = CID_IDXGIAdapter_BASE,
    CID_IDXGIAdapter_Release,
    CID_IDXGIAdapter_QueryInterface,
    CID_IDXGIAdapter_EnumOutputs,
    CID_IDXGIAdapter_GetDesc,
    CID_IDXGIAdapter_CheckInterfaceSupport,

    // CID for IDXGIFactory
    CID_IDXGIFactory_BASE,
    CID_IDXGIFactory_COUNT = 5,
    CID_IDXGIFactory_AddRef = CID_IDXGIFactory_BASE,
    CID_IDXGIFactory_Release,
    CID_IDXGIFactory_QueryInterface,
    CID_IDXGIFactory_EnumAdapters,
    CID_IDXGIFactory_MakeWindowAssociation,
    CID_IDXGIFactory_GetWindowAssociation,
    CID_IDXGIFactory_CreateSwapChain,
    CID_IDXGIFactory_CreateSoftwareAdapter,

    // CID for IDXGISwapChain
    CID_IDXGISwapChain_BASE,
    CID_IDXGISwapChain_COUNT = 10,
    CID_IDXGISwapChain_AddRef = CID_IDXGISwapChain_BASE,
    CID_IDXGISwapChain_Release,
    CID_IDXGISwapChain_QueryInterface,
    CID_IDXGISwapChain_Present,
    CID_IDXGISwapChain_GetBuffer,
    CID_IDXGISwapChain_SetFullscreenState,
    CID_IDXGISwapChain_GetFullscreenState,
    CID_IDXGISwapChain_GetDesc,
    CID_IDXGISwapChain_ResizeBuffers,
    CID_IDXGISwapChain_ResizeTarget,
    CID_IDXGISwapChain_GetContainingOutput,
    CID_IDXGISwapChain_GetFrameStatistics,
    CID_IDXGISwapChain_GetLastPresentCount,
};
