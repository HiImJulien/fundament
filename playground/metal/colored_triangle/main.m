#include <fundament/fundament.h>

#include <AppKit/AppKit.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

#include <stdio.h>
#include <stdlib.h>

const float vertices[] = {
    0.0f, 0.5f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,
};

const char* path = "/Users/kirsch/Repositories/fundament/build/playground/shader.metallib";

int main() {
	printf("Got here!\n");
	fn_init();
	printf("Initialized fn_init.\n");

	struct fn_window win = fn_window_create();
	fn_window_set_size(win, 800, 600);
	fn_window_set_visibility(win, true);

	id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
	NSWindow* nwin = fn_window_handle(win);
	NSView* view = nwin.contentView;
	view.wantsLayer = YES;
	view.layer = [CAMetalLayer layer];
	
	CAMetalLayer* layer = (CAMetalLayer*) view.layer;
	layer.device = dev;

	id<MTLLibrary> lib = [dev newDefaultLibrary];

	id<MTLFunction> vs = [lib newFunctionWithName:@"vertex_color_vmain"];
	id<MTLFunction> ps = [lib newFunctionWithName:@"vertex_color_pmain"];

	id<MTLBuffer> vbuf = [dev newBufferWithBytes: vertices
										  length: sizeof(float) * 12 * 2
										 options: MTLResourceOptionCPUCacheModeDefault];

	MTLRenderPipelineDescriptor* psd = [[MTLRenderPipelineDescriptor alloc] init];
	[psd setVertexFunction: vs];
	[psd setFragmentFunction: ps];
	psd.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

	id<MTLRenderPipelineState> pipe = [dev newRenderPipelineStateWithDescriptor: psd error: nil];

	id<MTLCommandQueue> cmd = [dev newCommandQueue];

	struct fn_event ev = {0};
	while(ev.type != fn_event_type_window_closed) {
		id<CAMetalDrawable> drawable = [layer nextDrawable];
		id<MTLCommandBuffer> cmd_buf = [cmd commandBuffer]; 

		MTLRenderPassDescriptor* rpd = [MTLRenderPassDescriptor renderPassDescriptor];
		rpd.colorAttachments[0].texture = drawable.texture;
		rpd.colorAttachments[0].loadAction = MTLLoadActionClear;
		rpd.colorAttachments[0].clearColor = MTLClearColorMake(0.4f, 0.6f, 0.9f, 0.0f);
		rpd.colorAttachments[0].storeAction = MTLStoreActionStore;

		id<MTLRenderCommandEncoder> enc = [cmd_buf renderCommandEncoderWithDescriptor: rpd];
		[enc setRenderPipelineState: pipe];
		[enc setVertexBuffer: vbuf offset: 0 atIndex: 0];
		[enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
		[enc endEncoding];

		[cmd_buf presentDrawable: drawable];
		[cmd_buf commit];

		fn_poll_event(&ev);
	}


	fn_quit();
	return 0;
}
