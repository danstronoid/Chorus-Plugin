# Chorus Plugin
by Daniel Schwartz

This chorus plugin is meant to comprehensive and support all of the features I would 
look for in a chorus effect.  It is not modelled after a particular hardware chorus, 
but rather it combines features found in different units.  It was designed with the idea 
of reproducing sounds from classic choruses as well as being flexible enough to create 
experimental effects. This project is still in development, and I plan to continue to 
add features and experiment.

Chorus Parameters: 
- Rate
- Depth
- Mix (wet/dry)
- Delay Time (minimum)
- Width (difference between L & R delay time)
- Mode (stereo, mono, dimension, vibrato)
- Voices (1 - 4 mono, 2 - 8 stereo)
- Voice Spread (Spreads delay times of voices between 5ms and the delay time)
- LFO Type (Tri or Sine)
- LFO Phase (both L & R individually) - maybe add a link option

Filter Parameters:
- High Pass Cutoff
- Low Pass Cutoff
- Filter bypass

Modulation Parameters:
- Mod Target (targets available paramters)
- Mod LFO Type (Tri or Sine)
- Mod Rate
- Mod Depth

Master Paramters:
- Input Gain
- Output Gain

# Todo:
- StateVariableFilter is depreciated, find a different filter.
- Finish adding support for 64-bit double processing
- Find a better way to manage IDs
- Customize look and feel
- Come up with a good name

Ideas for additions:
- Longer delay times (beyond traditional chorus)
- Delay feedback
- Multiple modulation sources (full mod matrix)
- Drive
- Tempo sync
- 100% wet mode (through-zero)
