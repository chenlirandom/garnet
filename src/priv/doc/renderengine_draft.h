#ifndef __GN_ENGINE_RENDERENGINE_H__
#define __GN_ENGINE_RENDERENGINE_H__
// *****************************************************************************
//! \file    doc/renderEngine_draft.h
//! \brief   interface of render engine
//! \author  chenli@@FAREAST (2007.4.13)
// *****************************************************************************

namespace GN { namespace engine
{
    struct ShaderResourceDesc
    {
        gfx::ShaderType      type;
        gfx::ShadingLanguage lang;
        const StrA &         code;
        const StrA &         hints;
    };

    struct TextureResourceDesc
    {
        gfx::TextureDesc desc;
        gfx::ImageDesc * initialData;
    };

    struct VtxBufResourceDesc
    {
        gfx::VtxBufDesc desc;
        const void *    initialData;
    };

    struct IdxBufResourceDesc
    {
        gfx::IdxBufDesc desc;
        const void *    initialData;
    };

    struct GraphicsResourceDesc
    {
        int                 type;
        ShaderResourceDesc  sd;
        TextureResourceDesc td;
        VtxBufResourceDesc  vd;
        IdxBufResourceDesc  id;
    };

    enum GraphicsResourceState
    {
        GRS_READY,
        GRS_DISPOSED,
    };

    struct GraphicsResource
    {
        int draw_fence; ///< last used by this draw
        int state;      ///< One of GraphicsResourceState, used by game thread only
        int type;
        int lod;        ///< current LOD. Definition of LOD is resource/application dependent.
        union
        {
            gfx::Shader  * shader;
            gfx::Texture * texture;
            gfx::VtxBuf  * vtxBuf;
            gfx::IdxBuf  * idxBuf;
        };
    };

    typedef int GraphicsResourceId;

    ///
    /// Graphics Resource cache (owned by 
    ///
    class GraphicsResourceCache
    {
    public:

        ///
        /// ctor
        ///
        GraphicsResourceCache( UInt32 maxTexBytes, UInt32 maxMeshBytes );

        ///
        /// you can create as many as graphics resources as you want. But only limited
        /// number of resources can be realized at any given time. The resource is
        /// useable only between realizeResource() and disposeResource().
        ///
        /// note that only realizeResource() and disposeResource() touch rendering device.
        ///
        //@{
        GraphicsResourceId createResource( const GraphicsResoureDesc & );
        void               freeResource( GraphicsResourceId );
        GraphicsResource * getResourceById( GraphicsResourceId );
        bool               realizeResource( GraphicsResourceId, bool * needreloadData );
        void               disposeResource( GraphicsResourceId );
        //@}
    };

    extern GraphicsResourceCache gGraphicsResourceCache;

    ///
    /// Graphics resource loader
    ///
    struct GraphicsResourceLoader : public NoCopy
    {
        bool load( void * & data, size_t & bytes, int lod ) = 0;  ///< load from external/slow storage (disk, cdrom, network)

        bool decompress( void * & outbuf, size_t & outbytes, const void * inbuf, size_t inbytes, int lod ) = 0; ///< decompress or other process to prepare to copy to graphics resource.

        bool copy( GraphicsResource & gfxres, const void * data, size_t bytes, int lod ) = 0;
    };

    ///
    /// type of operations
    ///
    enum GraphicsResourceOperation
    {
        OP_LOAD,       ///< load from disk. Handled by IO tread.

        OP_COPY,       ///< copy data to graphics resource. Handled by IO thread.

        OP_DECOMPRESS, ///< do decompress or other process to prepare to copy to graphics resource.
                       ///< Handled by processing thread.

        /// these operations are handled by rendering thread
        //@{
        OP_LOCK,       ///< lock the graphics resource, realize those disposed as well, need touch rendering device.
        OP_UNLOCK,     ///< unlock the graphics resource, need touch rendering device.
        OP_DISPOSE,    ///< dispose the resource.
        //@}
    };

    ///
    /// ...
    ///
    struct GraphicsResourceCommand
    {
        //@{
        int                       wait_for_draw_fence; ///< the request must be happend after this draw fence
        volatile int            * pending_resources;   ///< when this request is done. It'll decrease value pointed by this pointer.
        GraphicsResourceOperation op;         ///< requested operation
        GraphicsResourceId        resourceid; ///< target resource
        int                       targetlod;  ///< ignored for dispose operation.
        GraphicsResourceLoader  * loader;     ///< ignored for dispose operation.
        //@}
    };

    ///
    /// there are several queues in resource thread.
    ///
    class ResourceThreads
    {
        fifo<GraphicsResourceCommand*> copy_queue;
        fifo<GraphicsResourceCommand*> load_queue;
        fifp<GraphicsResourceCommand*> process_queue;

        void copy_loop()
        {
            while( !end_of_game() )
            {
                if( has_copy_request() )
                {
                    GraphicsResourceCommand * r = get_one_copy_requests();
                    copy_resource( r );
                    r->op = OP_UNLOCK;
                    submit_to_rendering_thread( r );
                }
                else
                {
                    wait_until_copy_queue_is_not_empty_or_game_is_about_to_exit();
                }
            }
        }

        void load_loop()
        {
            while( !end_of_game() )
            {
                // block until queue is not empty, or game is about to exit.
                GraphicsResourceCommand * r = get_one_load_request();

                if( r )
                {
                    load_resource( r );
                    submit_to_one_of_processing_threads( r );
                }
            }
        }

        void process_loop
        {
            while( !end_of_game() )
            {
                GraphicsResourceCommand * r = get_one_process_request();

                if( r )
                {
                    proces_resource( r );
                    r->op = OP_LOCK;
                    submit_to_rendering_thread( r );
                }
            }
        }
    };

    ///
    /// ...
    ///
    struct DrawCommand
    {
        int                draw_fence;
        volatile int       pending_resources; ///< number of resources that has to be updated before this draw happend.

        // other fields to describe the draw action.
        ...
    };

    ///
    /// ...
    ///
    class RenderingThread
    {
        void handle_all_resource_requests()
        {
            if( not_resource_queue_empty() )
            {
                for_each( resource in queue )
                {
                    if( the_draw_fence_of_the_resource_is_not_past() )
                    {
                        leave_it_in_queue();
                    }
                    else
                    {
                        switch( resource.operation )
                        {
                            OP_LOCK :
                                lock_resource();
                                break;

                            OP_UNLOCK:
                                unlock_resource();
                                dcrease_pending_resource_number();
                                break;

                            default:
                                unexpected();
                        }

                        remove_resource_from_queue();
                    }
                }
            }
        }

        void render_loop()
        {
            while( !end_of_game() )
            {
                size_t count;
                // block until draw request buffer is not empty or game is about to exit.
                const DrawCommand * requests = get_draw_request_buffer( &count );
                for( size_t i = 0; i < count; )
                {
                    // resource request has priority
                    handle_all_resource_requests();

                    const DrawCommand & dr = requests[i];

                    if( 0 == dr.pending_resource )
                    {
                        do_rendering( dr );
                        update_draw_fence( dr.draw_fence );
                        ++i;
                    }
                    else
                    {
                        sleep_a_short_while(); // or busy loop.
                    }
                }
            }
        }
    };

    class GameThread
    {
        struct Entity
        {
            std::vector<GraphicsResourceId> resources; ///< resources used by this draw
            ...; // other data

            void fill_draw_request( DrawCommand & dq )
            {
                // ...
            };
        };

        std::vector<Entity> entities;

        void game_loop()
        {
        	frame_begin();

            for_each( entities.begin(), entities.end(), draw_entity );

        	frame_end();
        }

        void draw_entity( Entity & e )
        {
            // note:  alloc_draw_request() will assign a fence value to dq.draw_fence
            DrawCommand & dq = alloc_new_draw_request_in_draw_request_buffer();
            compose_and_submit_resource_request( e, dq );
            fill_entity_specific_data_into_draw_request( e ); 
        }

        void compose_and_submit_resource_request( Entity & e, DrawCommand & dq )
        {
            std::vector<GraphicsResourceCommand*> to_be_loaded;
            std::vector<GraphicsResourceCommand*> to_be_disposed;

            const GraphicsResourceId * resources;
            UInt32 count;
            get_resources_used_by_entity( &resources, &count, e );
            const GraphicsResourceId * end = resources + count;

            for( ; resources < end; ++resources )
            {
                GraphicsResourceId newres = *resources;

                if( resource_is_disposed( newres ) )
                {
                    while( there_is_no_enough_room_to_hold_resource( newres ) )
                    {
                        GraphicsResourceId oldres = get_the_appropriate_resource_to_be_disposed();

                        mark_as_disposed( oldres );

                        add_to_list( to_be_disposed, oldres );
                    }

                    mark_as_ready( newres );
                }

                if( need_to_update( newres ) )
                {
                    add_to_list( to_be_loaded, newres );
                    update_resource_draw_fence( newres, draw_fence );
                }
            }

            if( !to_be_disposed.empty() )
            {
                submit_resource_request_to_rendering_thread(
                    OP_DISPOSE,
                    to_be_disposed );
            }

            if( !to_be_loaded.empty() )
            {
                dq.pending_resources = to_be_loaded.size();
                submit_resource_request_to_resource_thread(
                    OP_LOAD,
                    to_be_loaded );
            }
            else
            {
                dq.pending_resource = 0;
            }
        }

        void frame_begin()
        {
            // currently, do nothing
        }

        void frame_end()
        {
            wait_for_rendering_of_last_frame();
            submit_draw_request_buffer_of_this_frame();
            switch_draw_request_buffer_pointer();
        }
    };

    ///
    /// Rendering backend that handles all operations touching rendering device.
    ///
    class RenderEngine
    {
    public:

        /// syncronized command
        //@{

        // reset rendering engine.
        // all resources must be deleted, before calling this function.
        void resetRenderer( const gfx::RendererOptions &, gfx::RendererAPI = gfx::API_AUTO );

        // fence
        UInt32 getCurrentFence();

        // resource management
        void deleteShader( result, gfx::Shader * );
        void deleteTexture( gfx::Texture * );
        void deleteVtxBuf( gfx::VtxBuf * );
        void deleteIdxBuf( gfx::IdxBuf * );

        // sync operations
        void waitForIdle();          ///< wait until all submitted async command are executed
        void waitForFence( UInt32 ); ///< wait until specific fence is passed.

        //@}

        /// asyncronized commands
        //@{

        // fence
        UInt32 insertFence();

        /* resource instance creation (rely one resource cache implementation)
        void createShader( gfx::Shader * result, gfx::ShaderType type, gfx::ShadingLanguage lang, const StrA & code, const StrA & hints );
        void createTexture( gfx::Shader * result, const gfx::TextureDesc &, const gfx::ImageDesc * initialData );
        void createVtxBuf( gfx::VtxBuf * result, const gfx::VtxBufDesc &, const void * initialData );
        void createIdxBuf( gfx::IdxBuf * result, const gfx::IdxBufDesc &, const void * initialData );

        // resource content update
        void updateTexture( gfx::Texture *, ... );
        void updateVtxBuf( gfx::VtxBuf *, ... );
        void updateIdxBuf( gfx::IdxBuf *, ... );
        */

        // rendering
        void updateContext( const gfx::RendererContext & ctx );
        void draw( gfx::PrimitiveType prim, UInt32 numprim, UInt32 startvtx );
        void drawIndexed( gfx::PrimitiveType prim, UInt32 numprim, UInt32 startvtx, UInt32 startidx );
        //void drawUp( gfx::PrimitiveType prim, UInt32 numprim, ... );
        //void drawIndexedUp(...);
        //void drawLines(...);

        //@}
    };
}}

// *****************************************************************************
//                           End of renderEngine_draft.h
// *****************************************************************************
#endif // __GN_ENGINE_RENDERENGINE_H__
