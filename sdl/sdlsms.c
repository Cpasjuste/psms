
/*
    Copyright (c) 2002, 2003 Gregory Montoir

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include "SDL.h"
#include "SDL_thread.h"
#include "shared.h"
#include "saves.h"
#include "filters.h"
#include "sdlsms.h"



static const char *rom_filename;
static t_sdl_sync sdl_sync;
static t_sdl_video sdl_video;
static t_sdl_sound sdl_sound;
static t_sdl_controls sdl_controls;
static t_sdl_joystick sdl_joystick;
static t_filterfunc filters[FILTER_NUM] = {
  filter_2xsai,
  filter_super2xsai,
  filter_supereagle,
  filter_advmame2x,
  filter_tv2x,
  filter_normal2x,
  filter_bilinear,
  filter_dotmatrix
};



/* video */

static void sdlsms_video_blit_center(SDL_Surface* screen, SDL_Surface* buffer)
{
  /* center the bitmap in the screen only if necessary */
  if(screen->w != buffer->w || screen->h != buffer->h) {
    int dx, dy;
    SDL_Rect rect;
    
    SDL_FillRect(screen, NULL, 0);
    if(screen->h > buffer->h) {
      dx = (screen->w - buffer->w) >> 1;
      dy = (screen->h - buffer->h) >> 1;
      rect.x = dx;
      rect.y = dy;
      rect.w = buffer->w - dx;
      rect.h = buffer->h - dy;
      SDL_BlitSurface(buffer, NULL, screen, &rect);
    }
    else {
      dx = (buffer->w - screen->w) >> 1;
      dy = (buffer->h - screen->h) >> 1;
      rect.x = dx;
      rect.y = dy;
      rect.w = screen->w + dx;
      rect.h = screen->h + dy;
      SDL_BlitSurface(buffer, &rect, screen, NULL);
    }
  }
  else
	  SDL_BlitSurface(buffer, NULL, screen, NULL);
  SDL_UpdateRect(screen, 0, 0, 0, 0);
}

static void sdlsms_video_take_screenshot()
{
  int status;
  char ssname[0x100];

  strcpy(ssname, rom_filename);
  sprintf(strrchr(ssname, '.'), "-%03d.bmp", sdl_video.current_screenshot);
  ++sdl_video.current_screenshot;
  SDL_LockSurface(sdl_video.surf_screen);
  status = SDL_SaveBMP(sdl_video.surf_screen, ssname);
  SDL_UnlockSurface(sdl_video.surf_screen);
  if(status == 0)
    printf("[INFO] Screenshot written to '%s'.\n", ssname);
}

static int sdlsms_video_init(int frameskip, int fullscreen, int filter)
{
  int screen_width, screen_height;
  Uint32 vidflags = SDL_SWSURFACE;

  screen_width  = (IS_GG) ? GG_SCREEN_WIDTH  : SMS_SCREEN_WIDTH;
  screen_height = (IS_GG) ? GG_SCREEN_HEIGHT : SMS_SCREEN_HEIGHT;

  if(filter >= 0 && filter < FILTER_NUM)
    sdl_video.current_filter = filter;
  else
    sdl_video.current_filter = -1;

  if(fullscreen) {
    vidflags |= SDL_FULLSCREEN;
  }

  if(sdl_video.current_filter != -1) {
    screen_width  <<= 1;
    screen_height <<= 1;
  }

  if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }

  /* Create the 16 bits bitmap surface (RGB 565) */
  /* Even if we are in GameGear mode, we need to allocate space for a buffer of 256x192 */
  sdl_video.surf_bitmap = SDL_CreateRGBSurface(SDL_SWSURFACE, SMS_SCREEN_WIDTH, 
                           SMS_SCREEN_HEIGHT + FILTER_MARGIN_HEIGHT * 2, 16, 
                           0xF800, 0x07E0, 0x001F, 0);
  if(!sdl_video.surf_bitmap) {
    printf("ERROR: can't create surface: %s.\n", SDL_GetError());
    return 0;  	  	
  }

  sdl_video.bitmap_offset = sdl_video.surf_bitmap->pitch * FILTER_MARGIN_HEIGHT;

  if(sdl_video.current_filter >= 0) {
    sdl_video.surf_filter = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                              SMS_SCREEN_WIDTH * 2, SMS_SCREEN_HEIGHT * 2, 
                              16, 0xF800, 0x07E0, 0x001F, 0);
    filter_init_2xsai(565);
  }

  sdl_video.surf_screen = SDL_SetVideoMode(screen_width, screen_height, 0, vidflags);
  if(!sdl_video.surf_screen) {
    printf("ERROR: can't set video mode (%dx%d): %s.\n", screen_width, screen_height, SDL_GetError());
    return 0;
  }

//  if(fullscreen) {
    SDL_ShowCursor(SDL_DISABLE);
//  }

  SDL_WM_SetCaption(SMSSDL_TITLE, NULL);
  return 1;
}

static void sdlsms_video_finish_update()
{
  if(sdl_video.current_filter != -1) {
    SDL_LockSurface(sdl_video.surf_bitmap);
	  SDL_LockSurface(sdl_video.surf_filter);
    filters[sdl_video.current_filter](
      (Uint8*)sdl_video.surf_bitmap->pixels + sdl_video.bitmap_offset,
      sdl_video.surf_bitmap->pitch, 
      (Uint8*)sdl_video.surf_filter->pixels,
      sdl_video.surf_filter->pitch,
      sdl_video.surf_bitmap->w, 
      sdl_video.surf_bitmap->h - 2 * FILTER_MARGIN_HEIGHT
    );
	  SDL_UnlockSurface(sdl_video.surf_filter);
    SDL_UnlockSurface(sdl_video.surf_bitmap);
    sdlsms_video_blit_center(sdl_video.surf_screen, sdl_video.surf_filter);
  }
  else {
    sdlsms_video_blit_center(sdl_video.surf_screen, sdl_video.surf_bitmap);
  }
}

static void sdlsms_video_update()
{
  int skip_current_frame = 0;

  if(sdl_video.frame_skip > 1)
    skip_current_frame = (sdl_video.frames_rendered % sdl_video.frame_skip == 0) ? 0 : 1;

  if(!skip_current_frame) {
    sms_frame(0);
    sdlsms_video_finish_update();
  }
  else 
    sms_frame(1);

  ++sdl_video.frames_rendered;
}

static void sdlsms_video_close()
{
  if(sdl_video.surf_screen) SDL_FreeSurface(sdl_video.surf_screen);
  if(sdl_video.surf_bitmap) SDL_FreeSurface(sdl_video.surf_bitmap);
  if(sdl_video.surf_filter) SDL_FreeSurface(sdl_video.surf_filter);
  printf("[INFO] Frames rendered = %lu.\n", sdl_video.frames_rendered);
}


/* sound */

static void sdlsms_sound_callback(void *userdata, Uint8 *stream, int len)
{
  if(sdl_sound.current_emulated_samples < len) {
    memset(stream, 0, len);
  }
  else {
    memcpy(stream, sdl_sound.buffer, len);
    /* loop to compensate desync */
    do {
      sdl_sound.current_emulated_samples -= len;
    } while(sdl_sound.current_emulated_samples > 2 * len);
    memcpy(sdl_sound.buffer,
           sdl_sound.current_pos - sdl_sound.current_emulated_samples,
           sdl_sound.current_emulated_samples);
    sdl_sound.current_pos = sdl_sound.buffer + sdl_sound.current_emulated_samples;
  }
}

static int sdlsms_sound_init()
{
  int n;
  SDL_AudioSpec as_desired, as_obtained;
  
  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }

  as_desired.freq = SOUND_FREQUENCY;
  as_desired.format = AUDIO_S16LSB;
  as_desired.channels = 2;
  as_desired.samples = SOUND_SAMPLES_SIZE;
  as_desired.callback = sdlsms_sound_callback;

  if(SDL_OpenAudio(&as_desired, &as_obtained) == -1) {
    printf("ERROR: can't open audio: %s.\n", SDL_GetError());
    return 0;
  }

  if(as_desired.samples != as_obtained.samples) {
    printf("ERROR: soundcard driver does not accept specified samples size.\n");
    return 0;
  }

  sdl_sound.current_emulated_samples = 0;
  n = SOUND_SAMPLES_SIZE * 2 * sizeof(short) * 11;
  sdl_sound.buffer = (char*)malloc(n);
  if(!sdl_sound.buffer) {
    printf("ERROR: can't allocate memory for sound.\n");
    return 0;
  }
  memset(sdl_sound.buffer, 0, n);
  sdl_sound.current_pos = sdl_sound.buffer;
  return 1;
}

static void sdlsms_sound_update()
{
  int i;
  short* p;

  SDL_LockAudio();
  p = (short*)sdl_sound.current_pos;
  for(i = 0; i < snd.bufsize; ++i) {
      *p = snd.buffer[0][i];
      ++p;
      *p = snd.buffer[1][i];
      ++p;
  }
  sdl_sound.current_pos = (char*)p;
  sdl_sound.current_emulated_samples += snd.bufsize * 2 * sizeof(short);
  SDL_UnlockAudio();
}

static void sdlsms_sound_close()
{
  SDL_PauseAudio(1);
  SDL_CloseAudio();
  if(sdl_sync.sem_sync)
    SDL_DestroySemaphore(sdl_sync.sem_sync);
  free(sdl_sound.buffer);
}


/* controls */

static void sdlsms_controls_init()
{
  sdl_controls.state_slot = 0;
  sdl_controls.pad[0].up = SDLK_UP;
  sdl_controls.pad[0].down = SDLK_DOWN;
  sdl_controls.pad[0].left = SDLK_LEFT;
  sdl_controls.pad[0].right = SDLK_RIGHT;
  sdl_controls.pad[0].b1 = SDLK_c;
  sdl_controls.pad[0].b2 = SDLK_v;
  sdl_controls.pad[0].start = SDLK_RETURN;
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

static int sdlsms_controls_update(SDLKey k, int p)
{
  if(!p) {
	  switch(k) {
	  case SDLK_ESCAPE:
		  return 0;
    case SDLK_1:
    case SDLK_2:
    case SDLK_3:
    case SDLK_4:
    case SDLK_5:
    case SDLK_6:
    case SDLK_7:
    case SDLK_8:
    case SDLK_9:
      /* only 'hot change' if started with a filter */
      if(sdl_video.current_filter != -1) {
        int cur = k - SDLK_1;
        if(cur >= 0 && cur < FILTER_NUM) {
          SDL_FillRect(sdl_video.surf_filter, NULL, 0);
          sdl_video.current_filter = cur;
        }
      }
      break;
	  case SDLK_F1:
		  sdlsms_video_take_screenshot();
		  break;
	  case SDLK_F2:
		  if(save_state(rom_filename, sdl_controls.state_slot))
  		  printf("[INFO] Saved state to slot #%d.\n", sdl_controls.state_slot);
		  break;
	  case SDLK_F3:
		  if(load_state(rom_filename, sdl_controls.state_slot))
		    printf("[INFO] Loaded state from slot #%d.\n", sdl_controls.state_slot);
		  break;
    case SDLK_F4:
    case SDLK_F5:
      sdl_video.frame_skip += k == SDLK_F4 ? -1 : 1;
      if(sdl_video.frame_skip > 0) 
        sdl_video.frame_skip = 1;
      if(sdl_video.frame_skip == 1)
        printf("[INFO] Frame skip disabled.\n");
      else
        printf("[INFO] Frame skip set to %d.\n", sdl_video.frame_skip);
      break;
    case SDLK_F6:
    case SDLK_F7:
      sdl_controls.state_slot += k == SDLK_F6 ? -1 : 1;
      if(sdl_controls.state_slot < 0)
        sdl_controls.state_slot = 0;
		  printf("[INFO] Slot changed to #%d.\n", sdl_controls.state_slot);
      break;
    }
  }
  if(k == sdl_controls.pad[0].start) {
    if(p) input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
    else  input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
  }
  else if(k == sdl_controls.pad[0].up) {
    if(p) input.pad[0] |= INPUT_UP;
    else  input.pad[0] &= ~INPUT_UP;
  }
  else if(k == sdl_controls.pad[0].down) {
    if(p) input.pad[0] |= INPUT_DOWN;
    else  input.pad[0] &= ~INPUT_DOWN;
  }
  else if(k == sdl_controls.pad[0].left) {
    if(p) input.pad[0] |= INPUT_LEFT;
    else  input.pad[0] &= ~INPUT_LEFT;
  }
  else if(k == sdl_controls.pad[0].right) {
    if(p) input.pad[0] |= INPUT_RIGHT;
    else  input.pad[0] &= ~INPUT_RIGHT;
  }
  else if(k == sdl_controls.pad[0].b1) {
    if(p) input.pad[0] |= INPUT_BUTTON1;
    else  input.pad[0] &= ~INPUT_BUTTON1;
  }
  else if(k == sdl_controls.pad[0].b2) {
    if(p) input.pad[0] |= INPUT_BUTTON2;
    else  input.pad[0] &= ~INPUT_BUTTON2;
  }
  else if(k == SDLK_TAB) {
    if(p) input.system |= INPUT_HARD_RESET;
    else  input.system &= ~INPUT_HARD_RESET;
  }
  return 1;
}



/* joystick */

static int smssdl_joystick_init()
{
  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }
  else {
    sdl_joystick.commit_range = 3276;
    sdl_joystick.xstatus = 1;
    sdl_joystick.ystatus = 1;
    sdl_joystick.number = 0;
    sdl_joystick.map_b1 = 1;
    sdl_joystick.map_b2 = 2;
    sdl_joystick.map_start = 4;
    sdl_joystick.joy = SDL_JoystickOpen(sdl_joystick.number);
    if(sdl_joystick.joy != NULL) {
      printf("Joystick (%s) has %d axes and %d buttons.\n", SDL_JoystickName(sdl_joystick.number), SDL_JoystickNumAxes(sdl_joystick.joy), SDL_JoystickNumButtons(sdl_joystick.joy));
      printf("Using button mapping I=%d II=%d START=%d.\n", sdl_joystick.map_b1, sdl_joystick.map_b2, sdl_joystick.map_start);
      SDL_JoystickEventState(SDL_ENABLE);
      return 1;
    }
    else {
      printf("ERROR: Could not open joystick: %s.\n", SDL_GetError());
      return 0;
    }
  }
}


static void smssdl_joystick_update(SDL_Event event)
{
	int axisval;
  switch(event.type) {
	case SDL_JOYAXISMOTION:
		switch(event.jaxis.axis) {
		case 0:	/* X axis */
			axisval = event.jaxis.value;

			if(axisval > sdl_joystick.commit_range) {
				if(sdl_joystick.xstatus == 2)
					break;
				if(sdl_joystick.xstatus == 0) {
					input.pad[0] &= ~INPUT_LEFT;
				}
				input.pad[0] |= INPUT_RIGHT;
				sdl_joystick.xstatus = 2;
				break;
			}

			if(axisval < -sdl_joystick.commit_range) {
				if(sdl_joystick.xstatus == 0)
					break;
				if(sdl_joystick.xstatus == 2) {
				  input.pad[0] &= ~INPUT_RIGHT;
				}
				input.pad[0] |= INPUT_LEFT;
				sdl_joystick.xstatus = 0;
				break;
			}

			/* axis is centered */
			if(sdl_joystick.xstatus == 2) {
				input.pad[0] &= ~INPUT_RIGHT;
			}
			if(sdl_joystick.xstatus == 0) {
				input.pad[0] &= ~INPUT_LEFT;
			}
			sdl_joystick.xstatus = 1;
			break;

		case 1:	/* Y axis */
			axisval = event.jaxis.value;

			if(axisval > sdl_joystick.commit_range) {
				if(sdl_joystick.ystatus == 2)
					break;
				if(sdl_joystick.ystatus == 0) {
					input.pad[0] &= ~INPUT_UP;
				}
				input.pad[0] |= INPUT_DOWN;
				sdl_joystick.ystatus = 2;
				break;
			}
			
      if(axisval < -sdl_joystick.commit_range) {
				if(sdl_joystick.ystatus == 0)
					break;
				if(sdl_joystick.ystatus == 2) {
					input.pad[0] &= ~INPUT_DOWN;
				}
				input.pad[0] |= INPUT_UP;
				sdl_joystick.ystatus = 0;
				break;
			}

			/* axis is centered */
			if(sdl_joystick.ystatus == 2) {
				input.pad[0] &= ~INPUT_DOWN;
			}
			if(sdl_joystick.ystatus == 0) {
				input.pad[0] &= ~INPUT_UP;
			}
			sdl_joystick.ystatus = 1;
			break;
		}
		break;

	case SDL_JOYBUTTONDOWN:
		if(event.jbutton.button == sdl_joystick.map_b1) {
			input.pad[0] |= INPUT_BUTTON1;
		}
    else if(event.jbutton.button == sdl_joystick.map_b2) {
			input.pad[0] |= INPUT_BUTTON2;
		}
    else if(event.jbutton.button == sdl_joystick.map_start) {
			input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
		}
		break;

	case SDL_JOYBUTTONUP:
		if(event.jbutton.button == sdl_joystick.map_b1) {
			input.pad[0] &= ~INPUT_BUTTON1;
		}
    else if (event.jbutton.button == sdl_joystick.map_b2) {
			input.pad[0] &= ~INPUT_BUTTON2;
		}
    else if (event.jbutton.button == sdl_joystick.map_start) {
			input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
		}
		break;
	}
}


/* sync */

static Uint32 sdlsms_sync_timer_callback(Uint32 interval)
{
  SDL_SemPost(sdl_sync.sem_sync);
  return interval;
}

static int sdlsms_sync_init(int fullspeed)
{
  if(SDL_InitSubSystem(SDL_INIT_TIMER|SDL_INIT_EVENTTHREAD) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }
  sdl_sync.ticks_starting = SDL_GetTicks();
  if(fullspeed)
    sdl_sync.sem_sync = NULL;
  else
    sdl_sync.sem_sync = SDL_CreateSemaphore(0);
  return 1;
}

static void sdlsms_sync_close()
{
  float playtime = (float)(SDL_GetTicks() - sdl_sync.ticks_starting) / (float)1000;
  float avgfps = (float)sdl_video.frames_rendered / playtime;
  printf("[INFO] Average FPS = %.2f (%d%%).\n", avgfps, (int)(avgfps * 100 / MACHINE_FPS)); 
  printf("[INFO] Play time = %.2f sec.\n", playtime);
}


/* globals */

int sdlsms_init(const t_config* pcfg)
{
  rom_filename = pcfg->game_name;
  if(load_rom(rom_filename) == 0) {
    printf("ERROR: can't load `%s'.\n", pcfg->game_name);
    return 0;
  }
  else printf("Loaded `%s'.\n", pcfg->game_name);

  printf("Initializing SDL... ");
  if(SDL_Init(0) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }
  printf("Ok.\n");

  printf("Initializing SDL TIMER SUBSYSTEM... ");
  if(!sdlsms_sync_init(pcfg->fullspeed))
    return 0;
  printf("Ok.\n");

  printf("Initializing SDL CONTROLS SUBSYSTEM... ");
  sdlsms_controls_init();
  printf("Ok.\n");

  if(pcfg->joystick) {
    printf("Initializing SDL JOYSTICK SUBSYSTEM... ");
    if(smssdl_joystick_init())
      printf("Ok.\n");
  }

  printf("Initializing SDL VIDEO SUBSYSTEM... ");
  if(!sdlsms_video_init(pcfg->frameskip, pcfg->fullscreen, pcfg->filter))
    return 0;
  printf("Ok.\n");

  if(!pcfg->nosound) {
    printf("Initializing SDL SOUND SUBSYSTEM... ");
    if(!sdlsms_sound_init())
      return 0;
    printf("Ok.\n");
  }

  /* set up the virtual console emulation */
  SDL_LockSurface(sdl_video.surf_bitmap);
  printf("Initializing virtual console emulation... ");
  sms.use_fm = pcfg->fm;
  sms.country = pcfg->japan ? TYPE_DOMESTIC : TYPE_OVERSEAS;
  sms.save = pcfg->usesram;
  memset(&bitmap, 0, sizeof(t_bitmap));
  bitmap.width  = SMS_SCREEN_WIDTH;
  bitmap.height = SMS_SCREEN_HEIGHT;
  bitmap.depth  = sdl_video.surf_bitmap->format->BitsPerPixel;
  bitmap.pitch  = sdl_video.surf_bitmap->pitch;
  bitmap.data   = (unsigned char*)sdl_video.surf_bitmap->pixels + sdl_video.bitmap_offset;
  system_init(pcfg->nosound ? 0 : SOUND_FREQUENCY);
  load_sram(pcfg->game_name);
  SDL_UnlockSurface(sdl_video.surf_bitmap);
  printf("Ok.\n");

  return 1;
}

void sdlsms_emulate()
{
  int quit = 0;
  printf("Starting emulation...\n");

  if(snd.enabled)
    SDL_PauseAudio(0);

  if(sdl_sync.sem_sync) {
    SDL_SetTimer(50, sdlsms_sync_timer_callback);
  }

  while(!quit) {
    /* pump SDL events */
    SDL_Event event;
    if(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_KEYUP:
      case SDL_KEYDOWN:
        if(!sdlsms_controls_update(event.key.keysym.sym, event.type == SDL_KEYDOWN))
          quit = 1;
        break;
      case SDL_JOYAXISMOTION:
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        smssdl_joystick_update(event);
        break;
      case SDL_QUIT:
        quit = 1;
        break;
      }
    }
    sdlsms_video_update();
    if(snd.enabled)
      sdlsms_sound_update();

    if(sdl_sync.sem_sync && sdl_video.frames_rendered % 3 == 0)
      SDL_SemWait(sdl_sync.sem_sync);
  }
}

void sdlsms_shutdown() 
{
  /* shutdown the virtual console emulation */
  printf("Shutting down virtual console emulation...\n");
  save_sram(rom_filename);
  system_reset();
  system_shutdown();

  printf("Shutting down SDL...\n");
  if(snd.enabled)
    sdlsms_sound_close();
  sdlsms_video_close();
  sdlsms_sync_close();
  SDL_Quit();
}
