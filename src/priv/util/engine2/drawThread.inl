//
//
// -----------------------------------------------------------------------------
inline void GN::engine2::RenderEngine::DrawThread::present()
{
    submitDrawCommand( DCT_PRESENT, 0 );
    submitDrawBuffer();
}

//
//
// -----------------------------------------------------------------------------
inline GN::engine2::DrawCommandHeader *
GN::engine2::RenderEngine::DrawThread::submitDrawCommand(
    DrawCommandType type, size_t parameterBytes )
{
    GN_ASSERT( 0 <= type && type < NUM_DRAW_COMMAND_TYPES );
    GN_ASSERT( mDrawFunctions[type] );

    // DWORD aligned command size
    size_t commandBytes = align<size_t>( sizeof(DrawCommandHeader) + parameterBytes, 4 );

    if( mDrawBuffers[mWritingIndex].rooms() < commandBytes )
    {
        submitDrawBuffer();
    }

    DrawBuffer & db = mDrawBuffers[mWritingIndex];

    GN_ASSERT( db.rooms() >= commandBytes );

    DrawCommandHeader * header = (DrawCommandHeader * )db.next;

    header->bytes = (UInt32)commandBytes;
    header->fence = mEngine.fenceManager().getAndIncFence();
    header->func  = mDrawFunctions[type];
    header->resourceWaitingCount = 0;

    db.last = (UInt8*)header;
    db.next += commandBytes;
    GN_ASSERT( db.next <= db.end );

    return header;
}

//
//
// -----------------------------------------------------------------------------
template<typename T1>
inline GN::engine2::DrawCommandHeader *
GN::engine2::RenderEngine::DrawThread::submitDrawCommand1(
    DrawCommandType type, const T1 & a1 )
{
    DrawCommandHeader * header = submitDrawCommand( type, sizeof(T1) );

    if( header )
    {
        T1 * p1 = (T1*)header->param();
        *p1 = a1;
    }

    return header;
}

//
//
// -----------------------------------------------------------------------------
template<typename T1, typename T2>
inline GN::engine2::DrawCommandHeader *
GN::engine2::RenderEngine::DrawThread::submitDrawCommand2(
    DrawCommandType type, const T1 & a1, const T2 & a2 )
{
    DrawCommandHeader * header = submitDrawCommand(
        type, sizeof(T1)+sizeof(T2) );

    if( header )
    {
        T1 * p1 = (T1*)header->param();
        *p1 = a1;
        T2 * p2 = (T2*)( p1 + 1 );
        *p2 = a2;
    }

    return header;
}

//
//
// -----------------------------------------------------------------------------
template<typename T1, typename T2, typename T3>
inline GN::engine2::DrawCommandHeader *
GN::engine2::RenderEngine::DrawThread::submitDrawCommand3(
    DrawCommandType type, const T1 & a1, const T2 & a2, const T3 & a3 )
{
    DrawCommandHeader * header = submitDrawCommand(
        type, sizeof(T1)+sizeof(T2)+sizeof(T3) );

    if( header )
    {
        T1 * p1 = (T1*)header->param();
        *p1 = a1;
        T2 * p2 = (T2*)( p1 + 1 );
        *p2 = a2;
        T3 * p3 = (T3*)( p2 + 1 );
        *p3 = a3;
    }

    return header;
}

//
//
// -----------------------------------------------------------------------------
template<typename T1, typename T2, typename T3, typename T4>
inline GN::engine2::DrawCommandHeader *
GN::engine2::RenderEngine::DrawThread::submitDrawCommand4(
    DrawCommandType type, const T1 & a1, const T2 & a2, const T3 & a3, const T4 & a4 )
{
    DrawCommandHeader * header = submitDrawCommand(
        type, sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4) );

    if( header )
    {
        T1 * p1 = (T1*)header->param();
        *p1 = a1;
        T2 * p2 = (T2*)( p1 + 1 );
        *p2 = a2;
        T3 * p3 = (T3*)( p2 + 1 );
        *p3 = a3;
        T4 * p4 = (T4*)( p3 + 1 );
        *p4 = a4;
    }

    return header;
}

//
//
// -----------------------------------------------------------------------------
template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline GN::engine2::DrawCommandHeader *
GN::engine2::RenderEngine::DrawThread::submitDrawCommand6(
    DrawCommandType type, const T1 & a1, const T2 & a2, const T3 & a3, const T4 & a4, const T5 & a5, const T6 & a6 )
{
    DrawCommandHeader * header = submitDrawCommand(
        type, sizeof(T1)+sizeof(T2)+sizeof(T3)+sizeof(T4)+sizeof(T5)+sizeof(T6) );

    if( header )
    {
        T1 * p1 = (T1*)header->param();
        *p1 = a1;
        T2 * p2 = (T2*)( p1 + 1 );
        *p2 = a2;
        T3 * p3 = (T3*)( p2 + 1 );
        *p3 = a3;
        T4 * p4 = (T4*)( p3 + 1 );
        *p4 = a4;
        T5 * p5 = (T5*)( p4 + 1 );
        *p5 = a5;
        T6 * p6 = (T6*)( p5 + 1 );
        *p6 = a6;
    }

    return header;
}


//
//
// -----------------------------------------------------------------------------
inline void GN::engine2::RenderEngine::DrawThread::submitResourceCommand(
    ResourceCommand * item  )
{
    GN_ASSERT( item );
    GN_ASSERT( GROP_DISPOSE == item->op || GROP_COPY == item->op );
    mResourceMutex.lock();
    mResourceCommands.append( item );
    mAction->signal( RESOURCE_ACTION );
    mResourceCommandEmpty = false;
    mResourceMutex.unlock();
}

//
//
// -----------------------------------------------------------------------------
inline void GN::engine2::RenderEngine::DrawThread::submitResourceDisposingCommand(
    GraphicsResourceItem * item )
{
    GN_ASSERT( mEngine.resourceCache().checkResource( item ) );
    GN_ASSERT( GRS_DISPOSED == item->state );

    ResourceCommand * cmd = ResourceCommand::alloc();
    if( 0 == cmd ) return;

    FenceId fence = mEngine.fenceManager().getAndIncFence();

    cmd->noerr                      = true;
    cmd->op                         = GROP_DISPOSE;
    cmd->resource                   = item;
    cmd->mustAfterThisDrawFence     = item->lastReferenceFence;
    cmd->mustAfterThisResourceFence = item->lastSubmissionFence;
    cmd->submittedAtThisFence       = fence;

    item->lastSubmissionFence = fence;

    submitResourceCommand( cmd );
}
