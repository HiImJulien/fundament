#include <fundament/gfx.h>

#include <assert.h>

#define FN_IDX(handle) (handle.id - 1)
#define FN_HANDLE(type, id) (struct type) { id + 1} 
#define FN_INVALID(type) (struct type) { 0 }
#define FN_CHECK_HANDLE(handle) (handle.id != 0)

#if defined(_WIN32)

#ifndef COBJMACROS
#define COBJMACROS
#endif  // COBJMACROS

#define FN_DX_ADDREF(ptr) ptr->lpVtbl->AddRef(ptr)
#define FN_DX_RELEASE(ptr) ptr->lpVtbl->Release(ptr)

#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

inline static D3D11_PRIMITIVE_TOPOLOGY fn_imp_map_topology(
    enum fn_topology_type top
) {
    const D3D11_PRIMITIVE_TOPOLOGY mapping[] = {
        D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,         
        D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,         
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST,          
        D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,         
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,      
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,     
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,      
        D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,     
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,  
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ, 
    };

    return mapping[top];
}

inline static DXGI_FORMAT fn_imp_map_format(enum fn_data_format fmt) {
    const DXGI_FORMAT mapping[] = {
        DXGI_FORMAT_UNKNOWN,
        DXGI_FORMAT_R32G32B32A32_TYPELESS,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32_UINT,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UINT,
    };

    return mapping[fmt];
}

inline static UINT fn_imp_map_bind_flags(enum fn_resource_bind_flags flags) {
    UINT mapping = 0;

    if(flags & fn_resource_bind_flags_vertex) 
        mapping |= D3D11_BIND_VERTEX_BUFFER;

    if(flags & fn_resource_bind_flags_index) 
        mapping |= D3D11_BIND_INDEX_BUFFER;

    if(flags & fn_resource_bind_flags_constant) 
        mapping |= D3D11_BIND_CONSTANT_BUFFER;

    if(flags & fn_resource_bind_flags_shader_resource) 
        mapping |= D3D11_BIND_SHADER_RESOURCE;

    if(flags & fn_resource_bind_flags_render_target) 
        mapping |= D3D11_BIND_RENDER_TARGET;

    return mapping;
}

inline static UINT fn_imp_map_access_flags(enum fn_resource_access_flags flags) {
    UINT mapping = 0;

    if(flags == fn_resource_access_flags_none)
        return 0;

    if(flags & fn_resource_access_flags_write)
        mapping |= D3D11_CPU_ACCESS_WRITE;

    if(flags & fn_resource_access_flags_read)
        mapping |= D3D11_CPU_ACCESS_READ;

    return mapping;
}

inline static D3D11_BLEND fn_imp_map_blend(enum fn_blend_factor factor) {
    const D3D11_BLEND mapping[] = {
        D3D11_BLEND_ZERO,
        D3D11_BLEND_ONE,
        D3D11_BLEND_SRC_COLOR,
        D3D11_BLEND_INV_SRC_COLOR,
        D3D11_BLEND_SRC_ALPHA,
        D3D11_BLEND_INV_SRC_ALPHA,
        D3D11_BLEND_DEST_ALPHA,
        D3D11_BLEND_INV_DEST_ALPHA,
        D3D11_BLEND_DEST_COLOR,
        D3D11_BLEND_INV_DEST_COLOR,
    };

    return mapping[factor];
}

inline static D3D11_BLEND_OP fn_imp_map_blend_op(enum fn_blend_operation op) {
    const D3D11_BLEND_OP mapping[] = {
        D3D11_BLEND_OP_ADD,
        D3D11_BLEND_OP_SUBTRACT,
        D3D11_BLEND_OP_REV_SUBTRACT,
        D3D11_BLEND_OP_MIN,
        D3D11_BLEND_OP_MAX
    };

    return mapping[op];
}

inline static D3D11_BLEND_DESC fn_imp_map_blend_desc(struct fn_blend_desc* desc) {
    D3D11_BLEND_DESC b_desc = {0};

    b_desc.AlphaToCoverageEnable = desc->alpha_coverage;
    b_desc.IndependentBlendEnable = desc->independent_blend;

    for(size_t it = 0; it < 8; ++it) {
        D3D11_RENDER_TARGET_BLEND_DESC* b_target = 
            &b_desc.RenderTarget[it];

        struct fn_target_blend_desc* target = &desc->target_blend_desc[it];

        b_target->BlendEnable = target->enable;
        b_target->SrcBlend = fn_imp_map_blend(target->src);
        b_target->DestBlend = fn_imp_map_blend(target->dest);
        b_target->BlendOp = fn_imp_map_blend_op(target->op);
        b_target->SrcBlendAlpha = fn_imp_map_blend(target->alpha_src);
        b_target->DestBlendAlpha = fn_imp_map_blend(target->alpha_dest);
        b_target->BlendOpAlpha = fn_imp_map_blend_op(target->alpha_op);
        b_target->RenderTargetWriteMask = target->mask; 
    }

    return b_desc;
}

inline static D3D11_RASTERIZER_DESC fn_imp_map_rast_desc(
    struct fn_rasterizer_desc* desc) {
    const D3D11_FILL_MODE fill_mapping[] = {
        D3D11_FILL_WIREFRAME,
        D3D11_FILL_SOLID
    };

    const D3D11_CULL_MODE cull_mapping[] = {
        D3D11_CULL_NONE,
        D3D11_CULL_FRONT,
        D3D11_CULL_BACK
    };

    D3D11_RASTERIZER_DESC r_desc = {0};
    r_desc.FillMode = fill_mapping[desc->fill];
    r_desc.CullMode = cull_mapping[desc->cull];
    r_desc.FrontCounterClockwise = !desc->clock_wise;
    r_desc.DepthBias = desc->depth_bias;
    r_desc.DepthBiasClamp = desc->depth_bias_clamp;
    r_desc.SlopeScaledDepthBias = desc->scaled_depth_bias;
    r_desc.DepthClipEnable = desc->clip;
    r_desc.ScissorEnable = desc->scissor;
    r_desc.MultisampleEnable = desc->multisample;
    r_desc.AntialiasedLineEnable = desc->antialias;

    return r_desc;
}

struct fn_imp_shader {
    enum fn_shader_type type;
    const void*         byte_code;
    size_t              byte_code_size;

    union {
        ID3D11VertexShader* vertex_shader;
        ID3D11PixelShader*  pixel_shader;
    };
};

struct fn_imp_pipeline {
    ID3D11InputLayout*          layout;
    ID3D11VertexShader*         vertex_shader;
    ID3D11PixelShader*          pixel_shader;
    D3D11_PRIMITIVE_TOPOLOGY    topology;
    ID3D11BlendState*           blend;
    float                       blend_factor[4];
    uint32_t                    sample_mask;
    ID3D11RasterizerState*      rasterizer;
};

struct fn_imp_swap_chain {
    IDXGISwapChain*         swap_chain;
    ID3D11RenderTargetView* target_view;
};

struct fn_imp_texture {
    enum fn_texture_type    type;

    union {
        ID3D11Texture1D*    tex1d;
        ID3D11Texture2D*    tex2d;
        ID3D11Texture3D*    tex3d;
    };
};

struct fn_imp_render_device {
    ID3D11Device*               device;
    ID3D11DeviceContext*        context;
    IDXGIFactory*               factory;

    struct fn_imp_pipeline*     pipelines;
    size_t                      pipelines_capacity;
    struct fn_imp_shader*       shaders;
    size_t                      shaders_capacity;
    ID3D11Buffer**              buffers;
    size_t                      buffers_capacity;
    struct fn_imp_swap_chain*   swap_chains;
    size_t                      swap_chain_capacity;
    struct fn_imp_texture*      textures;
    size_t                      texture_capacity;
};

static struct fn_imp_render_device* g_imp_devices[4];

struct fn_render_device fn_create_render_device() {
    uint32_t id = 0;
    for(; id < 4; ++id) {
        if(id == 3 && g_imp_devices[id] != NULL)
            return FN_INVALID(fn_render_device);

        if(g_imp_devices[id] == NULL)
            break;
    }

    struct fn_imp_render_device* dev = malloc(
        sizeof(struct fn_imp_render_device)
    ); 

    HRESULT hr = D3D11CreateDevice(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_DEBUG,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &dev->device,
        NULL,
        &dev->context
    );

    if(FAILED(hr)) {
        free(g_imp_devices[id]);
        g_imp_devices[id] = NULL;
        return FN_INVALID(fn_render_device);
    }

    IDXGIDevice* dxgi_device = NULL;
    hr = ID3D11Device_QueryInterface(
        dev->device,
        &IID_IDXGIDevice,
        (void**) &dxgi_device
    );

    if(FAILED(hr)) {
        FN_DX_RELEASE(dev->device);
        free(g_imp_devices[id]);
        g_imp_devices[id] = NULL;
        return FN_INVALID(fn_render_device);
    }

    IDXGIAdapter* dxgi_adapter = NULL;
    hr = IDXGIDevice_GetAdapter(dxgi_device, &dxgi_adapter);

    if(FAILED(hr)) {
        FN_DX_RELEASE(dxgi_device);
        FN_DX_RELEASE(dev->device);
        free(g_imp_devices[id]);
        g_imp_devices[id] = NULL;
        return FN_INVALID(fn_render_device);
    }

    hr = IDXGIAdapter_GetParent(
        dxgi_adapter, 
        &IID_IDXGIFactory, 
        (void**) &dev->factory
    );

    FN_DX_RELEASE(dxgi_adapter);
    FN_DX_RELEASE(dxgi_device);

    if(FAILED(hr)) {
        FN_DX_RELEASE(dev->device);
        free(g_imp_devices[id]);
        g_imp_devices[id] = NULL;
        return FN_INVALID(fn_render_device);
    }

    dev->pipelines_capacity = 2048;
    dev->pipelines = calloc(
        dev->pipelines_capacity,
        sizeof(struct fn_imp_pipeline)
    );

    dev->shaders_capacity = 256;
    dev->shaders = calloc(
        dev->shaders_capacity,
        sizeof(struct fn_imp_shader)
    );

    dev->buffers_capacity = 2048;
    dev->buffers = calloc(
        dev->buffers_capacity,
        sizeof(ID3D11Buffer*)
    );

    dev->swap_chain_capacity = 8;
    dev->swap_chains = calloc(
        dev->swap_chain_capacity,
        sizeof(struct fn_imp_swap_chain)
    );

    dev->texture_capacity = 4096;
    dev->textures = calloc(
        dev->texture_capacity,
        sizeof(struct fn_imp_texture)
    );

    g_imp_devices[id] = dev;
    return FN_HANDLE(fn_render_device, id);
}

void fn_destroy_render_device(struct fn_render_device device) {
    assert(0 && "Not implemented yet!");
}

struct fn_swap_chain fn_create_swap_chain(
    struct fn_render_device device,
    struct fn_swap_chain_desc desc
) {
    if(!FN_CHECK_HANDLE(device))
        return FN_INVALID(fn_swap_chain);

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    uint32_t id = 0;
    for(; id <= dev->swap_chain_capacity; ++id) {
        if(id == dev->swap_chain_capacity)
            return FN_INVALID(fn_swap_chain);

        if(dev->swap_chains[id].swap_chain == NULL)
            break;
    }

    DXGI_SWAP_CHAIN_DESC sc_desc = {0};
    sc_desc.BufferDesc.Width = (UINT) desc.width;
    sc_desc.BufferDesc.Height = (UINT) desc.height;
    sc_desc.BufferDesc.Format = fn_imp_map_format(desc.format);
    sc_desc.SampleDesc.Count = 1;
    sc_desc.SampleDesc.Quality = 0;
    sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc_desc.BufferCount = 1;
    sc_desc.OutputWindow = (HWND) desc.window;
    sc_desc.Windowed = true;
    sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sc_desc.Flags = 0;    

    IDXGISwapChain* swap_chain = NULL;
    HRESULT hr = IDXGIFactory_CreateSwapChain(
        dev->factory,
        (IUnknown*) dev->device,
        &sc_desc,
        &swap_chain
    );

    if(FAILED(hr))
        return FN_INVALID(fn_swap_chain);

    ID3D11Texture2D* back_buffer = NULL;
    hr = IDXGISwapChain_GetBuffer(
        swap_chain,
        0,
        &IID_ID3D11Texture2D,
        (void**) &back_buffer
    );

    if(FAILED(hr)) {
        FN_DX_RELEASE(swap_chain);
        return FN_INVALID(fn_swap_chain);
    }

    ID3D11RenderTargetView* target_view = NULL;
    hr = ID3D11Device_CreateRenderTargetView(
        dev->device,
        (ID3D11Resource*) back_buffer,
        NULL,
        &target_view
    );

    FN_DX_RELEASE(back_buffer);

    if(FAILED(hr)) {
        FN_DX_RELEASE(swap_chain);
        return FN_INVALID(fn_swap_chain);
    }

    dev->swap_chains[id] = (struct fn_imp_swap_chain) {
        .swap_chain = swap_chain,
        .target_view = target_view
    };

    return FN_HANDLE(fn_swap_chain, id);
}

struct fn_shader fn_create_shader(
    struct fn_render_device device,
    struct fn_shader_desc desc
) {
    assert(desc.byte_code != NULL && desc.byte_code_size != 0);

    if(!FN_CHECK_HANDLE(device))
        return FN_INVALID(fn_shader);

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    uint32_t id = 0;
    for(; id <= dev->shaders_capacity; ++id) {
        if(id == dev->shaders_capacity)
            return FN_INVALID(fn_shader);

        if(dev->shaders[id].vertex_shader == NULL)
            break;
    }

    struct fn_imp_shader* shader = &dev->shaders[id];
    HRESULT hr = S_OK;
    if(desc.type == fn_shader_type_vertex)
            hr = ID3D11Device_CreateVertexShader(
                dev->device,
                desc.byte_code,
                (SIZE_T) desc.byte_code_size,
                NULL,
                &shader->vertex_shader
            );
    else if(desc.type == fn_shader_type_pixel)
            hr = ID3D11Device_CreatePixelShader(
                dev->device,
                desc.byte_code,
                (SIZE_T) desc.byte_code_size,
                NULL,
                &shader->pixel_shader
            );
    else
        return FN_INVALID(fn_shader);

    if(FAILED(hr))
        return FN_INVALID(fn_shader);

    shader->byte_code = malloc(desc.byte_code_size);
    memcpy((void*) shader->byte_code, desc.byte_code, desc.byte_code_size);
    shader->byte_code_size = desc.byte_code_size;

    return FN_HANDLE(fn_shader, id);
}

struct fn_pipeline fn_create_pipeline(
    struct fn_render_device device,
    struct fn_pipeline_desc desc
) {
    if(!FN_CHECK_HANDLE(device) 
        || !FN_CHECK_HANDLE(desc.vertex_shader)
        || !FN_CHECK_HANDLE(desc.pixel_shader)
        || (desc.topology == fn_topology_type_none)
        || (desc.layout[0].format == fn_data_format_none))
        return FN_INVALID(fn_pipeline);

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    uint32_t id = 0;
    for(; id <= dev->pipelines_capacity; ++id) {
        if(id == dev->pipelines_capacity)
            return FN_INVALID(fn_pipeline);

        if(dev->pipelines[id].layout == NULL)
            break;
    }

    struct fn_imp_pipeline* pipe = &dev->pipelines[id];
    struct fn_imp_shader* vs = &dev->shaders[FN_IDX(desc.vertex_shader)];
    struct fn_imp_shader* ps = &dev->shaders[FN_IDX(desc.pixel_shader)];

    ID3D11ShaderReflection* reflector = NULL;
    HRESULT hr = D3DReflect(
        vs->byte_code,
        vs->byte_code_size,
        &IID_ID3D11ShaderReflection,
        (void**) &reflector
    );

    if(FAILED(hr))
        return FN_INVALID(fn_pipeline);

    size_t element_count = 0;
    for(; element_count < 32; ++element_count) {
        if(desc.layout[element_count].format == fn_data_format_none)
            break;
    }

    D3D11_SHADER_DESC shader_desc;
    hr = reflector->lpVtbl->GetDesc(reflector, &shader_desc);

    if(FAILED(hr)) {
        FN_DX_RELEASE(reflector);
        return FN_INVALID(fn_pipeline);
    }

    if(shader_desc.InputParameters != element_count)
        return FN_INVALID(fn_pipeline);

    D3D11_INPUT_ELEMENT_DESC layout_desc[32];
    for(size_t it = 0; it < element_count; ++it) {
        D3D11_SIGNATURE_PARAMETER_DESC param;
        reflector->lpVtbl->GetInputParameterDesc(
            reflector,
            (UINT) it,
            &param
        );

        layout_desc[it].SemanticName = param.SemanticName;
        layout_desc[it].SemanticIndex = param.SemanticIndex;
        layout_desc[it].Format = fn_imp_map_format(desc.layout[it].format);
        layout_desc[it].InputSlot = (UINT) desc.layout[it].slot;
        layout_desc[it].AlignedByteOffset = (UINT) desc.layout[it].byte_offset;
        layout_desc[it].InputSlotClass = desc.layout[it].is_per_instance
            ? D3D11_INPUT_PER_INSTANCE_DATA
            : D3D11_INPUT_PER_VERTEX_DATA;

        layout_desc[it].InstanceDataStepRate = 
            (UINT) desc.layout[it].steps_per_instance;
    }

    hr = ID3D11Device_CreateInputLayout(
        dev->device,
        layout_desc,
        (UINT) element_count,
        vs->byte_code,
        vs->byte_code_size,
        &pipe->layout
    );

    FN_DX_RELEASE(reflector);

    if(FAILED(hr))
        return FN_INVALID(fn_pipeline);

    FN_DX_ADDREF(vs->vertex_shader);
    FN_DX_ADDREF(ps->pixel_shader);
    pipe->vertex_shader = vs->vertex_shader;
    pipe->pixel_shader = ps->pixel_shader;
    pipe->topology = fn_imp_map_topology(desc.topology);

    D3D11_BLEND_DESC blend_desc = fn_imp_map_blend_desc(&desc.blend);
    if(desc.blend.enable) {
        hr = ID3D11Device_CreateBlendState(
            dev->device,
            &blend_desc,
            &pipe->blend
        );

        pipe->blend_factor[0] = desc.blend.blend_factor[0];
        pipe->blend_factor[1] = desc.blend.blend_factor[1];
        pipe->blend_factor[2] = desc.blend.blend_factor[2];
        pipe->blend_factor[3] = desc.blend.blend_factor[3];
        pipe->sample_mask = desc.blend.sample_mask;
    } else 
        pipe->blend = NULL;

    if(FAILED(hr)) {
        FN_DX_RELEASE(vs->vertex_shader);
        FN_DX_RELEASE(ps->pixel_shader);
        FN_DX_RELEASE(pipe->layout);
        return FN_INVALID(fn_pipeline);
    }

    D3D11_RASTERIZER_DESC r_desc = fn_imp_map_rast_desc(&desc.rasterizer);
    hr = ID3D11Device_CreateRasterizerState(
        dev->device,
        &r_desc,
        &pipe->rasterizer
    );

    if(FAILED(hr)) {
        FN_DX_RELEASE(vs->vertex_shader);
        FN_DX_RELEASE(ps->pixel_shader);
        FN_DX_RELEASE(pipe->layout);

        if(pipe->blend)
            FN_DX_RELEASE(pipe->blend);
        return FN_INVALID(fn_pipeline);
    }

    return FN_HANDLE(fn_pipeline, id);
}

struct fn_buffer fn_create_buffer(
    struct fn_render_device device,
    struct fn_buffer_desc desc
) {
    if(!FN_CHECK_HANDLE(device))
        return FN_INVALID(fn_buffer);

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    uint32_t id = 0;
    for(; id <= dev->buffers_capacity; ++id) {
        if(id == dev->buffers_capacity)
            return FN_INVALID(fn_buffer);

        if(dev->buffers[id] == NULL)
            break;
    }

    D3D11_BUFFER_DESC buffer_desc = {0};
    buffer_desc.ByteWidth = (UINT) desc.size;
    buffer_desc.Usage = 0; // TODO: Map usage!
    buffer_desc.BindFlags = fn_imp_map_bind_flags(desc.bind_flags);

    D3D11_SUBRESOURCE_DATA data_desc = {0};
    data_desc.pSysMem = desc.data;
    data_desc.SysMemPitch = desc.pitch;
    data_desc.SysMemSlicePitch = desc.slice_pitch;

    ID3D11Buffer** buffer = &dev->buffers[id];
    HRESULT hr = ID3D11Device_CreateBuffer(
        dev->device,
        &buffer_desc,
        &data_desc,
        buffer
    );

    return FAILED(hr) ? FN_INVALID(fn_buffer) : FN_HANDLE(fn_buffer, id);
}

struct fn_texture fn_create_texture(
    struct fn_render_device device,
    struct fn_texture_desc desc,
    struct fn_data_desc data) {
    if(!FN_CHECK_HANDLE(device))
        return FN_INVALID(fn_texture);

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    if(desc.type == fn_texture_type_1d) {

    }

    return FN_INVALID(fn_texture);
}

void fn_apply_bindings(
    struct fn_render_device device,
    struct fn_buffer_binding desc
) {
    if(!FN_CHECK_HANDLE(device))
        return;

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    UINT strides[32] = {0};
    UINT offsets[32] = {0};

    ID3D11Buffer* buffers[32] = {0};
    for(size_t it = 0; it < desc.vertex_buffer_count; ++it) {

        buffers[it] = 
            dev->buffers[FN_IDX(desc.vertex_buffers[it].buffer)];

        strides[it] = desc.vertex_buffers[it].stride;
        offsets[it] = desc.vertex_buffers[it].offset;
    }

    ID3D11DeviceContext_IASetVertexBuffers(
        dev->context,
        0,
        desc.vertex_buffer_count,
        buffers,
        strides,
        offsets
    );

    if(FN_CHECK_HANDLE(desc.index_buffer.buffer)) {
        ID3D11DeviceContext_IASetIndexBuffer(
            dev->context,
            dev->buffers[FN_IDX(desc.index_buffer.buffer)],
            fn_imp_map_format(desc.index_buffer.format),
            (UINT) desc.index_buffer.offset
        );
    }

    ID3D11RenderTargetView* views[8] = {NULL};

    size_t render_target_count = 0;
    for(; render_target_count < 8; ++render_target_count) {
        if(desc.swap_chains[render_target_count].id == 0)
            break;

        struct fn_imp_swap_chain* sc =
            &dev->swap_chains[render_target_count];

        views[render_target_count] = sc->target_view;
    }

    ID3D11DeviceContext_OMSetRenderTargets(
        dev->context,
        (UINT) render_target_count,
        views,
        NULL
    );
}

void fn_apply_pipeline(
    struct fn_render_device device,
    struct fn_pipeline pipeline
) {
    if(!FN_CHECK_HANDLE(device) || !FN_CHECK_HANDLE(pipeline))
        return;

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    struct fn_imp_pipeline* pipe = &dev->pipelines[FN_IDX(pipeline)];

    ID3D11DeviceContext_IASetInputLayout(
        dev->context,
        pipe->layout
    );

    ID3D11DeviceContext_IASetPrimitiveTopology(
        dev->context,
        pipe->topology
    );

    ID3D11DeviceContext_VSSetShader (
        dev->context,
        pipe->vertex_shader,
        NULL,
        0
    );

    ID3D11DeviceContext_PSSetShader(
        dev->context,
        pipe->pixel_shader,
        NULL,
        0
    );

    ID3D11DeviceContext_RSSetState(
        dev->context,
        pipe->rasterizer
    );

    if(pipe->blend) {
        ID3D11DeviceContext_OMSetBlendState(
            dev->context,
            pipe->blend,
            pipe->blend_factor,
            pipe->sample_mask
        );
    }
}

void fn_draw(
    struct fn_render_device device,
    size_t vertex_count,
    size_t vertex_offset
) {
    if(!FN_CHECK_HANDLE(device))
        return;

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    D3D11_VIEWPORT vp = {0};
    vp.Width = 300;
    vp.Height = 300;
    vp.MinDepth = 0;
    vp.MaxDepth = 1.f;

    ID3D11DeviceContext_RSSetViewports(
        dev->context,
        1,
        &vp
    );


    ID3D11DeviceContext_Draw(
        dev->context,
        (UINT) vertex_count,
        (UINT) vertex_offset
    );
}

void fn_clear(
    struct fn_render_device device, 
    struct fn_swap_chain sc,
    float r, float g, float b, float a) {
    if(!FN_CHECK_HANDLE(device) || !FN_CHECK_HANDLE(sc))
        return;

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    float rgba[4] = {r, g, b, a};
    struct fn_imp_swap_chain* isc = &dev->swap_chains[FN_IDX(sc)];
    ID3D11DeviceContext_ClearRenderTargetView(
        dev->context,
        isc->target_view,
        rgba
    );
}

void fn_present(struct fn_render_device device, struct fn_swap_chain sc) {
    if(!FN_CHECK_HANDLE(device) || !FN_CHECK_HANDLE(sc))
        return;

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    struct fn_imp_swap_chain* isc = &dev->swap_chains[FN_IDX(sc)];

    IDXGISwapChain_Present(
        isc->swap_chain,
        0,
        0
    );
}

#endif // _WIN32

#if defined(__APPLE__)

#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

struct fn_imp_render_device {
    id<MTLDevice>  device;
};

static struct fn_imp_render_device* g_imp_devices[4];

struct fn_render_device fn_create_render_device() {
    uint32_t id = 0;
    for(; id <= 4; ++id) {
        if(id == 4)
            return FN_INVALID(fn_render_device);

        if(g_imp_devices[id] == NULL)
            break;
    }

    struct fn_imp_render_device* dev = calloc(
        1,
        sizeof(struct fn_imp_render_device)
    );

    dev->device = MTLCreateSystemDefaultDevice();
    if(dev->device == nil) {
        free(dev);
        return FN_INVALID(fn_render_device);
    }

    return FN_HANDLE(fn_render_device, id);
}

void fn_destroy_render_device(struct fn_render_device device) {

}

struct fn_swap_chain fn_create_swap_chain(
    struct fn_render_device device,
    struct fn_swap_chain_desc desc
) {
    NSWindow* window = desc.window;
    NSView* view = [window contentView];
    view.wantsLayer = YES;
    view.layer = [CAMetalLayer layer];

    return FN_INVALID(fn_swap_chain);
}

struct fn_shader fn_create_shader(
    struct fn_render_device device,
    struct fn_shader_desc desc
) {
    return FN_INVALID(fn_shader);
}

struct fn_pipeline fn_create_pipeline(
    struct fn_render_device device,
    struct fn_pipeline_desc desc
) {
    return FN_INVALID(fn_pipeline);
}

struct fn_buffer fn_create_buffer(
    struct fn_render_device device,
    struct fn_buffer_desc desc
) {
    return FN_INVALID(fn_buffer);
}

struct fn_texture fn_create_texture(
    struct fn_render_device device,
    struct fn_texture_desc desc,
    struct fn_data_desc data
) {
    return FN_INVALID(fn_texture);
}

void fn_apply_bindings(
    struct fn_render_device device,
    struct fn_buffer_binding desc
) {

}

void fn_apply_pipeline(
    struct fn_render_device device,
    struct fn_pipeline pipeline
) {

}

void fn_draw(
    struct fn_render_device device,
    size_t vertex_count,
    size_t vertex_offset
) {

}

void fn_clear(
    struct fn_render_device device, 
    struct fn_swap_chain sc,
    float r, float g, float b, float a
) {

}

void fn_present(struct fn_render_device device, struct fn_swap_chain sc) {

}








#endif