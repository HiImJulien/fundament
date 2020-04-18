#include <fundament/gfx.h>

#include <stdio.h>

#if defined(_WIN32)

#ifndef COBJMACROS
#define COBJMACROS
#endif 	// COBJMACROS

#include <d3d11.h>

struct fn_imp_swap_chain {
	IDXGISwapChain*			swap_chain;
	ID3D11RenderTargetView* render_target;
};

struct fn_imp_backend {
	ID3D11Device*				device;
	ID3D11DeviceContext*		context;
	IDXGIFactory*				factory;
	struct fn_imp_swap_chain	swap_chains[255];
} static g_imp_backend;

bool fn_gfx_init() {
	g_imp_backend = (struct fn_imp_backend) {0};
	HRESULT hr = S_OK;

	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
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
	sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
		printf("HR: 0x%.8lX\n", hr);
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

#endif 	// _WIN32