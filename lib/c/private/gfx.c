#include <fundament/gfx.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)

#ifndef COBJMACROS
#define COBJMACROS
#endif 	// COBJMACROS

#include <d3d11.h>
#include <d3d11shader.h>

inline static DXGI_FORMAT fn_imp_map_format(enum fn_gfx_data_format fmt) {
	const DXGI_FORMAT mapping[] = {
		DXGI_FORMAT_R32G32B32A32_TYPELESS,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_UINT,
		DXGI_FORMAT_R32G32B32A32_SINT,
		DXGI_FORMAT_R32G32B32_TYPELESS,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_UINT,
		DXGI_FORMAT_R32G32B32_SINT,
		DXGI_FORMAT_R8G8B8A8_TYPELESS,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_R8G8B8A8_UINT,
		DXGI_FORMAT_R8G8B8A8_SNORM,
		DXGI_FORMAT_R8G8B8A8_SINT
	};

	return mapping[fmt];
}

inline static UINT fn_imp_map_bind_flags(enum fn_gfx_buffer_type type) {
	const D3D11_BIND_FLAG mapping[] = {
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_BIND_INDEX_BUFFER,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_BIND_SHADER_RESOURCE
	};

	return mapping[type];
}

inline static D3D11_USAGE fn_imp_map_usage(enum fn_gfx_buffer_usage usage) {
	const D3D11_USAGE mapping[] = {
		D3D11_USAGE_DEFAULT,
		D3D11_USAGE_IMMUTABLE,
		D3D11_USAGE_DYNAMIC,
		D3D11_USAGE_STAGING
	};

	return mapping[usage];
}

struct fn_imp_swap_chain {
	IDXGISwapChain*			swap_chain;
	ID3D11RenderTargetView* render_target;
};

struct fn_imp_shader {
	union {
		ID3D11VertexShader*	vertex;
		ID3D11HullShader* hull;
		ID3D11DomainShader* domain;
		ID3D11GeometryShader* geometry;
		ID3D11PixelShader* pixel;
		ID3D11ComputeShader* compute;
	};

	enum fn_gfx_shader_type type;
	uint8_t* byte_code;
	size_t byte_code_size;
};

struct fn_imp_pipeline {
	ID3D11InputLayout* layout;
	ID3D11VertexShader*	vertex_shader;
	ID3D11HullShader* hull_shader;
	ID3D11DomainShader* domain_shader;
	ID3D11GeometryShader* geometry_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11ComputeShader* compute_shader;
};

struct fn_imp_backend {
	ID3D11Device*				device;
	ID3D11DeviceContext*		context;
	IDXGIFactory*				factory;
	struct fn_imp_swap_chain	swap_chains[255];
	struct fn_imp_shader 		shaders[255];
	ID3D11Buffer*				buffers[65535];
	struct fn_imp_pipeline* 	pipelines;
} static g_imp_backend;

bool fn_gfx_init() {
	g_imp_backend = (struct fn_imp_backend) {0};
	HRESULT hr = S_OK;

	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&g_imp_backend.device,
		NULL,
		&g_imp_backend.context
	);

	if(FAILED(hr))
		return false;

	IDXGIDevice* device = NULL;
	hr = ID3D11Device_QueryInterface(
		g_imp_backend.device,
		&IID_IDXGIDevice,
		(void**) &device
	);
	
	if(FAILED(hr)) {
		fn_gfx_quit();
		return false;
	}

	IDXGIAdapter* adapter = NULL;
	hr = IDXGIDevice_GetAdapter(device, &adapter);

	if(FAILED(hr)) {
		IDXGIDevice_Release(device);
		fn_gfx_quit();
		return false;
	}

	hr = IDXGIAdapter_GetParent(
		adapter,
		&IID_IDXGIFactory,
		(void**) &g_imp_backend.factory
	);

	IDXGIAdapter_Release(adapter);
	IDXGIDevice_Release(device);

	if(FAILED(hr)) {
		fn_gfx_quit();
		return false;
	}

	g_imp_backend.pipelines = malloc(sizeof(struct fn_imp_pipeline) * 65535);
	memset(g_imp_backend.pipelines, 0, sizeof(struct fn_imp_pipeline) * 65535);

	return SUCCEEDED(hr);
}

void fn_gfx_quit() {
	if(g_imp_backend.context) {
		ID3D11DeviceContext_Release(g_imp_backend.context);
		g_imp_backend.context = NULL;
	}

	if(g_imp_backend.device) {
		ID3D11Device_Release(g_imp_backend.device);
		g_imp_backend.device = NULL;
	}

}

struct fn_gfx_swap_chain fn_gfx_create_swap_chain(
	const struct fn_gfx_swap_chain_desc* desc) {

	uint8_t id = 0;
	for(id = 0; id <= 255; ++id) {
		if(id == 255)
			break;

		if(!g_imp_backend.swap_chains[id].swap_chain)
			break;
	}

	if(id == 255)
		return (struct fn_gfx_swap_chain) { id };


	DXGI_SWAP_CHAIN_DESC sc_desc = {0};
	sc_desc.BufferCount = desc->buffers;
	sc_desc.BufferDesc.Format = fn_imp_map_format(desc->format);
	sc_desc.BufferDesc.Height = desc->height;
	sc_desc.BufferDesc.Width = desc->width;
	sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.OutputWindow = (HWND) desc->window;
	sc_desc.SampleDesc.Count = 1;
	sc_desc.SampleDesc.Quality = 0;
	sc_desc.Windowed = true;

	struct fn_imp_swap_chain* sc = &g_imp_backend.swap_chains[id];
	HRESULT hr = IDXGIFactory_CreateSwapChain(
		g_imp_backend.factory,
		(IUnknown*) g_imp_backend.device,
		&sc_desc,
		&sc->swap_chain
	);

	if(FAILED(hr)) {
		return (struct fn_gfx_swap_chain) { 255 };
	}

	ID3D11Texture2D* back_buffer = NULL;
	hr = IDXGISwapChain_GetBuffer(
		sc->swap_chain,
		0,
		&IID_ID3D11Texture2D,
		(void**) &back_buffer
	);

	if(FAILED(hr)) {
		IDXGISwapChain_Release(sc->swap_chain);
		sc->swap_chain = NULL;
		return (struct fn_gfx_swap_chain) { 255 };
	}

	hr = ID3D11Device_CreateRenderTargetView(
		g_imp_backend.device,
		(ID3D11Resource*) back_buffer,
		NULL,
		&sc->render_target
	);

	ID3D11Texture2D_Release(back_buffer);
	
	if(FAILED(hr)) {
		IDXGISwapChain_Release(sc->swap_chain);
		sc->swap_chain = NULL;
		return (struct fn_gfx_swap_chain) { 255 };
	}

	return (struct fn_gfx_swap_chain) { id };
}

struct fn_gfx_shader fn_gfx_create_shader(struct fn_gfx_shader_desc* desc) {
	uint8_t id = 0;
	for(; id <= 255; ++id) {
		if(id == 255)
			return (struct fn_gfx_shader) { id };

		if(g_imp_backend.shaders[id].byte_code == NULL)
			break;
	}

	struct fn_imp_shader* sh = &g_imp_backend.shaders[id];
	sh->type = desc->type;

	HRESULT hr = S_OK;
	switch(desc->type) {
		case fn_gfx_shader_type_vertex:
			hr = ID3D11Device_CreateVertexShader(
				g_imp_backend.device,
				desc->byte_code,
				desc->byte_code_size,
				NULL,
				&sh->vertex
			);
			break;

		case fn_gfx_shader_type_hull:
			hr = ID3D11Device_CreateHullShader(
				g_imp_backend.device,
				desc->byte_code,
				desc->byte_code_size,
				NULL,
				&sh->hull
			);
			break;

		case fn_gfx_shader_type_domain:
			hr = ID3D11Device_CreateDomainShader(
				g_imp_backend.device,
				desc->byte_code,
				desc->byte_code_size,
				NULL,
				&sh->domain
			);
			break;

		case fn_gfx_shader_type_geometry:
			hr = ID3D11Device_CreateGeometryShader(
				g_imp_backend.device,
				desc->byte_code,
				desc->byte_code_size,
				NULL,
				&sh->geometry
			);
			break;

		case fn_gfx_shader_type_pixel:
			hr = ID3D11Device_CreatePixelShader(
				g_imp_backend.device,
				desc->byte_code,
				desc->byte_code_size,
				NULL,
				&sh->pixel
			);
			break;

		case fn_gfx_shader_type_compute:
			hr = ID3D11Device_CreateComputeShader(
				g_imp_backend.device,
				desc->byte_code,
				desc->byte_code_size,
				NULL,
				&sh->compute
			);
			break;
	}

	if(FAILED(hr)) {
		sh->type = 0;
		return (struct fn_gfx_shader) { 255 };
	}

	sh->byte_code = (uint8_t*) malloc(sizeof(uint8_t) * desc->byte_code_size);
	sh->byte_code_size = desc->byte_code_size;
	memcpy(sh->byte_code, desc->byte_code, desc->byte_code_size);

	return (struct fn_gfx_shader) { id };
}

struct fn_gfx_buffer fn_gfx_create_buffer(const struct fn_gfx_buffer_desc* desc) {
	uint16_t id = 0;
	for(; id <= 65535; ++id) {
		if(id == 65535)
			return (struct fn_gfx_buffer) { id };

		if(g_imp_backend.buffers[id] == NULL)
			break;
	}

	D3D11_BUFFER_DESC bdesc = {0};
	bdesc.ByteWidth = desc->capacity;
	bdesc.Usage = fn_imp_map_usage(desc->usage);
	bdesc.BindFlags = fn_imp_map_bind_flags(desc->type);
	bdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bdesc.MiscFlags = 0;
	bdesc.StructureByteStride = desc->stride;

	ID3D11Buffer** buffer = &g_imp_backend.buffers[id];
	HRESULT hr = ID3D11Device_CreateBuffer(
		g_imp_backend.device,
		&bdesc,
		NULL,
		buffer
	);

	if(FAILED(hr))
		return (struct fn_gfx_buffer) { -1 };

	return (struct fn_gfx_buffer) { id };
}

struct fn_gfx_pipeline fn_gfx_create_pipeline(struct fn_gfx_pipeline_desc* desc) {
	assert(desc != NULL);

	uint16_t id = 0;
	for(; id <= 65535; ++id) {
		if(id == 65535)
			return (struct fn_gfx_pipeline) { 65535 };

		if(g_imp_backend.pipelines[id].layout == NULL)
			break;
	}

	struct fn_imp_shader* vs = &g_imp_backend.shaders[desc->vertex_shader.id];
	struct fn_imp_shader* ps = &g_imp_backend.shaders[desc->pixel_shader.id];

	ID3D11ShaderReflection* reflector = NULL;
	HRESULT hr = D3DReflect(
		vs->byte_code,
		vs->byte_code_size,
		&IID_ID3D11ShaderReflection,
		(void**) &reflector
	);

	if(FAILED(hr))
		return (struct fn_gfx_pipeline) { 65535 };

	size_t it = 0;
	D3D11_INPUT_ELEMENT_DESC idesc[256] = {0};
	for(; it < 256; ++it) {
		if(desc->layout.elements[it].step == fn_gfx_input_element_step_none)
			break;

		D3D11_SIGNATURE_PARAMETER_DESC input_slot = {0};
		// ID3D11ShaderReflection_GetInputParameterDesc missing
		reflector->lpVtbl->GetInputParameterDesc(
			reflector,
			(UINT) it,
			&input_slot
		);

		idesc[it].SemanticName = input_slot.SemanticName;
		idesc[it].SemanticIndex = input_slot.SemanticIndex;
		idesc[it].Format = fn_imp_map_format(desc->layout.elements[it].format);
		idesc[it].InputSlot = 0;
		idesc[it].AlignedByteOffset = 0;
		idesc[it].InputSlotClass = desc->layout.elements[it].step 
			== fn_gfx_input_element_step_vertex
			? D3D11_INPUT_PER_VERTEX_DATA
			: D3D11_INPUT_PER_INSTANCE_DATA;
		idesc[it].InstanceDataStepRate = 0;
	}

	reflector->lpVtbl->Release(reflector);

	struct fn_imp_pipeline* pipeline = &g_imp_backend.pipelines[id];
	hr = ID3D11Device_CreateInputLayout(
		g_imp_backend.device,
		idesc,
		(UINT) it,
		(const void*) vs->byte_code,
		(SIZE_T) vs->byte_code_size,
		&pipeline->layout
	);

	if(FAILED(hr)) {
		return (struct fn_gfx_pipeline) { 65535 };
	}

	// TODO: How to determine whether a shader was passed or not?
	// PROBLEM: 0 is a valid id, which would be a default.
	// For now assume that a pixel- and vertex-shader is given.
	pipeline->vertex_shader = vs->vertex;
	ID3D11VertexShader_AddRef(vs->vertex);

	pipeline->pixel_shader = ps->pixel;
	ID3D11PixelShader_AddRef(ps->pixel);

	return (struct fn_gfx_pipeline) { id };
}

void fn_gfx_update_buffer(struct fn_gfx_buffer buffer, 
	struct fn_gfx_buffer_data_desc data) {

	if(buffer.id == 65535 || g_imp_backend.buffers[buffer.id] == NULL)
		return;

	ID3D11Buffer* d3d_buffer = g_imp_backend.buffers[buffer.id];
	D3D11_SUBRESOURCE_DATA d3d_data = {0};
	d3d_data.pSysMem = data.data;

	D3D11_MAPPED_SUBRESOURCE res;
	ID3D11DeviceContext_Map(
		g_imp_backend.context,
		(ID3D11Resource*) d3d_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&res
	);

	memcpy(res.pData, data.data, data.size);

	ID3D11DeviceContext_Unmap(
		g_imp_backend.context,
		(ID3D11Resource*) d3d_buffer,
		0
	);
}

void fn_gfx_swap_chain_present(struct fn_gfx_swap_chain sc) {
	if(sc.id == 255)
		return;

	IDXGISwapChain* scp = g_imp_backend.swap_chains[sc.id].swap_chain;
	IDXGISwapChain_Present(scp, 0, 0);
}

void fn_gfx_clear_target(struct fn_gfx_swap_chain sc, float (*rgba)[4]) {
	if(sc.id == 255)
		return;

	ID3D11RenderTargetView* rtv = g_imp_backend.swap_chains[sc.id].render_target;

	ID3D11DeviceContext_ClearRenderTargetView(
		g_imp_backend.context,
		rtv,
		rgba
	);
}

void fn_gfx_apply_pipeline(struct fn_gfx_pipeline pipeline) {
	if(pipeline.id == 65535)
		return;

	struct fn_imp_pipeline* pip = &g_imp_backend.pipelines[pipeline.id];
	ID3D11DeviceContext_IASetInputLayout(
		g_imp_backend.context,
		pip->layout
	);

	ID3D11DeviceContext_PSSetShader(
		g_imp_backend.context,
		pip->pixel_shader, 
		NULL,
		0
	);

	ID3D11DeviceContext_VSSetShader(
		g_imp_backend.context,
		pip->vertex_shader,
		NULL,
		0
	);

	ID3D11DeviceContext_IASetPrimitiveTopology(
		g_imp_backend.context,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
}

void fn_gfx_apply_bindings(struct fn_gfx_buffer_binding bindings) {
	ID3D11Buffer* buffers[256];
	UINT strides[256] = {3 * sizeof(float)};
	UINT offsets[256] = {0};

	for(size_t it = 0; it < bindings.buffer_count; ++it) {
		buffers[it] = g_imp_backend.buffers[bindings.buffers[it].id];
		strides[it] = bindings.strides[it];
	}

	ID3D11DeviceContext_IASetVertexBuffers(
		g_imp_backend.context,
		0,
		bindings.buffer_count,
		buffers,
		strides,
		offsets
	);
}

void fn_gfx_draw() {
	D3D11_VIEWPORT viewport = {0};
	viewport.Width = 300;
	viewport.Height = 300;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	ID3D11DeviceContext_RSSetViewports(
		g_imp_backend.context,
		1,
		&viewport
	);

	ID3D11DeviceContext_Draw(
		g_imp_backend.context,
		3, 
		0
	);
}

void fn_gfx_set_canvas(struct fn_gfx_swap_chain sc) {
	if(sc.id == 255)
		return;

	ID3D11RenderTargetView* rtv = g_imp_backend.swap_chains[sc.id].render_target;
	ID3D11DeviceContext_OMSetRenderTargets(
		g_imp_backend.context,
		1, 
		&rtv, 
		NULL
	);
}


#endif 	// _WIN32