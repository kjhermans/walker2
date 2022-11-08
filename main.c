/*****************************************************************************/
/*
 * PiSHi LE (Lite edition) - Fundamentals of the King's Crook graphics engine.
 * 
 *   by EMMIR 2018-2022
 *   
 *   YouTube: https://www.youtube.com/c/LMP88
 *   
 * This software is released into the public domain.
 */
/*****************************************************************************/

#include "config.h"
#include "pl.h"

/*  main.c
 * 
 * Basic demo showing how to define a 3D scene, generate geometry,
 * import geometry, implement first person camera controls, and transform
 * the geometry.
 * 
 * Controls:
 *      Arrow keys - looking
 *      W/A/S/D keys - movement
 *      T/G keys - move up and down
 *      C - cycle through culling modes
 *      1 - flat rendering
 *      2 - textured rendering
 *      3 - toggle between two FOVs
 *      SPACE - start/stop dynamic transformation
 * 
 */

#include <stdlib.h>
#include <stdio.h>

#include "fw/fw.h"

#include "world.h"
#include "overlay.h"

extern void *
EXT_calloc(unsigned n, unsigned esz)
{
    return calloc(n, esz);
}

extern void
EXT_free(void *p)
{
    free(p);
}

extern void
EXT_error(int err_id, char *modname, char *msg)
{
    printf("vx error 0x%x in %s: %s\n", err_id, modname, msg);
    sys_kill();
    getchar();
    exit(0);
}

#define VW 896
#define VH 504
/* grid size */
#define GRSZ 10
/* movement speed */
static int MOVSPD=4;


static struct PL_OBJ *floortile;
static struct PL_OBJ *bluecube;
static struct PL_OBJ *redcube;
static struct PL_OBJ *yellowcube;
static struct PL_OBJ *greencube;
static int camrx = 50, camry = 0;
static int vv = 0;
static int x = CUSZ/2, y = 4000, z = CUSZ/2;
//static struct PL_TEX checktex;
//static int checker[PL_REQ_TEX_DIM * PL_REQ_TEX_DIM];
static unsigned fpsclock = 0;
static text_object_t* coords;
static text_object_t* compass;
static text_object_t* helptext;
static int showtext = 0;
static int airplanemode = 0;




#include "pics/rock.h"
static struct PL_TEX rocktex;
static unsigned char rockimg[ PL_REQ_TEX_DIM ][ PL_REQ_TEX_DIM ][ 4 ] = IMG;
static struct PL_OBJ* rockcube;

static
void maketex_rock
  ()
{
  PL_texture(&rocktex);
  rocktex.data = (int*)rockimg;
  rockcube = PL_gen_box(CUSZ, CUSZ, CUSZ, PL_ALL, 255, 255, 255);
  PL_texture(NULL);
}



#undef IMG
#include "pics/wood.h"
static struct PL_TEX woodtex;
static unsigned char woodimg[ PL_REQ_TEX_DIM ][ PL_REQ_TEX_DIM ][ 4 ] = IMG;
static struct PL_OBJ* woodcube;

static
void maketex_wood
  ()
{
  PL_texture(&woodtex);
  woodtex.data = (int*)woodimg;
  woodcube = PL_gen_box(CUSZ, CUSZ, CUSZ, PL_ALL, 255, 255, 255);
  PL_texture(NULL);
}



static void
init
  (void)
{

  maketex_rock();
  maketex_wood();

  floortile = PL_gen_box(CUSZ, CUSZ, CUSZ, PL_TOP, 77, 101, 94);

  bluecube = PL_gen_box(CUSZ, CUSZ, CUSZ, PL_ALL, 128, 0, 255);
  redcube = PL_gen_box(CUSZ, CUSZ, CUSZ, PL_ALL, 255, 0, 0);
  yellowcube = PL_gen_box(CUSZ, CUSZ, CUSZ, PL_ALL, 128, 128, 0);
  greencube = PL_gen_box(CUSZ, CUSZ, CUSZ, PL_ALL, 0, 128, 0);

  PL_fov = 9;
    
  PL_cur_tex = NULL;
  PL_cull_mode = PL_CULL_BACK;
  PL_raster_mode = PL_TEXTURED;

  coords = overlay_add_textobject(30, 30, "");
  compass = overlay_add_textobject(30, 50, "");
  helptext = overlay_add_textobject(
    30, 70,
    "         ==== Key Bindings ====\n"
    "h        This text.\n"
    "q        Toggle data display.\n"
    "w,s      (non airplane mode) Move.\n"
    "t,g      Accellerate, decellerate.\n"
    "arrows   (airplane mode) Move (non) Look.\n"
    "space    (non airplane mode) Jump.\n"
    "esc      Exit.\n"
  );
  text_object_set_visibility(helptext, 0);
        
  fpsclock = clk_sample();
}

static void
update
  (void)
{
  int dx = 0, dy = 0, dz = 0;

  if (pkb_key_pressed(FW_KEY_ESCAPE)) {
    sys_shutdown();
  }
  if (pkb_key_held(FW_KEY_ARROW_RIGHT)) {
    camry += 1;
    if (camry > 256) { camry -= 256; }
  }
  if (pkb_key_held(FW_KEY_ARROW_LEFT)) {
    camry -= 1;
    if (camry < 0) { camry += 256; }
  }
  if (pkb_key_held(FW_KEY_ARROW_UP)) {
    camrx -= 1;
    if (camrx < 0) { camrx += 256; }
  }
  if (pkb_key_held(FW_KEY_ARROW_DOWN)) {
    camrx += 1;
    if (camrx > 256) { camrx -= 256; }
  }

  if (pkb_key_held('w') || airplanemode) {
    dx = (MOVSPD * PL_sin[camry & PL_TRIGMSK]) >> PL_P;
    if (airplanemode) {
      dy = -((MOVSPD * PL_sin[camrx & PL_TRIGMSK]) >> PL_P);
    }
    dz = (MOVSPD * PL_cos[camry & PL_TRIGMSK]) >> PL_P;
  }
  if (pkb_key_held('s')) {
    dx = -((MOVSPD * PL_sin[camry & PL_TRIGMSK]) >> PL_P);
    if (airplanemode) {
      dy = ((MOVSPD * PL_sin[camrx & PL_TRIGMSK]) >> PL_P);
    }
    dz = -((MOVSPD * PL_cos[camry & PL_TRIGMSK]) >> PL_P);
  }
  if (pkb_key_pressed('a')) {
    airplanemode = !airplanemode;
  }

/*
  if (pkb_key_held('a')) {
          x -= (MOVSPD * PL_cos[camry & PL_TRIGMSK]) >> PL_P;
          z += (MOVSPD * PL_sin[camry & PL_TRIGMSK]) >> PL_P;
  }
  if (pkb_key_held('d')) {
          x += (MOVSPD * PL_cos[camry & PL_TRIGMSK]) >> PL_P;
          z -= (MOVSPD * PL_sin[camry & PL_TRIGMSK]) >> PL_P;
  }
*/

  if (pkb_key_pressed('h')) {
    text_object_set_visibility(helptext, !(helptext->visible) );
  }

  if (pkb_key_held('t')) { /* accellerate */
    if (airplanemode) {
      MOVSPD++;
      if (MOVSPD > 16) {
        MOVSPD = 16;
      }
    }
  }
  if (pkb_key_held('g')) { /* decellerate */
    if (airplanemode) {
      MOVSPD--;
      if (MOVSPD < 4) {
        MOVSPD = 4;
      }
    }
  }
  if (pkb_key_pressed('l')) { /* 'level' */
          camrx = 0;
  }
  if (pkb_key_pressed('n')) { /* 'north' */
          camry = 0;
  }
  if (pkb_key_pressed('q')) { /* dashboard */
    showtext = !showtext;
    if (showtext) {
      text_object_set_visibility(coords, 1);
      text_object_set_visibility(compass, 1);
    } else {
      text_object_set_visibility(coords, 0);
      text_object_set_visibility(compass, 0);
    }
  }
  if (pkb_key_pressed('c')) {
    static int cmod = PL_CULL_BACK;
    if (cmod == PL_CULL_BACK) {
      cmod = PL_CULL_NONE;
    } else if (cmod == PL_CULL_FRONT) {
      cmod = PL_CULL_BACK;
    } else {
      cmod = PL_CULL_FRONT;
    }
    PL_cull_mode = cmod;
  }
/*
  if (pkb_key_held('1')) {
      PL_raster_mode = PL_FLAT;
  }
  if (pkb_key_held('2')) {
      PL_raster_mode = PL_TEXTURED;
  }
*/

  if (pkb_key_pressed('3')) {
    if (PL_fov == 8) {
      PL_fov = 9;
    } else {
      PL_fov = 8;
    }
    printf("fov: %d\n", PL_fov);
  }

  if (dx) {
    if ((x / CUSZ) != ((x + (8 * dx)) / CUSZ)) {
      struct pillar pillar;
      pillar_get_real(x + (8 * dx), z + (8 * dz), &pillar);
      if (((y / CUSZ) - 2) >= (int)(pillar.height)) {
        x += dx;
      }
    } else {
      x += dx;
    }
  }
  if (dy) {
    y += dy;
  }
  if (dz) {
    if ((z / CUSZ) != ((z + (8 * dz)) / CUSZ)) {
      struct pillar pillar;
      pillar_get_real(x + (8 * dx), z + (8 * dz), &pillar);
      if (((y / CUSZ) - 2) >= (int)(pillar.height)) {
        z += dz;
      }
    } else {
      z += dz;
    }
  }
  if (pkb_key_pressed(' ') && vv == 0) { /* jump */
    vv = -JUMPSPEED;
  }

  if (!airplanemode) { /* perform falling check */
    int yy = (y / CUSZ) - 2;
    struct pillar pillar;
    pillar_get_real(x, z, &pillar);
    if (yy < (int)(pillar.height)) {
      vv = 0;
      y = ((pillar.height+2) * CUSZ);
    } else if (yy > (int)(pillar.height)) {
      if (vv == 0) {
        vv = INITIALVELOCITY;
      } else if (vv < TERMINALVELOCITY) {
        vv++;
      }
    }
    if (vv) {
      y -= vv;
    }
  }

}

static void
display(void)
{

  /* clear viewport to black */
  PL_clear_vp(0, 0, 0);
  //PL_clear_vp(255, 255, 255); /* white background for fade to white */
  PL_polygon_count = 0;
    
  /* define camera orientation */
  PL_set_camera(x, y, z, camrx, camry);

  {
    int xx = x / CUSZ; if (x < 0) { --xx; }
    int zz = z / CUSZ; if (z < 0) { --zz; }
    struct pillar pillar;
    for (int i = xx - (FIELD_SIZE/2); i < xx + (FIELD_SIZE/2); i++) {
      for (int j = zz - (FIELD_SIZE/2); j < zz + (FIELD_SIZE/2); j++) {
        { /* floortile */
          PL_mst_push();
          PL_mst_translate(i * CUSZ, 0, j * CUSZ);
          PL_render_object(floortile);
          PL_mst_pop();
        }
        pillar_get(i, j, &pillar);
        for (unsigned k = 0; k < pillar.height; k++) {
          struct PL_OBJ* obj = NULL;
          switch (pillar.block[ k ]) {
          case 1: obj = bluecube; break;
          case 2: obj = redcube; break;
          case 3: obj = woodcube; break; //yellowcube; break;
          case 4: obj = rockcube; break; //greencube; break;
          default: goto BREAKPILLAR;
          }
          if (obj)
          { /* draw textured cube */
            PL_mst_push();
            PL_mst_translate(i * CUSZ + (CUSZ/2), (k+1) * CUSZ + (CUSZ/2), j * CUSZ + (CUSZ/2));
            PL_render_object(obj);
            PL_mst_pop();
          }
        }
BREAKPILLAR: ;
      }
    }
  }
  if (showtext)
  {
    char buf[ 1024 ];
    snprintf(buf, sizeof(buf), "X:%d, Y:%d, Z:%d", x, y, z);
    text_object_set_text(coords, buf);
    snprintf(
      buf, sizeof(buf),
      "D:%d, B:%d, S:%d"
      , (camry % 256) * 90 / 64
      , (camrx % 256) * 90 / 64
      , MOVSPD
    );
    text_object_set_text(compass, buf);
  }
    
  if (clk_sample() > fpsclock) {
    fpsclock = clk_sample() + 1000;
    printf("FPS: %d, x: %d, y: %d, z: %d\n", sys_getfps(), x, y, z);
  }

  /* update window and sync */
  vid_blit();
  vid_sync();
}

int
main
  (int argc, char **argv)
{
    if (argc != 1) {
        printf("note: %s does not take any arguments.\n", argv[0]);
    }
    
    sys_init();
    sys_updatefunc(update);
    sys_displayfunc(display);

    clk_mode(FW_CLK_MODE_HIRES);
    pkb_reset();
    sys_sethz(70);
    sys_capfps(0);

    if (vid_open("PL", VW, VH, 1, FW_VFLAG_VIDFAST) != FW_VERR_OK) {
        FW_error("unable to create window\n");
        return 1;
    }

    /* give the video memory to PL */
    PL_init(vid_getinfo()->video, VW, VH);

pillar_init();
    
    init();
    sys_start();

    sys_shutdown();
    return 0;
}
