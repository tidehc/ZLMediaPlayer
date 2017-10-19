#ifndef YUVDISPLAYER_H_
#define YUVDISPLAYER_H_
#include <stdexcept>
#include "Util/onceToken.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "SDL2/SDL.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
#pragma comment(lib,"SDL2.lib")
#endif //defined(_WIN32)

using namespace ZL::Util;

class YuvDisplayer {
public:
	YuvDisplayer(void *hwnd = nullptr,const char *title = "untitled"){
		_title = title;
		_hwnd = hwnd;
	}
	virtual ~YuvDisplayer(){
		if (_texture) {
			SDL_DestroyTexture(_texture);
			_texture = nullptr;
		}
		if (_render) {
			SDL_DestroyRenderer(_render);
			_render = nullptr;
		}
		if (_win) {
			SDL_DestroyWindow(_win);
			_win = nullptr;
		}
	}
	bool displayYUV(AVFrame *pFrame){
		if (!_win) {
			if (_hwnd) {
				_win = SDL_CreateWindowFrom(_hwnd);
			}else {
				_win = SDL_CreateWindow(_title.data(),
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					pFrame->width/2,
					pFrame->height/2,
					SDL_WINDOW_OPENGL);
			}
		}
		if (_win && ! _render){
                        #if 0
                    SDL_RENDERER_SOFTWARE = 0x00000001,         /**< The renderer is a software fallback */
                    SDL_RENDERER_ACCELERATED = 0x00000002,      /**< The renderer uses hardware
                                                                     acceleration */
                    SDL_RENDERER_PRESENTVSYNC = 0x00000004,     /**< Present is synchronized
                                                                     with the refresh rate */
                    SDL_RENDERER_TARGETTEXTURE = 0x00000008     /**< The renderer supports
                                                                     rendering to texture */
                        #endif
                        #if defined(__linux__)
                     		_render = SDL_CreateRenderer(_win, -1, SDL_RENDERER_SOFTWARE);
                        #else
                            _render = SDL_CreateRenderer(_win, -1, SDL_RENDERER_ACCELERATED);
                        #endif
		}
		if (_render && !_texture) {
			_texture = SDL_CreateTexture(_render, SDL_PIXELFORMAT_IYUV,
				SDL_TEXTUREACCESS_STREAMING,
				pFrame->width,
				pFrame->height);
		}
        if (_texture) {
            SDL_UpdateYUVTexture(_texture, nullptr,
                pFrame->data[0], pFrame->linesize[0],
                pFrame->data[1], pFrame->linesize[1],
                pFrame->data[2], pFrame->linesize[2]);

            //SDL_UpdateTexture(_texture, nullptr, pFrame->data[0], pFrame->linesize[0]);
            SDL_RenderClear(_render);
            SDL_RenderCopy(_render, _texture, nullptr, nullptr);
            SDL_RenderPresent(_render);
            return true;
        }
		return false;
	}
private:
	string _title;
	SDL_Window *_win = nullptr;
	SDL_Renderer *_render = nullptr;
	SDL_Texture *_texture = nullptr;
	void *_hwnd = nullptr;
};

#endif /* YUVDISPLAYER_H_ */