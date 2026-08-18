#include "shaders.h"
#include <SDL3/SDL.h>

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("AMDGPU GPUVM Crash PoC", 800, 600, 0);
  SDL_GPUDevice *device =
      SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

  SDL_ClaimWindowForGPUDevice(device, window);

  SDL_GPUShader *vertexShader =
      SDL_CreateGPUShader(device, &(SDL_GPUShaderCreateInfo){
                                      .code = VertexShader,
                                      .code_size = 1188,
                                      .entrypoint = "main",
                                      .stage = SDL_GPU_SHADERSTAGE_VERTEX,
                                      .format = SDL_GPU_SHADERFORMAT_SPIRV,
                                      .num_samplers = 0,
                                      .num_storage_buffers = 0,
                                      .num_storage_textures = 0,
                                      .num_uniform_buffers = 0,
                                  });

  // This shader was compiled with a uniform buffer in use. We will not be
  // binding that uniform buffer. This causes a crash.
  SDL_GPUShader *fragmentShader =
      SDL_CreateGPUShader(device, &(SDL_GPUShaderCreateInfo){
                                      .code = FragmentShaderVuln,
                                      .code_size = 716,
                                      .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
                                      .entrypoint = "main",
                                      .format = SDL_GPU_SHADERFORMAT_SPIRV,
                                      .num_samplers = 0,
                                      .num_storage_buffers = 0,
                                      .num_storage_textures = 0,
                                      .num_uniform_buffers = 1,
                                  });

  SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(
      device,
      &(SDL_GPUGraphicsPipelineCreateInfo){
          .target_info = {.num_color_targets = 1,
                          .color_target_descriptions =
                              (SDL_GPUColorTargetDescription[]){
                                  {.format = SDL_GetGPUSwapchainTextureFormat(
                                       device, window)}}},
          .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
          .vertex_shader = vertexShader,
          .fragment_shader = fragmentShader,
      });

  bool shouldQuit = false;

  while (!shouldQuit) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        shouldQuit = true;
      }
    }

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUTexture *swapchainTexture;
    Uint32 sizeX, sizeY;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window,
                                          &swapchainTexture, &sizeX, &sizeY);

    if (swapchainTexture != NULL) {
      SDL_GPUColorTargetInfo colorTargetInfo = {0};
      colorTargetInfo.texture = swapchainTexture;
      colorTargetInfo.clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 1.0f};
      colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
      colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
      SDL_GPURenderPass *renderPass =
          SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);
      SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
      SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
      SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }
  return 0;
}
