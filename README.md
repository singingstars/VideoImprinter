VideoImprinter
==============

A video event note taker utility. Similar to subtitle editing but more specific for predefined event types. Written with Qt5.

Helps
-----
VideoImprinter combines a video player and an event(subtitle) editor. The two are vertically stacked together by default, and can be changed to horizontally stacked via F12 key.

When start, it will read settings in config.ini in the startup folder. It will automatically create one if it doesn't exist.

Events are added using number keys while playing video. Events are automatically sorted, possible duplicates (time overlaps AND type equals) are automatically detected.

Event types (0~9) can be defined by editing config.ini in the program start folder.

Events can be saved in srt format(same as subtitles). Whenever a video file is opened, the program will try to load/create the srt file with the same base-name in the same directroy(e.g. Video.avi -- Video.srt, if Video.srt doesn't exist, create Video.srt).

Events are auto-saved with "~" suffix, auto-save interval can be set in config.ini (in milisec).

Video playback utilizes DirectShow on windows, so additional codec support can be installed (e.g. codec packs). Output can be filtered through controls (brightness, contrast, hue, saturation).

Video can be quick jumped forward/backward using Left/Right Arrow Keys. The speed can be modified in config.ini (in milisec).

Keyboard Shortcuts:
-------------------

-Video Control
Space           Play/Pause
Left            Quick Jump Backwards 1
Ctrl + Left     Quick Jump Backwards 2
Shift + Left    Quick Jump Backwards 3

Right           Quick Jump Forward 1
Ctrl + Right    Quick Jump Forward 2
Shift + Right   Quick Jump Forward 3

-Adding Event
0~9             Add Event 0~9

-Select Event
P               Select Current Event
[               Select Previous Event
]               Select Next Event

A               Jump Video to Selected Start Time
D               Jump Video to Selected End Time

-Edit Selected Event
;               Modify/Change Start Time to now
'               Modify/Change End Time to now
Ctrl + 0~9      Modify/Change Text to 0~9
Ctrl + Del      Delete Event
G               Sort Event List

-File Operation
Ctrl + S        Save current file
Ctrl + L        Load new *.srt file

-------------------
