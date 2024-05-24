#include "../logger/logger.hpp"
#include "kiero.h"

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#include "dx11_impl.hpp"
#include "menu.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Menu {
    namespace DX11 {
        HWND window = nullptr;

        LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);

            ImGuiIO &io = ImGui::GetIO();
            if (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput) {
                return true;
            }

            return CallWindowProc(wndProc, hwnd, uMsg, wParam, lParam);
        }

        long __stdcall hookPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags) {
            if (!Menu::initialized) {
                DXGI_SWAP_CHAIN_DESC desc;
                pSwapChain->GetDesc(&desc);

                ID3D11Device *device;
                pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&device);

                device->GetImmediateContext(&context);

                ID3D11Texture2D *backBuffer;
                pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer);
                device->CreateRenderTargetView(backBuffer, NULL, &mainRenderTargetView);
                backBuffer->Release();

                ImGui::CreateContext();
                ImGuiIO &io = ImGui::GetIO();
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
                io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
                ImGui_ImplWin32_Init(desc.OutputWindow);
                ImGui_ImplDX11_Init(device, context);

                Menu::initialized = true;

                Logger::success("Menu initialized with D3D11 backend");

                Menu::postInit();

                window = desc.OutputWindow;
                wndProc = (WNDPROC)SetWindowLongPtr((HWND)desc.OutputWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);
            }

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            Menu::render();

            ImGui::Render();
            context->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            return present(pSwapChain, SyncInterval, Flags);
        }

        bool init() {
            kiero::init(kiero::RenderType::D3D11);
            kiero::bind(8, (void **)&present, (void *)hookPresent);

            return true;
        }

        void shutdown() {
            kiero::unbind(8);
            kiero::shutdown();
            SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)wndProc);

            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }
    } // namespace DX11
} // namespace Menu