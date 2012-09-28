// script generated file. DO NOT edit.
#include "pch.h"
#include "d3d11hook.h"
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::SetMessageCountLimit(
    UINT64 MessageCountLimit)
{
    if (_SetMessageCountLimit_pre_ptr._value) { (this->*_SetMessageCountLimit_pre_ptr._value)(MessageCountLimit); }
    HRESULT ret = GetRealObj()->SetMessageCountLimit(MessageCountLimit);
    if (_SetMessageCountLimit_post_ptr._value) { (this->*_SetMessageCountLimit_post_ptr._value)(ret, MessageCountLimit); }
    return ret;
}

// -----------------------------------------------------------------------------
void STDMETHODCALLTYPE D3D11InfoQueueHook::ClearStoredMessages()
{
    if (_ClearStoredMessages_pre_ptr._value) { (this->*_ClearStoredMessages_pre_ptr._value)(); }
    GetRealObj()->ClearStoredMessages();
    if (_ClearStoredMessages_post_ptr._value) { (this->*_ClearStoredMessages_post_ptr._value)(); }
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::GetMessage(
    UINT64 MessageIndex,
    D3D11_MESSAGE * pMessage,
    SIZE_T * pMessageByteLength)
{
    if (_GetMessage_pre_ptr._value) { (this->*_GetMessage_pre_ptr._value)(MessageIndex, pMessage, pMessageByteLength); }
    HRESULT ret = GetRealObj()->GetMessage(MessageIndex, pMessage, pMessageByteLength);
    if (_GetMessage_post_ptr._value) { (this->*_GetMessage_post_ptr._value)(ret, MessageIndex, pMessage, pMessageByteLength); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT64 STDMETHODCALLTYPE D3D11InfoQueueHook::GetNumMessagesAllowedByStorageFilter()
{
    if (_GetNumMessagesAllowedByStorageFilter_pre_ptr._value) { (this->*_GetNumMessagesAllowedByStorageFilter_pre_ptr._value)(); }
    UINT64 ret = GetRealObj()->GetNumMessagesAllowedByStorageFilter();
    if (_GetNumMessagesAllowedByStorageFilter_post_ptr._value) { (this->*_GetNumMessagesAllowedByStorageFilter_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT64 STDMETHODCALLTYPE D3D11InfoQueueHook::GetNumMessagesDeniedByStorageFilter()
{
    if (_GetNumMessagesDeniedByStorageFilter_pre_ptr._value) { (this->*_GetNumMessagesDeniedByStorageFilter_pre_ptr._value)(); }
    UINT64 ret = GetRealObj()->GetNumMessagesDeniedByStorageFilter();
    if (_GetNumMessagesDeniedByStorageFilter_post_ptr._value) { (this->*_GetNumMessagesDeniedByStorageFilter_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT64 STDMETHODCALLTYPE D3D11InfoQueueHook::GetNumStoredMessages()
{
    if (_GetNumStoredMessages_pre_ptr._value) { (this->*_GetNumStoredMessages_pre_ptr._value)(); }
    UINT64 ret = GetRealObj()->GetNumStoredMessages();
    if (_GetNumStoredMessages_post_ptr._value) { (this->*_GetNumStoredMessages_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT64 STDMETHODCALLTYPE D3D11InfoQueueHook::GetNumStoredMessagesAllowedByRetrievalFilter()
{
    if (_GetNumStoredMessagesAllowedByRetrievalFilter_pre_ptr._value) { (this->*_GetNumStoredMessagesAllowedByRetrievalFilter_pre_ptr._value)(); }
    UINT64 ret = GetRealObj()->GetNumStoredMessagesAllowedByRetrievalFilter();
    if (_GetNumStoredMessagesAllowedByRetrievalFilter_post_ptr._value) { (this->*_GetNumStoredMessagesAllowedByRetrievalFilter_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT64 STDMETHODCALLTYPE D3D11InfoQueueHook::GetNumMessagesDiscardedByMessageCountLimit()
{
    if (_GetNumMessagesDiscardedByMessageCountLimit_pre_ptr._value) { (this->*_GetNumMessagesDiscardedByMessageCountLimit_pre_ptr._value)(); }
    UINT64 ret = GetRealObj()->GetNumMessagesDiscardedByMessageCountLimit();
    if (_GetNumMessagesDiscardedByMessageCountLimit_post_ptr._value) { (this->*_GetNumMessagesDiscardedByMessageCountLimit_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT64 STDMETHODCALLTYPE D3D11InfoQueueHook::GetMessageCountLimit()
{
    if (_GetMessageCountLimit_pre_ptr._value) { (this->*_GetMessageCountLimit_pre_ptr._value)(); }
    UINT64 ret = GetRealObj()->GetMessageCountLimit();
    if (_GetMessageCountLimit_post_ptr._value) { (this->*_GetMessageCountLimit_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::AddStorageFilterEntries(
    D3D11_INFO_QUEUE_FILTER * pFilter)
{
    if (_AddStorageFilterEntries_pre_ptr._value) { (this->*_AddStorageFilterEntries_pre_ptr._value)(pFilter); }
    HRESULT ret = GetRealObj()->AddStorageFilterEntries(pFilter);
    if (_AddStorageFilterEntries_post_ptr._value) { (this->*_AddStorageFilterEntries_post_ptr._value)(ret, pFilter); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::GetStorageFilter(
    D3D11_INFO_QUEUE_FILTER * pFilter,
    SIZE_T * pFilterByteLength)
{
    if (_GetStorageFilter_pre_ptr._value) { (this->*_GetStorageFilter_pre_ptr._value)(pFilter, pFilterByteLength); }
    HRESULT ret = GetRealObj()->GetStorageFilter(pFilter, pFilterByteLength);
    if (_GetStorageFilter_post_ptr._value) { (this->*_GetStorageFilter_post_ptr._value)(ret, pFilter, pFilterByteLength); }
    return ret;
}

// -----------------------------------------------------------------------------
void STDMETHODCALLTYPE D3D11InfoQueueHook::ClearStorageFilter()
{
    if (_ClearStorageFilter_pre_ptr._value) { (this->*_ClearStorageFilter_pre_ptr._value)(); }
    GetRealObj()->ClearStorageFilter();
    if (_ClearStorageFilter_post_ptr._value) { (this->*_ClearStorageFilter_post_ptr._value)(); }
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::PushEmptyStorageFilter()
{
    if (_PushEmptyStorageFilter_pre_ptr._value) { (this->*_PushEmptyStorageFilter_pre_ptr._value)(); }
    HRESULT ret = GetRealObj()->PushEmptyStorageFilter();
    if (_PushEmptyStorageFilter_post_ptr._value) { (this->*_PushEmptyStorageFilter_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::PushCopyOfStorageFilter()
{
    if (_PushCopyOfStorageFilter_pre_ptr._value) { (this->*_PushCopyOfStorageFilter_pre_ptr._value)(); }
    HRESULT ret = GetRealObj()->PushCopyOfStorageFilter();
    if (_PushCopyOfStorageFilter_post_ptr._value) { (this->*_PushCopyOfStorageFilter_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::PushStorageFilter(
    D3D11_INFO_QUEUE_FILTER * pFilter)
{
    if (_PushStorageFilter_pre_ptr._value) { (this->*_PushStorageFilter_pre_ptr._value)(pFilter); }
    HRESULT ret = GetRealObj()->PushStorageFilter(pFilter);
    if (_PushStorageFilter_post_ptr._value) { (this->*_PushStorageFilter_post_ptr._value)(ret, pFilter); }
    return ret;
}

// -----------------------------------------------------------------------------
void STDMETHODCALLTYPE D3D11InfoQueueHook::PopStorageFilter()
{
    if (_PopStorageFilter_pre_ptr._value) { (this->*_PopStorageFilter_pre_ptr._value)(); }
    GetRealObj()->PopStorageFilter();
    if (_PopStorageFilter_post_ptr._value) { (this->*_PopStorageFilter_post_ptr._value)(); }
}

// -----------------------------------------------------------------------------
UINT STDMETHODCALLTYPE D3D11InfoQueueHook::GetStorageFilterStackSize()
{
    if (_GetStorageFilterStackSize_pre_ptr._value) { (this->*_GetStorageFilterStackSize_pre_ptr._value)(); }
    UINT ret = GetRealObj()->GetStorageFilterStackSize();
    if (_GetStorageFilterStackSize_post_ptr._value) { (this->*_GetStorageFilterStackSize_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::AddRetrievalFilterEntries(
    D3D11_INFO_QUEUE_FILTER * pFilter)
{
    if (_AddRetrievalFilterEntries_pre_ptr._value) { (this->*_AddRetrievalFilterEntries_pre_ptr._value)(pFilter); }
    HRESULT ret = GetRealObj()->AddRetrievalFilterEntries(pFilter);
    if (_AddRetrievalFilterEntries_post_ptr._value) { (this->*_AddRetrievalFilterEntries_post_ptr._value)(ret, pFilter); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::GetRetrievalFilter(
    D3D11_INFO_QUEUE_FILTER * pFilter,
    SIZE_T * pFilterByteLength)
{
    if (_GetRetrievalFilter_pre_ptr._value) { (this->*_GetRetrievalFilter_pre_ptr._value)(pFilter, pFilterByteLength); }
    HRESULT ret = GetRealObj()->GetRetrievalFilter(pFilter, pFilterByteLength);
    if (_GetRetrievalFilter_post_ptr._value) { (this->*_GetRetrievalFilter_post_ptr._value)(ret, pFilter, pFilterByteLength); }
    return ret;
}

// -----------------------------------------------------------------------------
void STDMETHODCALLTYPE D3D11InfoQueueHook::ClearRetrievalFilter()
{
    if (_ClearRetrievalFilter_pre_ptr._value) { (this->*_ClearRetrievalFilter_pre_ptr._value)(); }
    GetRealObj()->ClearRetrievalFilter();
    if (_ClearRetrievalFilter_post_ptr._value) { (this->*_ClearRetrievalFilter_post_ptr._value)(); }
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::PushEmptyRetrievalFilter()
{
    if (_PushEmptyRetrievalFilter_pre_ptr._value) { (this->*_PushEmptyRetrievalFilter_pre_ptr._value)(); }
    HRESULT ret = GetRealObj()->PushEmptyRetrievalFilter();
    if (_PushEmptyRetrievalFilter_post_ptr._value) { (this->*_PushEmptyRetrievalFilter_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::PushCopyOfRetrievalFilter()
{
    if (_PushCopyOfRetrievalFilter_pre_ptr._value) { (this->*_PushCopyOfRetrievalFilter_pre_ptr._value)(); }
    HRESULT ret = GetRealObj()->PushCopyOfRetrievalFilter();
    if (_PushCopyOfRetrievalFilter_post_ptr._value) { (this->*_PushCopyOfRetrievalFilter_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::PushRetrievalFilter(
    D3D11_INFO_QUEUE_FILTER * pFilter)
{
    if (_PushRetrievalFilter_pre_ptr._value) { (this->*_PushRetrievalFilter_pre_ptr._value)(pFilter); }
    HRESULT ret = GetRealObj()->PushRetrievalFilter(pFilter);
    if (_PushRetrievalFilter_post_ptr._value) { (this->*_PushRetrievalFilter_post_ptr._value)(ret, pFilter); }
    return ret;
}

// -----------------------------------------------------------------------------
void STDMETHODCALLTYPE D3D11InfoQueueHook::PopRetrievalFilter()
{
    if (_PopRetrievalFilter_pre_ptr._value) { (this->*_PopRetrievalFilter_pre_ptr._value)(); }
    GetRealObj()->PopRetrievalFilter();
    if (_PopRetrievalFilter_post_ptr._value) { (this->*_PopRetrievalFilter_post_ptr._value)(); }
}

// -----------------------------------------------------------------------------
UINT STDMETHODCALLTYPE D3D11InfoQueueHook::GetRetrievalFilterStackSize()
{
    if (_GetRetrievalFilterStackSize_pre_ptr._value) { (this->*_GetRetrievalFilterStackSize_pre_ptr._value)(); }
    UINT ret = GetRealObj()->GetRetrievalFilterStackSize();
    if (_GetRetrievalFilterStackSize_post_ptr._value) { (this->*_GetRetrievalFilterStackSize_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::AddMessage(
    D3D11_MESSAGE_CATEGORY Category,
    D3D11_MESSAGE_SEVERITY Severity,
    D3D11_MESSAGE_ID ID,
    LPCSTR pDescription)
{
    if (_AddMessage_pre_ptr._value) { (this->*_AddMessage_pre_ptr._value)(Category, Severity, ID, pDescription); }
    HRESULT ret = GetRealObj()->AddMessage(Category, Severity, ID, pDescription);
    if (_AddMessage_post_ptr._value) { (this->*_AddMessage_post_ptr._value)(ret, Category, Severity, ID, pDescription); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::AddApplicationMessage(
    D3D11_MESSAGE_SEVERITY Severity,
    LPCSTR pDescription)
{
    if (_AddApplicationMessage_pre_ptr._value) { (this->*_AddApplicationMessage_pre_ptr._value)(Severity, pDescription); }
    HRESULT ret = GetRealObj()->AddApplicationMessage(Severity, pDescription);
    if (_AddApplicationMessage_post_ptr._value) { (this->*_AddApplicationMessage_post_ptr._value)(ret, Severity, pDescription); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::SetBreakOnCategory(
    D3D11_MESSAGE_CATEGORY Category,
    BOOL bEnable)
{
    if (_SetBreakOnCategory_pre_ptr._value) { (this->*_SetBreakOnCategory_pre_ptr._value)(Category, bEnable); }
    HRESULT ret = GetRealObj()->SetBreakOnCategory(Category, bEnable);
    if (_SetBreakOnCategory_post_ptr._value) { (this->*_SetBreakOnCategory_post_ptr._value)(ret, Category, bEnable); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::SetBreakOnSeverity(
    D3D11_MESSAGE_SEVERITY Severity,
    BOOL bEnable)
{
    if (_SetBreakOnSeverity_pre_ptr._value) { (this->*_SetBreakOnSeverity_pre_ptr._value)(Severity, bEnable); }
    HRESULT ret = GetRealObj()->SetBreakOnSeverity(Severity, bEnable);
    if (_SetBreakOnSeverity_post_ptr._value) { (this->*_SetBreakOnSeverity_post_ptr._value)(ret, Severity, bEnable); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11InfoQueueHook::SetBreakOnID(
    D3D11_MESSAGE_ID ID,
    BOOL bEnable)
{
    if (_SetBreakOnID_pre_ptr._value) { (this->*_SetBreakOnID_pre_ptr._value)(ID, bEnable); }
    HRESULT ret = GetRealObj()->SetBreakOnID(ID, bEnable);
    if (_SetBreakOnID_post_ptr._value) { (this->*_SetBreakOnID_post_ptr._value)(ret, ID, bEnable); }
    return ret;
}

// -----------------------------------------------------------------------------
BOOL STDMETHODCALLTYPE D3D11InfoQueueHook::GetBreakOnCategory(
    D3D11_MESSAGE_CATEGORY Category)
{
    if (_GetBreakOnCategory_pre_ptr._value) { (this->*_GetBreakOnCategory_pre_ptr._value)(Category); }
    BOOL ret = GetRealObj()->GetBreakOnCategory(Category);
    if (_GetBreakOnCategory_post_ptr._value) { (this->*_GetBreakOnCategory_post_ptr._value)(ret, Category); }
    return ret;
}

// -----------------------------------------------------------------------------
BOOL STDMETHODCALLTYPE D3D11InfoQueueHook::GetBreakOnSeverity(
    D3D11_MESSAGE_SEVERITY Severity)
{
    if (_GetBreakOnSeverity_pre_ptr._value) { (this->*_GetBreakOnSeverity_pre_ptr._value)(Severity); }
    BOOL ret = GetRealObj()->GetBreakOnSeverity(Severity);
    if (_GetBreakOnSeverity_post_ptr._value) { (this->*_GetBreakOnSeverity_post_ptr._value)(ret, Severity); }
    return ret;
}

// -----------------------------------------------------------------------------
BOOL STDMETHODCALLTYPE D3D11InfoQueueHook::GetBreakOnID(
    D3D11_MESSAGE_ID ID)
{
    if (_GetBreakOnID_pre_ptr._value) { (this->*_GetBreakOnID_pre_ptr._value)(ID); }
    BOOL ret = GetRealObj()->GetBreakOnID(ID);
    if (_GetBreakOnID_post_ptr._value) { (this->*_GetBreakOnID_post_ptr._value)(ret, ID); }
    return ret;
}

// -----------------------------------------------------------------------------
void STDMETHODCALLTYPE D3D11InfoQueueHook::SetMuteDebugOutput(
    BOOL bMute)
{
    if (_SetMuteDebugOutput_pre_ptr._value) { (this->*_SetMuteDebugOutput_pre_ptr._value)(bMute); }
    GetRealObj()->SetMuteDebugOutput(bMute);
    if (_SetMuteDebugOutput_post_ptr._value) { (this->*_SetMuteDebugOutput_post_ptr._value)(bMute); }
}

// -----------------------------------------------------------------------------
BOOL STDMETHODCALLTYPE D3D11InfoQueueHook::GetMuteDebugOutput()
{
    if (_GetMuteDebugOutput_pre_ptr._value) { (this->*_GetMuteDebugOutput_pre_ptr._value)(); }
    BOOL ret = GetRealObj()->GetMuteDebugOutput();
    if (_GetMuteDebugOutput_post_ptr._value) { (this->*_GetMuteDebugOutput_post_ptr._value)(ret); }
    return ret;
}

