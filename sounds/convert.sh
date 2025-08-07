rm ../sd-card/sounds/*.wav
ffmpeg -i death.wav -ac 1 -ar 8000 -acodec pcm_s16le ../sd-card/sounds/death.wav
ffmpeg -i bounce.wav -ac 1 -ar 8000 -acodec pcm_s16le ../sd-card/sounds/bounce.wav
ffmpeg -i count.wav -ac 1 -ar 8000 -acodec pcm_s16le ../sd-card/sounds/count.wav
ffmpeg -i portal.wav -ac 1 -ar 8000 -acodec pcm_s16le ../sd-card/sounds/portal.wav
ffmpeg -i win.wav -ac 1 -ar 8000 -acodec pcm_s16le ../sd-card/sounds/win.wav
ffmpeg -i slow.wav -ac 1 -ar 8000 -acodec pcm_s16le ../sd-card/sounds/slow.wav
ffmpeg -i silence.wav -ac 1 -ar 8000 -acodec pcm_s16le ../sd-card/sounds/silence.wav

# ffmpeg -i silence.wav -acodec pcm_s16le -f s16le -ac 1 -ar 16000

ffmpeg -i bounce.wav -ac 1 -ar 8000 -f s16le -acodec pcm_s16le ../sd-card/sounds/bounce.pcm
ffmpeg -i silence.wav -ac 1 -ar 8000 -f s16le -acodec pcm_s16le ../sd-card/sounds/silence.pcm
