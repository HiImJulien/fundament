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
};

struct fn_imp_render_device {
    ID3D11Device*           device;
    ID3D11DeviceContext*    context;
    IDXGIFactory*           factory;

    struct fn_imp_pipeline* pipelines;
    size_t                  pipelines_capacity;
    struct fn_imp_shader*   shaders;
    size_t                  shaders_capacity;
    ID3D11Buffer**          buffers;
    size_t                  buffers_capacity;
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
        (void**) dxgi_device
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

    return FN_HANDLE(fn_render_device, id);
}

void fn_destroy_render_device(struct fn_render_device device) {
    assert(0 && "Not implemented yet!");
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
        (void**) reflector
    );

    if(FAILED(hr))
        return FN_INVALID(fn_pipeline);

    size_t element_count = 0;
    for(size_t it = 0; it < 32; ++it) {
        if(desc.layout[it].format == fn_data_format_none)
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

    if(FAILED(hr)) {
        FN_DX_RELEASE(reflector);
        return FN_INVALID(fn_pipeline);
    }

    FN_DX_ADDREF(vs->vertex_shader);
    FN_DX_ADDREF(ps->pixel_shader);
    pipe->vertex_shader = vs->vertex_shader;
    pipe->pixel_shader = ps->pixel_shader;
    pipe->topology = fn_imp_map_topology(desc.topology);

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

void fn_apply_bindings(
    struct fn_render_device device,
    struct fn_buffer_binding desc
) {
    if(!FN_CHECK_HANDLE(device))
        return;

    struct fn_imp_render_device* dev = g_imp_devices[FN_IDX(device)];
    assert(dev != NULL);

    size_t min_slot = 32;
    size_t max_slot = 0;
    ID3D11Buffer* vertex_buffers[32] = {0};
    UINT strides[32] = {0};
    UINT offsets[32] = {0};
    for(size_t it = 0; it < desc.vertex_buffer_count; ++it) {
        size_t slot = desc.vertex_buffers[it].slot;
        vertex_buffers[slot] = 
            dev->buffers[FN_IDX(desc.vertex_buffers[it].buffer)];

        strides[slot] = desc.vertex_buffers[it].stride;
        offsets[slot] = desc.vertex_buffers[it].offset;

        if(slot > max_slot)
            max_slot = slot;

        if(slot < min_slot)
            min_slot = slot;
    }

    ID3D11DeviceContext_IASetVertexBuffers(
        dev->context,
        min_slot,
        desc.vertex_buffer_count,
        &vertex_buffers[min_slot],
        &strides[min_slot],
        &offsets[max_slot]
    );

    if(!FN_CHECK_HANDLE(desc.index_buffer.buffer)) 
        return;

    ID3D11DeviceContext_IASetIndexBuffer(
        dev->context,
        dev->buffers[FN_IDX(desc.index_buffer.buffer)],
        fn_imp_map_format(desc.index_buffer.format),
        (UINT) desc.index_buffer.offset
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
} 

#endif // _WIN32