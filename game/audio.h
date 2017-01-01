#ifndef __AUDIO_H
#define __AUDIO_H

extern UWORD audio_beat1, audio_beat2, audio_beat3, audio_beat4, audio_shoot, audio_invaderkilled, audio_explosion;

extern void audio_playBeat(int beat);
extern void audio_playShoot(void);
extern void audio_playInvaderKilled(void);
extern void audio_playExplosion(void);
extern void audio_vbl(int kill);

#endif
