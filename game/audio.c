#include "game.h"

void 
audio_playBeat(int beat)
{
  static UWORD* beats[4] = {&audio_beat1, &audio_beat2, &audio_beat3, &audio_beat4};
  static UWORD beatLengths[4] = {982/2, 1042/2, 1054/2, 1098/2}; 
  struct  AudChannel *aud = &custom->aud[0];
  aud->ac_ptr = beats[beat];
  aud->ac_per = 322;
  aud->ac_vol = 64;
  aud->ac_len = beatLengths[beat];
  custom->dmacon = DMAF_AUD0|DMAF_SETCLR;
}


void 
audio_playShoot()
{
  struct  AudChannel *aud = &custom->aud[1];
  aud->ac_ptr = &audio_shoot;
  aud->ac_per = 322;
  aud->ac_vol = 64;
  aud->ac_len = 3377/2;
  custom->dmacon = DMAF_AUD1|DMAF_SETCLR;
}

void 
audio_playInvaderKilled(void)
{
  struct  AudChannel *aud = &custom->aud[2];
  aud->ac_ptr = &audio_invaderkilled;
  aud->ac_per = 322;
  aud->ac_vol = 64;
  aud->ac_len = 4080/2;
  custom->dmacon = DMAF_AUD2|DMAF_SETCLR;
}

void 
audio_playExplosion(void)
{
  custom->dmacon = DMAF_AUD0|DMAF_AUD1|DMAF_AUD2|DMAF_AUD3;
  audio_vbl(1);
  hw_waitScanLines(4);
  struct  AudChannel *aud = &custom->aud[2];
  aud->ac_ptr = &audio_explosion;
  aud->ac_per = 322;
  aud->ac_vol = 64;
  aud->ac_len = 8731/2;
  custom->dmacon = DMAF_AUD2|DMAF_SETCLR;
}


void
audio_vbl(int kill)
{
  static UWORD empty[2] = {0,0};
  
  for (int i = 0; i < 4; i++) {
    struct AudChannel *aud = &custom->aud[i];    
    aud->ac_len = 2;
    if (kill) {
      aud->ac_per = 1;
    }
    aud->ac_ptr = &empty[0];
  }
}


