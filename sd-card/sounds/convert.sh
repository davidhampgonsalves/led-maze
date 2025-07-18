mkdir out
ffmpeg -i death.wav -ac 1 -ar 8000 -acodec pcm_s16le out/death.wav
ffmpeg -i bounce.wav -ac 1 -ar 8000 -acodec pcm_s16le out/bounce.wav
ffmpeg -i count.wav -ac 1 -ar 8000 -acodec pcm_s16le out/count.wav
ffmpeg -i portal.wav -ac 1 -ar 8000 -acodec pcm_s16le out/portal.wav
ffmpeg -i win.wav -ac 1 -ar 8000 -acodec pcm_s16le out/win.wav
rm *.wav
mv out/* .
rm -rf out
