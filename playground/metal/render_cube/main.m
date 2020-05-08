#include <fundament/fundament.h>

#include <AppKit/AppKit.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <simd/SIMD.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vertex;

const vertex vertices[] = {
    -1.0f,-1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f, 1.0f,
    1.0f,-1.0f,-1.0f, 1.0f,
    1.0f, 1.0f,-1.0f, 1.0f,
    1.0f,-1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f, 1.0f,
    1.0f, 1.0f,-1.0f, 1.0f,
    1.0f,-1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f, 1.0f
};

// Adapted from
// https://github.com/metal-by-example/sample-code/blob/master/objc/05-Lighting/Lighting/MBEMathUtilities.m
//
matrix_float4x4 matrix_float4x4_perspective(float aspect, float fovy, float near, float far)
{
    float yScale = 1 / tan(fovy * 0.5);
    float xScale = yScale / aspect;
    float zRange = far - near;
    float zScale = -(far + near) / zRange;
    float wzScale = -2 * far * near / zRange;

    vector_float4 P = { xScale, 0, 0, 0 };
    vector_float4 Q = { 0, yScale, 0, 0 };
    vector_float4 R = { 0, 0, zScale, -1 };
    vector_float4 S = { 0, 0, wzScale, 0 };

    matrix_float4x4 mat = { P, Q, R, S };
    return mat;
}

matrix_float4x4 matrix_float4x4_camera_pos(float x, float y, float z) {
	vector_float4 X = {1, 0, 0, 0};
	vector_float4 Y = {0, 1, 0, 0};
	vector_float4 Z = {0, 0, 1, 0};
	vector_float4 W = {x, y, z, 1};

	matrix_float4x4 mat = {X, Y, Z, W};
	return mat;
}

matrix_float4x4 matrix_float4x4_rot_axis(vector_float3 axis, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    
    vector_float4 X;
    X.x = axis.x * axis.x + (1 - axis.x * axis.x) * c;
    X.y = axis.x * axis.y * (1 - c) - axis.z * s;
    X.z = axis.x * axis.z * (1 - c) + axis.y * s;
    X.w = 0.0;
    
    vector_float4 Y;
    Y.x = axis.x * axis.y * (1 - c) + axis.z * s;
    Y.y = axis.y * axis.y + (1 - axis.y * axis.y) * c;
    Y.z = axis.y * axis.z * (1 - c) - axis.x * s;
    Y.w = 0.0;
    
    vector_float4 Z;
    Z.x = axis.x * axis.z * (1 - c) - axis.y * s;
    Z.y = axis.y * axis.z * (1 - c) + axis.x * s;
    Z.z = axis.z * axis.z + (1 - axis.z * axis.z) * c;
    Z.w = 0.0;
    
    vector_float4 W;
    W.x = 0.0;
    W.y = 0.0;
    W.z = 0.0;
    W.w = 1.0;
    
    matrix_float4x4 mat = { X, Y, Z, W };
    return mat;
}

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

	id<MTLFunction> vs = [lib newFunctionWithName:@"s3df3_vmain"];
	id<MTLFunction> ps = [lib newFunctionWithName:@"s3df3_pmain"];

	MTLDepthStencilDescriptor* dss_desc = [MTLDepthStencilDescriptor new];
	dss_desc.depthCompareFunction = MTLCompareFunctionLess;
	dss_desc.depthWriteEnabled = YES;

	id<MTLDepthStencilState> dss = [dev newDepthStencilStateWithDescriptor: dss_desc];

	id<MTLBuffer> vbuf = [dev newBufferWithBytes: vertices
										  length: sizeof(vertex) * 36
										 options: MTLResourceOptionCPUCacheModeDefault];

	id<MTLBuffer> proj_buf = [dev newBufferWithLength: sizeof(simd_float4x4)
							        	      options: MTLResourceOptionCPUCacheModeDefault];

    MTLVertexDescriptor* input_layout = [MTLVertexDescriptor new];
    input_layout.attributes[0].format = MTLVertexFormatFloat4;
    input_layout.attributes[0].offset = 0;
    input_layout.attributes[0].bufferIndex = 0;
    input_layout.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    input_layout.layouts[0].stride = sizeof(vertex);
    
	matrix_float4x4 projection = matrix_float4x4_perspective(
		800.f / 600.f,
		(2* M_PI) / 5, 
		1.0f,
		1000.f
	);

	matrix_float4x4 pos = matrix_float4x4_camera_pos(0, 0, -5);
	const matrix_float4x4 world_projection = matrix_multiply(projection, pos);

	void* ptr = [proj_buf contents];
	memcpy(ptr, &world_projection, sizeof(matrix_float4x4));

	MTLTextureDescriptor* tex_desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatDepth32Float
                                                                                        width: 800
                                                                                       height: 600
                                                                                    mipmapped: NO];
    
    tex_desc.storageMode = MTLStorageModePrivate;
	tex_desc.usage = MTLTextureUsageRenderTarget;

	id<MTLTexture> depth_tex = [dev newTextureWithDescriptor: tex_desc];

	MTLRenderPipelineDescriptor* psd = [[MTLRenderPipelineDescriptor alloc] init];
	psd.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
	psd.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    psd.vertexDescriptor = input_layout;

	[psd setVertexFunction: vs];
	[psd setFragmentFunction: ps];

	id<MTLRenderPipelineState> pipe = [dev newRenderPipelineStateWithDescriptor: psd error: nil];
	id<MTLCommandQueue> cmd = [dev newCommandQueue];

	//const size_t vertex_count = sizeof(vertices) / (sizeof(float) * 3);
    const size_t vertex_count = 36;
    
    float angle = 0;
    const vector_float3 axis = {0, 1, 0};
	struct fn_event ev = {0};
	while(ev.type != fn_event_type_window_closed) {
        
        angle += (M_PI / 2) * 0.01f;
        matrix_float4x4 rot = matrix_float4x4_rot_axis(axis, angle);
        matrix_float4x4 mat = matrix_multiply(projection, matrix_multiply(pos, rot));
        
        void* buf_ptr = [proj_buf contents];
        memcpy(buf_ptr, &mat, sizeof(matrix_float4x4));
        
		id<CAMetalDrawable> drawable = [layer nextDrawable];
		id<MTLCommandBuffer> cmd_buf = [cmd commandBuffer]; 

		MTLRenderPassDescriptor* rpd = [MTLRenderPassDescriptor renderPassDescriptor];
		rpd.colorAttachments[0].texture = drawable.texture;
		rpd.colorAttachments[0].loadAction = MTLLoadActionClear;
		rpd.colorAttachments[0].clearColor = MTLClearColorMake(0.4f, 0.6f, 0.9f, 0.0f);
		rpd.colorAttachments[0].storeAction = MTLStoreActionStore;

		rpd.depthAttachment.texture = depth_tex;
		rpd.depthAttachment.clearDepth = 1.0f;
		rpd.depthAttachment.loadAction = MTLLoadActionClear;
		rpd.depthAttachment.storeAction = MTLStoreActionDontCare;

		id<MTLRenderCommandEncoder> enc = [cmd_buf renderCommandEncoderWithDescriptor: rpd];
		[enc setRenderPipelineState: pipe];
		[enc setDepthStencilState: dss];
		[enc setVertexBuffer: vbuf offset: 0 atIndex: 0];
		[enc setVertexBuffer: proj_buf offset: 0 atIndex: 1];
		[enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount: 36];
		[enc setFrontFacingWinding: MTLWindingClockwise];
		[enc setCullMode: MTLCullModeBack];
		[enc endEncoding];

		[cmd_buf presentDrawable: drawable];
		[cmd_buf commit];

		fn_poll_event(&ev);
	}


	fn_quit();
	return 0;
}
