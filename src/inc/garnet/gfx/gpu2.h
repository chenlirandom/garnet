#include "../GNwin.h"
#include <vector>

namespace GN
{
///
/// namespace for GFX module
///
namespace gfx
{
    struct Gpu2 : public RefCounter
    {
        //@{
        enum class GraphicsAPI
        {
            AUTO,
            D3D12,
            VULKAN,
        };
        struct CreationParameters
        {
            win::Window * window;
            GraphicsAPI   api = GraphicsAPI::AUTO;
            uint32_t      width = 0, height = 0; // back buffer size. set to 0 to use window size.
            bool          fullscreen = false;
        };
        static GN_API AutoRef<Gpu2> createGpu2(const CreationParameters &);
        //@}

        /// GPU pipeline
        //@{
        struct PipelineCreationParameters
        {
            const uint8_t * compiledGpuProgramBinary;
            uint64_t        compiledGpuProgramSizeInBytes;
            const char * states; // pipeline state defined in JSON format.
        };
        struct Pipeline : public RefCounter
        {
        };
        virtual std::vector<Pipeline> createPipelineObjects(const PipelineCreationParameters *, size_t) = 0;
        //@}

        struct Surface;

        /// GPU command list
        //@{
        struct CopyParameters
        {
            Surface * source;
            uint32_t  sourceSubresourceId;
            Surface * dest;
            uint32_t  destSubresourceId;
        };
        struct DrawParameters
        {
            const Pipeline * pipeline;
            uint32_t         indexcount;
            uint32_t         basevertex;
            uint32_t         baseindex;
            uint32_t         primitive;
        };
        struct CommandListCreationParameters
        {

        };
        struct CommandList : public RefCounter
        {
            virtual void     copy(const CopyParameters &) = 0;
            virtual void     draw(const DrawParameters *) = 0;
            virtual uint64_t mark() = 0; // insert a new fence into command list, returns fence id.
            virtual void     wait(uint64_t fence) = 0; // insert a wait-for-fence into command list
        };
        virtual AutoRef<CommandList> createCommandList(const CommandListCreationParameters &) = 0;
        virtual void                 kickoff(CommandList &) = 0; ///< kick off command list.
        //@}

        /// GPU memory pool
        //@{
        struct MemPoolCreationParameters
        {
            uint64_t sizeInMB;
        };
        struct MemPool : public RefCounter
        {

        };
        virtual AutoRef<MemPool>  createMemoryPool(const MemPoolCreationParameters &) = 0;
        //@}

        /// GPU program compilation
        //@{
        enum ShadingLanguage
        {
            HLSL,
            GLSL,
        };
        struct ProgramSource
        {
            ShadingLanguage lang; ///< shading language
            bool            debugable = false; ///< set to true to generate shader binary with debug information.
            bool            optimized = true;  ///< set to true to generate optimized shader.
            const char *    vs = nullptr;
            const char *    ts = nullptr;
            const char *    ds = nullptr;
            const char *    gs = nullptr;
            const char *    ps = nullptr;
            const char *    cs = nullptr;
        };
        virtual std::vector<uint8> compileProgram(const ProgramSource &) = 0;
        //@}

        /// GPU surface
        //@{
        struct SurfaceCreationParameters
        {
            union
            {
                struct TextureDesc
                {
                    uint32_t w, h, d, a, l;
                } t;

                struct BufferDesc
                {
                    uint32_t bytes;
                } b;
            };
            MemPool * pool = nullptr;
            uint64_t     offset = 0; // ignored if pool is null.
        };
        /// this could be a texture or a buffer.
        struct Surface : public RefCounter
        {
            /// For immediate update, no dealy, no async. do hazard tracking yourself.
            virtual void * getPersistentPointer(uint32_t subResourceId, uint32_t * pRowPitch, uint32_t * pSlicePitch) = 0;
        };
        virtual AutoRef<Surface> createSurface(const SurfaceCreationParameters &) = 0;
        //@}

        //@{
        struct QueryCreationParameters
        {
        };
        struct Query : public RefCounter
        {
        };
        virtual AutoRef<Query> createQuery(const QueryCreationParameters &) = 0;
        //@}

        /// present
        //@{
        struct PresentParameters
        {

        };
        virtual void present(const PresentParameters &) = 0;
        //@}

        /// misc
        //@{
        virtual void wait(uint64_t fence) = 0; ///< block caller thread until the fence completes.
        virtual bool testFence(uint64_t fence) = 0;
        //}
    };
} // end of namespace gfx
} // end of namespace GN
